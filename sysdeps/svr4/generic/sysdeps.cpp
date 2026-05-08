#include <errno.h>
#include <string.h>

#include <abi-bits/errno.h>
#include <abi-bits/fcntl.h>
#include <abi-bits/ioctls.h>
#include <abi-bits/termios.h>
#include <abi-bits/vm-flags.h>
#include <bits/syscall.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/tcb.hpp>

static long raw_syscall(long number);

template<typename Arg0>
static long raw_syscall(long number, Arg0 arg0);

template<typename Arg0, typename Arg1>
static long raw_syscall(long number, Arg0 arg0, Arg1 arg1);

template<typename Arg0, typename Arg1, typename Arg2>
static long raw_syscall(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2);

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
static long raw_syscall(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3);

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
static long raw_syscall(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4);

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
static long raw_syscall(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5);

static int syscall_error(long ret);

namespace {

struct ssd {
	unsigned int sel;
	unsigned int bo;
	unsigned int ls;
	unsigned int acc1;
	unsigned int acc2;
};

constexpr int kSi86Dscr = 75;
constexpr unsigned int kUserDataAcc1 = 0xF2;
constexpr unsigned int kDataAcc2 = 0xC;
constexpr unsigned int kFirstTlsSelectorIndex = 7;
constexpr unsigned int kMaxLdtIndex = 8192;
constexpr int kMsggetSubcode = 0;
constexpr int kMsgctlSubcode = 1;
constexpr int kMsgrcvSubcode = 2;
constexpr int kMsgsndSubcode = 3;
constexpr int kShmatSubcode = 0;
constexpr int kShmctlSubcode = 1;
constexpr int kShmdtSubcode = 2;
constexpr int kShmgetSubcode = 3;
constexpr int kSemctlSubcode = 0;
constexpr int kSemgetSubcode = 1;
constexpr int kSemopSubcode = 2;
constexpr int kSockAcceptSubcode = 1;
constexpr int kSockBindSubcode = 2;
constexpr int kSockConnectSubcode = 3;
constexpr int kSockGetPeernameSubcode = 4;
constexpr int kSockGetsocknameSubcode = 5;
constexpr int kSockGetsockoptSubcode = 6;
constexpr int kSockListenSubcode = 7;
constexpr int kSockRecvSubcode = 8;
constexpr int kSockRecvfromSubcode = 9;
constexpr int kSockSendSubcode = 10;
constexpr int kSockSendtoSubcode = 11;
constexpr int kSockSetsockoptSubcode = 12;
constexpr int kSockShutdownSubcode = 13;
constexpr int kSockSocketSubcode = 14;
constexpr unsigned long kSiocSocksys = 0x801c6956;
constexpr unsigned long kSiocAtmark = 0x40047307;

constexpr unsigned int make_ldt_selector(unsigned int index) {
	return (index << 3) | 0x7;
}

unsigned int allocate_tls_selector() {
	static unsigned int next_selector_index = kFirstTlsSelectorIndex;
	auto index = __atomic_fetch_add(&next_selector_index, 1U, __ATOMIC_RELAXED);
	if(index >= kMaxLdtIndex)
		return 0;
	return make_ldt_selector(index);
}

int set_socket_flags(int fd, int flags) {
	if(flags & ~(SOCK_CLOEXEC | SOCK_NONBLOCK))
		return EINVAL;

	if(flags & SOCK_CLOEXEC) {
		if(int e = syscall_error(raw_syscall(SYS_fcntl, fd, F_SETFD, FD_CLOEXEC)); e)
			return e;
	}

	if(flags & SOCK_NONBLOCK) {
		auto current = raw_syscall(SYS_fcntl, fd, F_GETFL, 0);
		if(int e = syscall_error(current); e)
			return e;
		if(int e = syscall_error(raw_syscall(SYS_fcntl, fd, F_SETFL, current | O_NONBLOCK)); e)
			return e;
	}

	return 0;
}

int open_socket_bootstrap() {
	constexpr const char *paths[] = {
		"/dev/socksys",
		"/dev/sock"
	};

	int last_error = ENOENT;
	for(const char *path : paths) {
		auto ret = raw_syscall(SYS_open, path, O_RDWR, 0);
		if(!syscall_error(ret))
			return ret;
		last_error = syscall_error(ret);
		if(last_error != ENOENT && last_error != ENXIO)
			return -last_error;
	}

	return -last_error;
}

int socksys_ioctl(int fd, socksysreq *req, long *ret_value) {
	auto ret = raw_syscall(SYS_ioctl, fd, kSiocSocksys, req);
	if(int e = syscall_error(ret); e)
		return e;
	if(ret_value)
		*ret_value = ret;
	return 0;
}

} // namespace

#ifndef MLIBC_BUILDING_RTLD
extern "C" long __do_syscall_ret(unsigned long ret) {
	if(ret > -4096UL) {
		errno = -ret;
		return -1;
	}
	return ret;
}
#endif

template<typename... Args>
static long raw_syscall(long number, Args... args);

long raw_syscall(long number) {
	return __do_syscall0(number);
}

template<typename Arg0>
long raw_syscall(long number, Arg0 arg0) {
	return __do_syscall1(number, (long)arg0);
}

template<typename Arg0, typename Arg1>
long raw_syscall(long number, Arg0 arg0, Arg1 arg1) {
	return __do_syscall2(number, (long)arg0, (long)arg1);
}

template<typename Arg0, typename Arg1, typename Arg2>
long raw_syscall(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2) {
	return __do_syscall3(number, (long)arg0, (long)arg1, (long)arg2);
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
long raw_syscall(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3) {
	return __do_syscall4(number, (long)arg0, (long)arg1, (long)arg2, (long)arg3);
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
long raw_syscall(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
	return __do_syscall5(number, (long)arg0, (long)arg1, (long)arg2, (long)arg3, (long)arg4);
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
long raw_syscall(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) {
	return __do_syscall6(number, (long)arg0, (long)arg1, (long)arg2, (long)arg3, (long)arg4, (long)arg5);
}

static int syscall_error(long ret) {
	if(ret < 0 && ret >= -4095)
		return -ret;
	return 0;
}

#define SVR4_STUB() do { \
	mlibc::infoLogger() << "mlibc: " << __func__ \
		<< " is only scaffolded for sysdeps/svr4" << frg::endlog; \
	return ENOSYS; \
} while(0)

namespace mlibc {

void Sysdeps<LibcLog>::operator()(const char *message) {
	size_t length = strlen(message);
	raw_syscall(SYS_write, 2, message, length);
	char lf = '\n';
	raw_syscall(SYS_write, 2, &lf, 1);
}

void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("mlibc panic on SVR4 scaffold");
	__builtin_trap();
}

int Sysdeps<Read>::operator()(int fd, void *buffer, size_t count, ssize_t *bytes_read) {
	auto ret = raw_syscall(SYS_read, fd, buffer, count);
	if(int e = syscall_error(ret); e)
		return e;
	*bytes_read = ret;
	return 0;
}

int Sysdeps<Write>::operator()(int fd, const void *buffer, size_t count, ssize_t *bytes_written) {
	auto ret = raw_syscall(SYS_write, fd, buffer, count);
	if(int e = syscall_error(ret); e)
		return e;
	*bytes_written = ret;
	return 0;
}

int Sysdeps<Open>::operator()(const char *path, int flags, mode_t mode, int *fd) {
	int open_flags = flags & ~O_CLOEXEC;
	auto ret = raw_syscall(SYS_open, path, open_flags, mode);
	if(int e = syscall_error(ret); e)
		return e;
	if(flags & O_CLOEXEC) {
		if(int e = syscall_error(raw_syscall(SYS_fcntl, ret, F_SETFD, FD_CLOEXEC)); e) {
			raw_syscall(SYS_close, ret);
			return e;
		}
	}
	*fd = ret;
	return 0;
}

int Sysdeps<Close>::operator()(int fd) {
	if(int e = syscall_error(raw_syscall(SYS_close, fd)); e)
		return e;
	return 0;
}

int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) {
	if(flags & ~O_CLOEXEC)
		return EINVAL;

	if(fd == newfd) {
		if(int e = syscall_error(raw_syscall(SYS_fcntl, fd, F_GETFD, 0)); e)
			return e;
		return 0;
	}

	int close_error = syscall_error(raw_syscall(SYS_close, newfd));
	if(close_error && close_error != EBADF)
		return close_error;

	auto ret = raw_syscall(SYS_fcntl, fd, F_DUPFD, newfd);
	if(int e = syscall_error(ret); e)
		return e;

	if(flags & O_CLOEXEC) {
		if(int e = syscall_error(raw_syscall(SYS_fcntl, ret, F_SETFD, FD_CLOEXEC)); e) {
			raw_syscall(SYS_close, ret);
			return e;
		}
	}

	return 0;
}

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
	auto ret = raw_syscall(SYS_lseek, fd, offset, whence);
	if(int e = syscall_error(ret); e)
		return e;
	*new_offset = ret;
	return 0;
}

int Sysdeps<AnonAllocate>::operator()(size_t size, void **pointer) {
	return sysdep<VmMap>(nullptr, size, PROT_READ | PROT_WRITE,
			MAP_PRIVATE | MAP_ANONYMOUS, -1, 0, pointer);
}

int Sysdeps<AnonFree>::operator()(void *pointer, size_t size) {
	return sysdep<VmUnmap>(pointer, size);
}

int Sysdeps<VmMap>::operator()(void *hint, size_t size, int prot, int flags,
		int fd, off_t offset, void **window) {
	int temporary_fd = -1;
	int map_fd = fd;

	if((flags & MAP_ANONYMOUS) && fd == -1) {
		auto open_ret = raw_syscall(SYS_open, "/dev/zero", O_RDWR, 0);
		if(int e = syscall_error(open_ret); e)
			return e;
		temporary_fd = open_ret;
		map_fd = temporary_fd;
		flags &= ~MAP_ANONYMOUS;
	}

	auto ret = raw_syscall(SYS_mmap, hint, size, prot, flags, map_fd, offset);
	if(temporary_fd != -1)
		raw_syscall(SYS_close, temporary_fd);
	if(int e = syscall_error(ret); e)
		return e;
	*window = reinterpret_cast<void *>(ret);
	return 0;
}

int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
	if(int e = syscall_error(raw_syscall(SYS_munmap, pointer, size)); e)
		return e;
	return 0;
}

int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot) {
	if(int e = syscall_error(raw_syscall(SYS_mprotect, pointer, size, prot)); e)
		return e;
	return 0;
}

[[noreturn]]
void Sysdeps<Exit>::operator()(int status) {
	raw_syscall(SYS_exit, status);
	__builtin_trap();
}

int Sysdeps<TcbSet>::operator()(void *pointer) {
	auto tcb = reinterpret_cast<Tcb *>(pointer);
	if(!tcb)
		return EINVAL;

	if(!tcb->tlsSelector) {
		tcb->tlsSelector = allocate_tls_selector();
		if(!tcb->tlsSelector)
			return ENOMEM;
	}

	ssd request = {
		.sel = tcb->tlsSelector,
		.bo = reinterpret_cast<uintptr_t>(pointer),
		.ls = 0xFFFFF,
		.acc1 = kUserDataAcc1,
		.acc2 = kDataAcc2,
	};

	auto ret = raw_syscall(SYS_sys3b, kSi86Dscr, &request, 0, 0);
	if(int e = syscall_error(ret); e)
		return e;

	asm volatile ("movw %w0, %%gs" : : "q"(tcb->tlsSelector) : "memory");
	return 0;
}

pid_t Sysdeps<FutexTid>::operator()() {
	return static_cast<pid_t>(raw_syscall(SYS_getpid));
}

int Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void *arg, int *result) {
	auto ret = raw_syscall(SYS_ioctl, fd, request, arg);
	if(int e = syscall_error(ret); e)
		return e;
	if(result)
		*result = ret;
	return 0;
}

int Sysdeps<Isatty>::operator()(int fd) {
	unsigned short winsize_hack[4];
	auto ret = raw_syscall(SYS_ioctl, fd, TIOCGWINSZ, &winsize_hack);
	if(int e = syscall_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<FutexWake>::operator()(int *, bool) {
	SVR4_STUB();
}

int Sysdeps<FutexWait>::operator()(int *, int, const struct timespec *) {
	SVR4_STUB();
}

int Sysdeps<ClockGet>::operator()(int, time_t *, long *) {
	SVR4_STUB();
}

int Sysdeps<Stat>::operator()(fsfd_target target, int dirfd, const char *path, int flags,
		struct stat *out) {
	if(!out)
		return EINVAL;
	if(flags & ~AT_SYMLINK_NOFOLLOW)
		return EINVAL;

	switch(target) {
	case fsfd_target::path: {
		if(!path)
			return EINVAL;
		auto call = (flags & AT_SYMLINK_NOFOLLOW) ? SYS_lxstat : SYS_xstat;
		auto ret = raw_syscall(call, _STAT_VER, path, out);
		if(int e = syscall_error(ret); e)
			return e;
		return 0;
	}
	case fsfd_target::fd: {
		auto ret = raw_syscall(SYS_fxstat, _STAT_VER, dirfd, out);
		if(int e = syscall_error(ret); e)
			return e;
		return 0;
	}
	case fsfd_target::fd_path: {
		if(dirfd != AT_FDCWD)
			return ENOSYS;
		if(!path)
			return EINVAL;
		auto call = (flags & AT_SYMLINK_NOFOLLOW) ? SYS_lxstat : SYS_xstat;
		auto ret = raw_syscall(call, _STAT_VER, path, out);
		if(int e = syscall_error(ret); e)
			return e;
		return 0;
	}
	default:
		return EINVAL;
	}
}

int Sysdeps<Statvfs>::operator()(const char *path, struct statvfs *out) {
	if(!path || !out)
		return EINVAL;
	auto ret = raw_syscall(SYS_statvfs, path, out);
	if(int e = syscall_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Fstatvfs>::operator()(int fd, struct statvfs *out) {
	if(!out)
		return EINVAL;
	auto ret = raw_syscall(SYS_fstatvfs, fd, out);
	if(int e = syscall_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Access>::operator()(const char *path, int mode) {
	if(int e = syscall_error(raw_syscall(SYS_access, path, mode)); e)
		return e;
	return 0;
}

int Sysdeps<Socket>::operator()(int family, int type, int protocol, int *fd) {
	int flags = type & (SOCK_CLOEXEC | SOCK_NONBLOCK);
	int kernel_type = type & ~(SOCK_CLOEXEC | SOCK_NONBLOCK);
	int bootstrap_fd = open_socket_bootstrap();
	if(bootstrap_fd < 0)
		return -bootstrap_fd;

	socksysreq req = {{kSockSocketSubcode, family, kernel_type, protocol, 0, 0, 0}};
	long new_socket_fd;
	if(int e = socksys_ioctl(bootstrap_fd, &req, &new_socket_fd); e) {
		raw_syscall(SYS_close, bootstrap_fd);
		return e;
	}

	int result_fd = bootstrap_fd;
	if(new_socket_fd != bootstrap_fd) {
		auto dup_ret = raw_syscall(SYS_fcntl, new_socket_fd, F_DUPFD, bootstrap_fd);
		if(int e = syscall_error(dup_ret); e) {
			raw_syscall(SYS_close, new_socket_fd);
			raw_syscall(SYS_close, bootstrap_fd);
			return e;
		}
		raw_syscall(SYS_close, new_socket_fd);
		result_fd = dup_ret;
	}

	if(int e = set_socket_flags(result_fd, flags); e) {
		raw_syscall(SYS_close, result_fd);
		return e;
	}

	*fd = result_fd;
	return 0;
}

int Sysdeps<Accept>::operator()(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
	socksysreq req = {{kSockAcceptSubcode, fd, (int)(long)addr_ptr, (int)(long)addr_length, 0, 0, 0}};
	long accepted_fd;
	if(int e = socksys_ioctl(fd, &req, &accepted_fd); e)
		return e;
	if(int e = set_socket_flags(accepted_fd, flags); e) {
		raw_syscall(SYS_close, accepted_fd);
		return e;
	}
	*newfd = accepted_fd;
	return 0;
}

int Sysdeps<Bind>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	socksysreq req = {{kSockBindSubcode, fd, (int)(long)addr_ptr, (int)addr_length, 0, 0, 0}};
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<Connect>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	socksysreq req = {{kSockConnectSubcode, fd, (int)(long)addr_ptr, (int)addr_length, 0, 0, 0}};
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<Sockname>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t, socklen_t *actual_length) {
	socksysreq req = {{kSockGetsocknameSubcode, fd, (int)(long)addr_ptr, (int)(long)actual_length, 0, 0, 0}};
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<Peername>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t, socklen_t *actual_length) {
	socksysreq req = {{kSockGetPeernameSubcode, fd, (int)(long)addr_ptr, (int)(long)actual_length, 0, 0, 0}};
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<GetSockopt>::operator()(int fd, int layer, int number, void *buffer, socklen_t *size) {
	socksysreq req = {{kSockGetsockoptSubcode, fd, layer, number, (int)(long)buffer, (int)(long)size, 0}};
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<SetSockopt>::operator()(int fd, int layer, int number, const void *buffer, socklen_t size) {
	socksysreq req = {{kSockSetsockoptSubcode, fd, layer, number, (int)(long)buffer, (int)size, 0}};
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<Listen>::operator()(int fd, int backlog) {
	socksysreq req = {{kSockListenSubcode, fd, backlog, 0, 0, 0, 0}};
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<Recvfrom>::operator()(int fd, void *buffer, size_t size, int flags, struct sockaddr *sock_addr, socklen_t *addr_length, ssize_t *length) {
	socksysreq req = {{kSockRecvfromSubcode, fd, (int)(long)buffer, (int)size, flags, (int)(long)sock_addr, (int)(long)addr_length}};
	long ret;
	if(int e = socksys_ioctl(fd, &req, &ret); e)
		return e;
	*length = ret;
	return 0;
}

int Sysdeps<Sendto>::operator()(int fd, const void *buffer, size_t size, int flags, const struct sockaddr *sock_addr, socklen_t addr_length, ssize_t *length) {
	int subcode = sock_addr ? kSockSendtoSubcode : kSockSendSubcode;
	socksysreq req = {{subcode, fd, (int)(long)buffer, (int)size, flags, (int)(long)sock_addr, (int)addr_length}};
	long ret;
	if(int e = socksys_ioctl(fd, &req, &ret); e)
		return e;
	*length = ret;
	return 0;
}

int Sysdeps<Shutdown>::operator()(int sockfd, int how) {
	socksysreq req = {{kSockShutdownSubcode, sockfd, how, 0, 0, 0, 0}};
	return socksys_ioctl(sockfd, &req, nullptr);
}

int Sysdeps<Sockatmark>::operator()(int sockfd, int *out) {
	auto ret = raw_syscall(SYS_ioctl, sockfd, kSiocAtmark, out);
	if(int e = syscall_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Semget>::operator()(key_t key, int n, int fl, int *id) {
	auto ret = raw_syscall(SYS_semsys, kSemgetSubcode, key, n, fl);
	if(int e = syscall_error(ret); e)
		return e;
	*id = ret;
	return 0;
}

int Sysdeps<Semctl>::operator()(int semid, int semnum, int cmd, void *semun, int *ret_value) {
	auto ret = raw_syscall(SYS_semsys, kSemctlSubcode, semid, semnum, cmd, semun);
	if(int e = syscall_error(ret); e)
		return e;
	*ret_value = ret;
	return 0;
}

int Sysdeps<Semop>::operator()(int semid, struct sembuf *sops, size_t nsops) {
	auto ret = raw_syscall(SYS_semsys, kSemopSubcode, semid, sops, nsops);
	if(int e = syscall_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Shmat>::operator()(void **seg_start, int shmid, const void *shmaddr, int shmflg) {
	auto ret = raw_syscall(SYS_shmsys, kShmatSubcode, shmid, shmaddr, shmflg);
	if(int e = syscall_error(ret); e)
		return e;
	*seg_start = reinterpret_cast<void *>(ret);
	return 0;
}

int Sysdeps<Shmctl>::operator()(int *idx, int shmid, int cmd, struct shmid_ds *buf) {
	auto ret = raw_syscall(SYS_shmsys, kShmctlSubcode, shmid, cmd, buf);
	if(int e = syscall_error(ret); e)
		return e;
	*idx = ret;
	return 0;
}

int Sysdeps<Shmdt>::operator()(const void *shmaddr) {
	auto ret = raw_syscall(SYS_shmsys, kShmdtSubcode, shmaddr);
	if(int e = syscall_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Shmget>::operator()(int *shm_id, key_t key, size_t size, int shmflg) {
	auto ret = raw_syscall(SYS_shmsys, kShmgetSubcode, key, size, shmflg);
	if(int e = syscall_error(ret); e)
		return e;
	*shm_id = ret;
	return 0;
}

int Sysdeps<Msgctl>::operator()(int q, int cmd, struct msqid_ds *buf) {
	auto ret = raw_syscall(SYS_msgsys, kMsgctlSubcode, q, cmd, buf);
	if(int e = syscall_error(ret); e)
		return e;
	return 0;
}

int Sysdeps<Msgget>::operator()(key_t k, int flag, int *out) {
	auto ret = raw_syscall(SYS_msgsys, kMsggetSubcode, k, flag);
	if(int e = syscall_error(ret); e)
		return e;
	*out = ret;
	return 0;
}

int Sysdeps<Msgrcv>::operator()(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg, ssize_t *out) {
	auto ret = raw_syscall(SYS_msgsys, kMsgrcvSubcode, msqid, msgp, msgsz, msgtyp, msgflg);
	if(int e = syscall_error(ret); e)
		return e;
	*out = ret;
	return 0;
}

int Sysdeps<Msgsnd>::operator()(int msqid, const void *msgp, size_t msgsz, int msgflg) {
	auto ret = raw_syscall(SYS_msgsys, kMsgsndSubcode, msqid, msgp, msgsz, msgflg);
	if(int e = syscall_error(ret); e)
		return e;
	return 0;
}

} // namespace mlibc