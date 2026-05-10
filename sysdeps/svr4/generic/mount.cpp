#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <bits/syscall.h>
#include <sys/mount.h>
#include <sys/mntent.h>
#include <sys/mnttab.h>
#include <sys/syscall.h>

namespace {

constexpr unsigned long kSvr4MountData = 0x04;
constexpr unsigned long kSupportedMountFlags = MS_RDONLY | MS_HADBAD
		| MS_NOSUID | MS_REMOUNT | MS_NOTRUNC;
constexpr const char *kMnttabLockPath = "/etc/.mnttab.lock";
constexpr const char *kMnttabTempPath = "/etc/.mnttab.tmp";

char *find_option(char *options, const char *opt) {
	if(!options || !opt)
		return nullptr;

	size_t opt_len = strlen(opt);
	for(char *current = options; *current;) {
		char *token_end = current;
		while(*token_end && *token_end != ',')
			++token_end;
		if(static_cast<size_t>(token_end - current) == opt_len
				&& !strncmp(current, opt, opt_len))
			return current;
		if(!*token_end)
			break;
		current = token_end + 1;
	}

	return nullptr;
}

bool append_option(char *buffer, size_t size, bool *first, const char *option) {
	if(!option || !*option)
		return true;

	size_t current = strlen(buffer);
	int written = snprintf(buffer + current, size - current, "%s%s",
			*first ? "" : ",", option);
	if(written < 0 || static_cast<size_t>(written) >= size - current)
		return false;
	*first = false;
	return true;
}

bool build_mount_options(unsigned long flags, const char *data, char *buffer, size_t size) {
	buffer[0] = '\0';
	bool first = true;
	auto raw = data ? data : "";

	if(!append_option(buffer, size, &first, (flags & MS_RDONLY) ? MNTOPT_RO : MNTOPT_RW))
		return false;

	const char *suid_opt = (flags & MS_NOSUID) ? MNTOPT_NOSUID : "suid";
	if(!find_option(const_cast<char *>(raw), suid_opt)) {
		if(!append_option(buffer, size, &first, suid_opt))
			return false;
	}

	if((flags & MS_REMOUNT) && !find_option(const_cast<char *>(raw), MNTOPT_REMOUNT)) {
		if(!append_option(buffer, size, &first, MNTOPT_REMOUNT))
			return false;
	}

	if((flags & MS_NOTRUNC) && !find_option(const_cast<char *>(raw), "notrunc")) {
		if(!append_option(buffer, size, &first, "notrunc"))
			return false;
	}

	if((flags & MS_HADBAD) && !find_option(const_cast<char *>(raw), "hadbad")) {
		if(!append_option(buffer, size, &first, "hadbad"))
			return false;
	}

	if(*raw) {
		if(!append_option(buffer, size, &first, raw))
			return false;
	}

	return true;
}

int lock_mnttab() {
	int fd = open(kMnttabLockPath, O_RDWR | O_CREAT | O_CLOEXEC, 0644);
	if(fd < 0)
		return -1;

	struct flock lock{};
	lock.l_type = F_WRLCK;
	lock.l_whence = SEEK_SET;
	lock.l_start = 0;
	lock.l_len = 0;

	while(fcntl(fd, F_SETLKW, &lock) < 0) {
		if(errno != EINTR) {
			int saved = errno;
			close(fd);
			errno = saved;
			return -1;
		}
	}

	return fd;
}

int rewrite_mnttab(const struct mnttab *replacement, const char *target) {
	int lock_fd = lock_mnttab();
	if(lock_fd < 0)
		return -1;

	FILE *input = fopen(MNTTAB, "r");
	if(!input && errno != ENOENT) {
		int saved = errno;
		close(lock_fd);
		errno = saved;
		return -1;
	}

	FILE *output = fopen(kMnttabTempPath, "w");
	if(!output) {
		int saved = errno;
		if(input)
			fclose(input);
		close(lock_fd);
		errno = saved;
		return -1;
	}

	bool replaced = false;
	if(input) {
		struct mnttab current;
		while(true) {
			int ret = getmntent(input, &current);
			if(ret < 0)
				break;
			if(ret > 0) {
				int saved = EINVAL;
				fclose(input);
				fclose(output);
				unlink(kMnttabTempPath);
				close(lock_fd);
				errno = saved;
				return -1;
			}

			if(target && current.mnt_mountp && !strcmp(current.mnt_mountp, target)) {
				if(replacement) {
					if(putmntent(output, replacement) < 0) {
						int saved = errno;
						fclose(input);
						fclose(output);
						unlink(kMnttabTempPath);
						close(lock_fd);
						errno = saved;
						return -1;
					}
					replaced = true;
				}
				continue;
			}

			if(putmntent(output, &current) < 0) {
				int saved = errno;
				fclose(input);
				fclose(output);
				unlink(kMnttabTempPath);
				close(lock_fd);
				errno = saved;
				return -1;
			}
		}
		fclose(input);
	}

	if(replacement && !replaced) {
		if(putmntent(output, replacement) < 0) {
			int saved = errno;
			fclose(output);
			unlink(kMnttabTempPath);
			close(lock_fd);
			errno = saved;
			return -1;
		}
	}

	if(fclose(output) == EOF) {
		int saved = errno;
		unlink(kMnttabTempPath);
		close(lock_fd);
		errno = saved;
		return -1;
	}

	if(rename(kMnttabTempPath, MNTTAB) < 0) {
		int saved = errno;
		unlink(kMnttabTempPath);
		close(lock_fd);
		errno = saved;
		return -1;
	}

	close(lock_fd);
	return 0;
}

} // namespace

int mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data) {
	if(!source || !target || !fstype) {
		errno = EINVAL;
		return -1;
	}

	if(flags & ~kSupportedMountFlags) {
		errno = EINVAL;
		return -1;
	}

	// Linux-style mount data is commonly a NUL-terminated options string,
	// while the SVR4 syscall ABI requires an explicit byte count.
	auto mount_data = static_cast<const char *>(data);
	size_t mount_data_length = 0;
	if(mount_data)
		mount_data_length = strlen(mount_data) + 1;
	if(mount_data_length > INT_MAX) {
		errno = EINVAL;
		return -1;
	}

	if(syscall(SYS_mount, source, target, static_cast<int>(flags | kSvr4MountData),
			fstype, mount_data, static_cast<int>(mount_data_length)) < 0)
		return -1;

	char options[MNT_LINE_MAX];
	if(!build_mount_options(flags, mount_data, options, sizeof(options)))
		return 0;

	char mounted_at[32];
	time_t now = time(nullptr);
	if(now < 0)
		now = 0;
	snprintf(mounted_at, sizeof(mounted_at), "%ld", static_cast<long>(now));

	struct mnttab entry = {
		.mnt_special = const_cast<char *>(source),
		.mnt_mountp = const_cast<char *>(target),
		.mnt_fstype = const_cast<char *>(fstype),
		.mnt_mntopts = options,
		.mnt_time = mounted_at,
	};

	(void)rewrite_mnttab(&entry, target);
	return 0;
}

int umount(const char *target) {
	return umount2(target, 0);
}

int umount2(const char *target, int flags) {
	if(!target) {
		errno = EINVAL;
		return -1;
	}

	if(flags) {
		errno = ENOSYS;
		return -1;
	}

	if(syscall(SYS_umount, target) < 0)
		return -1;

	(void)rewrite_mnttab(nullptr, target);
	return 0;
}