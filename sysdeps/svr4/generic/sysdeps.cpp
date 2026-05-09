#include <errno.h>
#include <poll.h>
#include <stdint.h>
#include <string.h>

#include <type_traits>

#include <abi-bits/errno.h>
#include <abi-bits/fcntl.h>
#include <abi-bits/ioctls.h>
#include <abi-bits/termios.h>
#include <abi-bits/vm-flags.h>
#include <bits/syscall.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/wait.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/tcb.hpp>

/// SVR4 syscalls are a bit weird, compared to more modern kernels.
/// Not only are they done via far calls and call gates, they return whether
/// the syscall succeded or errored in the carry flag.
/// This has the (theoretical) side effect that, as far as I am aware, the
/// kernel could return any length of errno, including negative values, that
/// would need to be handled.

/// The kernel itself does limit errno to 8 bits at the moment, but to preseve
/// the quite interesting ability to error *and* return a longer value,
/// I implemented the syscall function in a way that allows the kernel to
/// return a full 32 bit value and an error at the same time, by using the
/// carry flag to indicate whether the syscall failed, and returning the value
/// and errno in a struct.

/// Convert a type into a raw syscall argument while preserving pointer-ness.
template<typename T>
constexpr long syscall_arg(T value) {
	if constexpr(std::is_pointer_v<T>)
		return reinterpret_cast<long>(value);
	else
		return static_cast<long>(value);
}

uint64_t syscall_state(long number) {
	return __do_syscall0(number);
}

template<typename Arg0>
uint64_t syscall_state(long number, Arg0 arg0) {
	return __do_syscall1(number, syscall_arg(arg0));
}

template<typename Arg0, typename Arg1>
uint64_t syscall_state(long number, Arg0 arg0, Arg1 arg1) {
	return __do_syscall2(number, syscall_arg(arg0), syscall_arg(arg1));
}

template<typename Arg0, typename Arg1, typename Arg2>
uint64_t syscall_state(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2) {
	return __do_syscall3(number, syscall_arg(arg0), syscall_arg(arg1), syscall_arg(arg2));
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
uint64_t syscall_state(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3) {
	return __do_syscall4(number, syscall_arg(arg0), syscall_arg(arg1), syscall_arg(arg2), syscall_arg(arg3));
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
uint64_t syscall_state(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
	return __do_syscall5(number, syscall_arg(arg0), syscall_arg(arg1), syscall_arg(arg2), syscall_arg(arg3), syscall_arg(arg4));
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
uint64_t syscall_state(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) {
	return __do_syscall6(number, syscall_arg(arg0), syscall_arg(arg1), syscall_arg(arg2), syscall_arg(arg3), syscall_arg(arg4), syscall_arg(arg5));
}

uint64_t syscall_state_dual(__sc_word_t *value2, long number) {
	return __do_syscall0_dual(number, value2);
}

template<typename Arg0>
uint64_t syscall_state_dual(__sc_word_t *value2, long number, Arg0 arg0) {
	return __do_syscall1_dual(number, syscall_arg(arg0), value2);
}

template<typename Arg0, typename Arg1>
uint64_t syscall_state_dual(__sc_word_t *value2, long number, Arg0 arg0, Arg1 arg1) {
	return __do_syscall2_dual(number, syscall_arg(arg0), syscall_arg(arg1), value2);
}

template<typename Arg0, typename Arg1, typename Arg2>
uint64_t syscall_state_dual(__sc_word_t *value2, long number, Arg0 arg0, Arg1 arg1, Arg2 arg2) {
	return __do_syscall3_dual(number, syscall_arg(arg0), syscall_arg(arg1), syscall_arg(arg2), value2);
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
uint64_t syscall_state_dual(__sc_word_t *value2, long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3) {
	return __do_syscall4_dual(number, syscall_arg(arg0), syscall_arg(arg1), syscall_arg(arg2), syscall_arg(arg3), value2);
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
uint64_t syscall_state_dual(__sc_word_t *value2, long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4) {
	return __do_syscall5_dual(number, syscall_arg(arg0), syscall_arg(arg1), syscall_arg(arg2), syscall_arg(arg3), syscall_arg(arg4), value2);
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
uint64_t syscall_state_dual(__sc_word_t *value2, long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5) {
	return __do_syscall6_dual(number, syscall_arg(arg0), syscall_arg(arg1), syscall_arg(arg2), syscall_arg(arg3), syscall_arg(arg4), syscall_arg(arg5), value2);
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
uint64_t syscall_state_dual(__sc_word_t *value2, long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3, Arg4 arg4, Arg5 arg5, Arg6 arg6) {
	return __do_syscall7_dual(number, syscall_arg(arg0), syscall_arg(arg1), syscall_arg(arg2), syscall_arg(arg3), syscall_arg(arg4), syscall_arg(arg5), syscall_arg(arg6), value2);
}

/// Extract the returned value from the system call.
/// In case the system call failed, this will be the errno.
constexpr long syscall_state_value(uint64_t state) {
	return static_cast<long>(static_cast<int32_t>(state));
}

/// Extract the value of the carry flag after the syscall.
/// If it is set, the syscall failed.
constexpr bool syscall_state_carry(uint64_t state) {
	return static_cast<bool>(state >> 32);
}

struct syscall_ret {
	long value;
	bool failed;

	int error() const {
		if(!failed)
			return 0;
		return static_cast<int>(value);
	}

	template<typename Result>
	int store(Result *result) const {
		if(int e = error(); e)
			return e;

		if constexpr(std::is_pointer_v<Result>)
			*result = reinterpret_cast<Result>(value);
		else
			*result = static_cast<Result>(value);

		return 0;
	}
};

struct syscall_ret2 {
	long value;
	long value2;
	bool failed;

	int error() const {
		if(!failed)
			return 0;
		return static_cast<int>(value);
	}
};

/// Run a syscall.
/// Returns a syscall_ret struct containing the return value and whether the syscall failed.
template<typename... Args>
syscall_ret syscall_call(long number, Args... args) {
	auto state = syscall_state(number, args...);
	return {syscall_state_value(state), syscall_state_carry(state)};
}

template<typename... Args>
syscall_ret2 syscall_call_dual(long number, Args... args) {
	__sc_word_t value2;
	auto state = syscall_state_dual(&value2, number, args...);
	return {syscall_state_value(state), static_cast<long>(value2), syscall_state_carry(state)};
}

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
constexpr int kPgrpGetSidSubcode = 2;
constexpr int kPgrpSetSidSubcode = 3;
constexpr int kPgrpGetPgidSubcode = 4;
constexpr int kPgrpSetPgidSubcode = 5;
constexpr int kSigpendingSubcode = 1;
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


// socksysreq stores its payload in int slots, matching the historical SVR4
// ioctl interface rather than the wider raw syscall calling convention.
template<typename T>
constexpr int socksys_arg(T value) {
	if constexpr(std::is_pointer_v<T>)
		return static_cast<int>(reinterpret_cast<intptr_t>(value));
	else
		return static_cast<int>(value);
}


// Keep the socket multiplexor request packing in one place so the socket
// sysdeps read as subcode-specific logic rather than field shuffling.
template<typename... Args>
socksysreq make_socksys_request(Args... args) {
	static_assert(sizeof...(Args) == 7);
	return socksysreq {{ socksys_arg(args)... }};
}

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
		if(int e = syscall_call(SYS_fcntl, fd, F_SETFD, FD_CLOEXEC).error(); e)
			return e;
	}

	if(flags & SOCK_NONBLOCK) {
		long current;
		if(int e = syscall_call(SYS_fcntl, fd, F_GETFL, 0).store(&current); e)
			return e;
		if(int e = syscall_call(SYS_fcntl, fd, F_SETFL, current | O_NONBLOCK).error(); e)
			return e;
	}

	return 0;
}

int open_socket_bootstrap() {
	constexpr const char *paths[] = {
		"/dev/socksys",
		"/dev/sock"
	};

	// Different SVR4-derived systems expose the STREAMS socket entry point under
	// different device nodes; try the common ones in order.
	int last_error = ENOENT;
	for(const char *path : paths) {
		auto opened = syscall_call(SYS_open, path, O_RDWR, 0);
		long fd;
		if(!opened.store(&fd))
			return fd;
		last_error = opened.error();
		if(last_error != ENOENT && last_error != ENXIO)
			return -last_error;
	}

	return -last_error;
}

int socksys_ioctl(int fd, socksysreq *req, long *ret_value) {
	auto ret = syscall_call(SYS_ioctl, fd, kSiocSocksys, req);
	if(int e = ret.error(); e)
		return e;
	if(ret_value)
		*ret_value = ret.value;
	return 0;
}

int wait_status_from_siginfo(const siginfo_t &info) {
	int status = info.si_status & 0377;

	switch(info.si_code) {
	case CLD_EXITED:
		return status << 8;
	case CLD_DUMPED:
		return status | WCOREFLG;
	case CLD_KILLED:
		return status;
	case CLD_TRAPPED:
	case CLD_STOPPED:
		return (status << 8) | WSTOPFLG;
	case CLD_CONTINUED:
		return WCONTFLG;
	default:
		return 0;
	}
}

int waitid_options_from_waitpid_flags(int flags) {
	int options = WEXITED | WTRAPPED;
	if(flags & WNOHANG)
		options |= WNOHANG;
	if(flags & WUNTRACED)
		options |= WSTOPPED;
	if(flags & WCONTINUED)
		options |= WCONTINUED;
	return options;
}

int require_at_path(int dirfd, const char *path) {
	if(dirfd != AT_FDCWD)
		return ENOSYS;
	if(!path)
		return EINVAL;
	return 0;
}

} // namespace

#define SVR4_STUB() do { \
	mlibc::infoLogger() << "mlibc: " << __func__ \
		<< " is stubbed for sysdeps/svr4" << frg::endlog; \
	return ENOSYS; \
} while(0)

namespace mlibc {

void Sysdeps<LibcLog>::operator()(const char *message) {
	size_t length = strlen(message);
	(void)syscall_call(SYS_write, 2, message, length);
	char lf = '\n';
	(void)syscall_call(SYS_write, 2, &lf, 1);
}

void Sysdeps<LibcPanic>::operator()() {
	sysdep<LibcLog>("mlibc panic on SVR4 scaffold");
	__builtin_trap();
}

pid_t Sysdeps<GetPid>::operator()() {
	return static_cast<pid_t>(syscall_call_dual(SYS_getpid).value);
}

uid_t Sysdeps<GetUid>::operator()() {
	return static_cast<uid_t>(syscall_call_dual(SYS_getuid).value);
}

uid_t Sysdeps<GetEuid>::operator()() {
	return static_cast<uid_t>(syscall_call_dual(SYS_getuid).value2);
}

gid_t Sysdeps<GetGid>::operator()() {
	return static_cast<gid_t>(syscall_call_dual(SYS_getgid).value);
}

gid_t Sysdeps<GetEgid>::operator()() {
	return static_cast<gid_t>(syscall_call_dual(SYS_getgid).value2);
}

pid_t Sysdeps<GetPpid>::operator()() {
	return static_cast<pid_t>(syscall_call_dual(SYS_getpid).value2);
}

int Sysdeps<Chdir>::operator()(const char *path) {
	return syscall_call(SYS_chdir, path).error();

}

int Sysdeps<Fchdir>::operator()(int fd) {
	return syscall_call(SYS_fchdir, fd).error();
}

int Sysdeps<Chroot>::operator()(const char *path) {
	return syscall_call(SYS_chroot, path).error();
}

int Sysdeps<Chmod>::operator()(const char *pathname, mode_t mode) {
	return syscall_call(SYS_chmod, pathname, mode).error();
}

int Sysdeps<Fchmod>::operator()(int fd, mode_t mode) {
	return syscall_call(SYS_fchmod, fd, mode).error();
}

int Sysdeps<Mkdir>::operator()(const char *path, mode_t mode) {
	return syscall_call(SYS_mkdir, path, mode).error();
}

int Sysdeps<Rmdir>::operator()(const char *path) {
	return syscall_call(SYS_rmdir, path).error();
}

int Sysdeps<Link>::operator()(const char *old_path, const char *new_path) {
	return syscall_call(SYS_link, old_path, new_path).error();
}

int Sysdeps<Symlink>::operator()(const char *target_path, const char *link_path) {
	return syscall_call(SYS_symlink, target_path, link_path).error();
}

int Sysdeps<Rename>::operator()(const char *path, const char *new_path) {
	return syscall_call(SYS_rename, path, new_path).error();
}

int Sysdeps<Read>::operator()(int fd, void *buffer, size_t count, ssize_t *bytes_read) {
	return syscall_call(SYS_read, fd, buffer, count).store(bytes_read);
}

int Sysdeps<Readv>::operator()(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_read) {
	return syscall_call(SYS_readv, fd, iovs, iovc).store(bytes_read);
}

int Sysdeps<Write>::operator()(int fd, const void *buffer, size_t count, ssize_t *bytes_written) {
	return syscall_call(SYS_write, fd, buffer, count).store(bytes_written);
}

int Sysdeps<Writev>::operator()(int fd, const struct iovec *iovs, int iovc, ssize_t *bytes_written) {
	return syscall_call(SYS_writev, fd, iovs, iovc).store(bytes_written);
}

int Sysdeps<Open>::operator()(const char *path, int flags, mode_t mode, int *fd) {
	int open_flags = flags & ~O_CLOEXEC;
	auto opened = syscall_call(SYS_open, path, open_flags, mode);
	long opened_fd;
	if(int e = opened.store(&opened_fd); e)
		return e;
	if(flags & O_CLOEXEC) {
		if(int e = syscall_call(SYS_fcntl, opened_fd, F_SETFD, FD_CLOEXEC).error(); e) {
			(void)syscall_call(SYS_close, opened_fd);
			return e;
		}
	}
	*fd = opened_fd;
	return 0;
}

int Sysdeps<OpenDir>::operator()(const char *path, int *handle) {
	int fd;
	if(int e = sysdep<Open>(path, O_RDONLY, 0, &fd); e)
		return e;

	struct stat st;
	if(int e = sysdep<Stat>(fsfd_target::fd, fd, nullptr, 0, &st); e) {
		(void)syscall_call(SYS_close, fd);
		return e;
	}

	if(!S_ISDIR(st.st_mode)) {
		(void)syscall_call(SYS_close, fd);
		return ENOTDIR;
	}

	*handle = fd;
	return 0;
}

int Sysdeps<ReadEntries>::operator()(int handle, void *buffer, size_t max_size, size_t *bytes_read) {
	long result;
	if(int e = syscall_call(SYS_getdents, handle, buffer, max_size).store(&result); e)
		return e;
	*bytes_read = static_cast<size_t>(result);
	return 0;
}

int Sysdeps<Close>::operator()(int fd) {
	return syscall_call(SYS_close, fd).error();
}

int Sysdeps<Dup>::operator()(int fd, int flags, int *newfd) {
	if(flags & ~O_CLOEXEC)
		return EINVAL;

	long duplicated_fd;
	if(int e = syscall_call(SYS_dup, fd).store(&duplicated_fd); e)
		return e;

	if(flags & O_CLOEXEC) {
		if(int e = syscall_call(SYS_fcntl, duplicated_fd, F_SETFD, FD_CLOEXEC).error(); e) {
			(void)syscall_call(SYS_close, duplicated_fd);
			return e;
		}
	}

	*newfd = static_cast<int>(duplicated_fd);
	return 0;
}

int Sysdeps<Dup2>::operator()(int fd, int flags, int newfd) {
	if(flags & ~O_CLOEXEC)
		return EINVAL;

	if(fd == newfd) {
		if(int e = syscall_call(SYS_fcntl, fd, F_GETFD, 0).error(); e)
			return e;
		return 0;
	}

	int close_error = syscall_call(SYS_close, newfd).error();
	if(close_error && close_error != EBADF)
		return close_error;

	long duplicated_fd;
	if(int e = syscall_call(SYS_fcntl, fd, F_DUPFD, newfd).store(&duplicated_fd); e)
		return e;

	if(flags & O_CLOEXEC) {
		if(int e = syscall_call(SYS_fcntl, duplicated_fd, F_SETFD, FD_CLOEXEC).error(); e) {
			(void)syscall_call(SYS_close, duplicated_fd);
			return e;
		}
	}

	return 0;
}

int Sysdeps<Seek>::operator()(int fd, off_t offset, int whence, off_t *new_offset) {
	return syscall_call(SYS_lseek, fd, offset, whence).store(new_offset);
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
		if(int e = syscall_call(SYS_open, "/dev/zero", O_RDWR, 0).store(&temporary_fd); e)
			return e;
		map_fd = temporary_fd;
		flags &= ~MAP_ANONYMOUS;
	}

	void *mapped_window;
	int ret_error = syscall_call(SYS_mmap, hint, size, prot, flags, map_fd, offset).store(&mapped_window);
	if(temporary_fd != -1)
		(void)syscall_call(SYS_close, temporary_fd);
	if(ret_error)
		return ret_error;
	*window = mapped_window;
	return 0;
}

int Sysdeps<VmUnmap>::operator()(void *pointer, size_t size) {
	return syscall_call(SYS_munmap, pointer, size).error();
}

int Sysdeps<VmProtect>::operator()(void *pointer, size_t size, int prot) {
	return syscall_call(SYS_mprotect, pointer, size, prot).error();
}

[[noreturn]]
void Sysdeps<Exit>::operator()(int status) {
	(void)syscall_call(SYS_exit, status);
	__builtin_trap();
}

int Sysdeps<Fork>::operator()(pid_t *child) {
	auto ret = syscall_call_dual(SYS_fork);
	if(int e = ret.error(); e)
		return e;
	*child = ret.value2 ? 0 : static_cast<pid_t>(ret.value);
	return 0;
}

int Sysdeps<Execve>::operator()(const char *path, char *const argv[], char *const envp[]) {
	auto ret = syscall_call(SYS_execve, path, argv, envp);
	if(int e = ret.error(); e)
		return e;
	__ensure(!"execve() unexpectedly returned successfully");
	__builtin_unreachable();
}

int Sysdeps<Kill>::operator()(pid_t pid, int sig) {
	return syscall_call(SYS_kill, pid, sig).error();
}

int Sysdeps<Sigprocmask>::operator()(int how, const sigset_t *__restrict set, sigset_t *__restrict retrieve) {
	return syscall_call(SYS_sigprocmask, how, set, retrieve).error();
}

int Sysdeps<Sigaction>::operator()(int sn, const struct sigaction *__restrict act,
		struct sigaction *__restrict old) {
	return syscall_call(SYS_sigaction, sn, act, old).error();
}

int Sysdeps<Sigsuspend>::operator()(const sigset_t *set) {
	return syscall_call(SYS_sigsuspend, set).error();
}

int Sysdeps<Sigaltstack>::operator()(const stack_t *ss, stack_t *oss) {
	return syscall_call(SYS_sigaltstack, ss, oss).error();
}

int Sysdeps<Sigpending>::operator()(sigset_t *set) {
	return syscall_call(SYS_sigpending, kSigpendingSubcode, set).error();
}

int Sysdeps<Waitid>::operator()(idtype_t idtype, id_t id, siginfo_t *info, int options) {
	return syscall_call(SYS_waitsys, idtype, id, info, options).error();
}

int Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	idtype_t idtype;
	id_t id;

	if(pid > 0) {
		idtype = P_PID;
		id = pid;
	} else if(pid == -1) {
		idtype = P_ALL;
		id = 0;
	} else {
		idtype = P_PGID;
		id = (pid == 0) ? 0 : -pid;
	}

	siginfo_t info{};
	if(int e = syscall_call(SYS_waitsys, idtype, id, &info,
			waitid_options_from_waitpid_flags(flags)).error(); e)
		return e;

	if(ret_pid)
		*ret_pid = info.si_pid;
	if(status)
		*status = wait_status_from_siginfo(info);
	if(ru)
		memset(ru, 0, sizeof(struct rusage));
	return 0;
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

	if(int e = syscall_call(SYS_sys3b, kSi86Dscr, &request, 0, 0).error(); e)
		return e;

	asm volatile ("movw %w0, %%gs" : : "q"(tcb->tlsSelector) : "memory");
	return 0;
}

pid_t Sysdeps<FutexTid>::operator()() {
	return static_cast<pid_t>(syscall_call_dual(SYS_getpid).value);
}

int Sysdeps<GetPgid>::operator()(pid_t pid, pid_t *pgid) {
	return syscall_call(SYS_pgrpsys, kPgrpGetPgidSubcode, pid).store(pgid);
}

int Sysdeps<GetSid>::operator()(pid_t pid, pid_t *sid) {
	return syscall_call(SYS_pgrpsys, kPgrpGetSidSubcode, pid).store(sid);
}

int Sysdeps<SetPgid>::operator()(pid_t pid, pid_t pgid) {
	return syscall_call(SYS_pgrpsys, kPgrpSetPgidSubcode, pid, pgid).error();
}

int Sysdeps<SetSid>::operator()(pid_t *sid) {
	return syscall_call(SYS_pgrpsys, kPgrpSetSidSubcode).store(sid);
}

int Sysdeps<SetUid>::operator()(uid_t uid) {
	return syscall_call(SYS_setuid, uid).error();
}

int Sysdeps<SetEuid>::operator()(uid_t euid) {
	return syscall_call(SYS_seteuid, euid).error();
}

int Sysdeps<SetGid>::operator()(gid_t gid) {
	return syscall_call(SYS_setgid, gid).error();
}

int Sysdeps<SetEgid>::operator()(gid_t egid) {
	return syscall_call(SYS_setegid, egid).error();
}

int Sysdeps<GetGroups>::operator()(size_t size, gid_t *list, int *ret) {
	return syscall_call(SYS_getgroups, size, list).store(ret);
}

int Sysdeps<SetGroups>::operator()(size_t size, const gid_t *list) {
	return syscall_call(SYS_setgroups, size, list).error();
}

int Sysdeps<GetRlimit>::operator()(int resource, struct rlimit *limit) {
	return syscall_call(SYS_getrlimit, resource, limit).error();
}

int Sysdeps<SetRlimit>::operator()(int resource, const struct rlimit *limit) {
	return syscall_call(SYS_setrlimit, resource, limit).error();
}

int Sysdeps<Nice>::operator()(int nice, int *new_nice) {
	return syscall_call(SYS_nice, nice).store(new_nice);
}

int Sysdeps<Readlink>::operator()(const char *path, void *buffer, size_t max_size, ssize_t *length) {
	return syscall_call(SYS_readlink, path, buffer, max_size).store(length);
}

int Sysdeps<Pipe>::operator()(int *fds, int flags) {
	if(flags & ~(O_CLOEXEC | O_NONBLOCK))
		return EINVAL;

	auto ret = syscall_call_dual(SYS_pipe);
	if(int e = ret.error(); e)
		return e;

	fds[0] = static_cast<int>(ret.value);
	fds[1] = static_cast<int>(ret.value2);

	if(flags & O_CLOEXEC) {
		if(int e = syscall_call(SYS_fcntl, fds[0], F_SETFD, FD_CLOEXEC).error(); e)
			return e;
		if(int e = syscall_call(SYS_fcntl, fds[1], F_SETFD, FD_CLOEXEC).error(); e)
			return e;
	}

	if(flags & O_NONBLOCK) {
		long first_flags;
		if(int e = syscall_call(SYS_fcntl, fds[0], F_GETFL, 0).store(&first_flags); e)
			return e;
		if(int e = syscall_call(SYS_fcntl, fds[0], F_SETFL, first_flags | O_NONBLOCK).error(); e)
			return e;

		long second_flags;
		if(int e = syscall_call(SYS_fcntl, fds[1], F_GETFL, 0).store(&second_flags); e)
			return e;
		if(int e = syscall_call(SYS_fcntl, fds[1], F_SETFL, second_flags | O_NONBLOCK).error(); e)
			return e;
	}

	return 0;
}

void Sysdeps<Sync>::operator()() {
	(void)syscall_call(SYS_sync);
}

int Sysdeps<Fsync>::operator()(int fd) {
	return syscall_call(SYS_fsync, fd).error();
}

int Sysdeps<Fdatasync>::operator()(int fd) {
	return syscall_call(SYS_fsync, fd).error();
}

int Sysdeps<Fcntl>::operator()(int fd, int request, va_list args, int *result) {
	long arg = 0;

	switch(request) {
	case F_GETFD:
	case F_GETFL:
	case F_GETOWN:
		break;
	case F_DUPFD:
	case F_SETFD:
	case F_SETFL:
	case F_SETOWN:
	case F_CHKFL:
	case F_CHSIZE:
	case F_RDCHK:
	case F_LK_UNLCK:
	case F_LK_LOCK:
	case F_LK_NBLCK:
	case F_LK_RLCK:
	case F_LK_NBRLCK:
		arg = va_arg(args, int);
		break;
	case F_GETLK:
	case F_SETLK:
	case F_SETLKW:
	case F_ALLOCSP:
	case F_FREESP:
	case F_RSETLK:
	case F_RGETLK:
	case F_RSETLKW:
		arg = syscall_arg(va_arg(args, void *));
		break;
	default:
		return EINVAL;
	}

	return syscall_call(SYS_fcntl, fd, request, arg).store(result);
}

int Sysdeps<Times>::operator()(struct tms *tms, clock_t *out) {
	return syscall_call(SYS_times, tms).store(out);
}

int Sysdeps<Uname>::operator()(struct utsname *buf) {
	return syscall_call(SYS_uname, buf).error();
}

int Sysdeps<Ioctl>::operator()(int fd, unsigned long request, void *arg, int *result) {
	if(result)
		return syscall_call(SYS_ioctl, fd, request, arg).store(result);
	return syscall_call(SYS_ioctl, fd, request, arg).error();
}

int Sysdeps<Isatty>::operator()(int fd) {
	struct termios termios_hack;
	return syscall_call(SYS_ioctl, fd, TCGETS, &termios_hack).error();
}

int Sysdeps<Tcgetattr>::operator()(int fd, struct termios *attr) {
	int result;
	return sysdep<Ioctl>(fd, TCGETS, attr, &result);
}

int Sysdeps<Tcsetattr>::operator()(int fd, int optional_action, const struct termios *attr) {
	int request;
	switch(optional_action) {
	case TCSANOW:
		request = TCSETS;
		break;
	case TCSADRAIN:
		request = TCSETSW;
		break;
	case TCSAFLUSH:
		request = TCSETSF;
		break;
	default:
		return EINVAL;
	}

	int result;
	return sysdep<Ioctl>(fd, request, const_cast<struct termios *>(attr), &result);
}

int Sysdeps<Tcsendbreak>::operator()(int fd, int) {
	int result;
	return sysdep<Ioctl>(fd, TCSBRK, reinterpret_cast<void *>(0), &result);
}

int Sysdeps<Tcflow>::operator()(int fd, int action) {
	int result;
	return sysdep<Ioctl>(fd, TCXONC, reinterpret_cast<void *>(static_cast<uintptr_t>(action)), &result);
}

int Sysdeps<Tcflush>::operator()(int fd, int queue) {
	int result;
	return sysdep<Ioctl>(fd, TCFLSH, reinterpret_cast<void *>(static_cast<uintptr_t>(queue)), &result);
}

int Sysdeps<Tcdrain>::operator()(int fd) {
	int result;
	return sysdep<Ioctl>(fd, TCSBRK, reinterpret_cast<void *>(1), &result);
}

int Sysdeps<Tcgetwinsize>::operator()(int fd, struct winsize *winsz) {
	int result;
	return sysdep<Ioctl>(fd, TIOCGWINSZ, winsz, &result);
}

int Sysdeps<Tcsetwinsize>::operator()(int fd, const struct winsize *winsz) {
	int result;
	return sysdep<Ioctl>(fd, TIOCSWINSZ, const_cast<struct winsize *>(winsz), &result);
}

int Sysdeps<Poll>::operator()(struct pollfd *fds, nfds_t count, int timeout, int *num_events) {
	return syscall_call(SYS_poll, fds, count, timeout).store(num_events);
}

int Sysdeps<Pause>::operator()() {
	auto ret = syscall_call(SYS_pause);
	if(int e = ret.error(); e)
		return e;
	__ensure(!"pause() unexpectedly returned successfully");
	__builtin_unreachable();
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
		return syscall_call(call, _STAT_VER, path, out).error();
	}
	case fsfd_target::fd: {
		return syscall_call(SYS_fxstat, _STAT_VER, dirfd, out).error();
	}
	case fsfd_target::fd_path: {
		if(dirfd != AT_FDCWD)
			return ENOSYS;
		if(!path)
			return EINVAL;
		auto call = (flags & AT_SYMLINK_NOFOLLOW) ? SYS_lxstat : SYS_xstat;
		return syscall_call(call, _STAT_VER, path, out).error();
	}
	default:
		return EINVAL;
	}
}

int Sysdeps<Statvfs>::operator()(const char *path, struct statvfs *out) {
	if(!path || !out)
		return EINVAL;
	return syscall_call(SYS_statvfs, path, out).error();
}

int Sysdeps<Fstatvfs>::operator()(int fd, struct statvfs *out) {
	if(!out)
		return EINVAL;
	return syscall_call(SYS_fstatvfs, fd, out).error();
}

int Sysdeps<Access>::operator()(const char *path, int mode) {
	return syscall_call(SYS_access, path, mode).error();
}

int Sysdeps<Faccessat>::operator()(int dirfd, const char *pathname, int mode, int flags) {
	if(flags & ~(AT_EACCESS | AT_SYMLINK_NOFOLLOW))
		return EINVAL;
	if(flags)
		return ENOSYS;
	if(int e = require_at_path(dirfd, pathname); e)
		return e;
	return sysdep<Access>(pathname, mode);
}

int Sysdeps<Openat>::operator()(int dirfd, const char *path, int flags, mode_t mode, int *fd) {
	if(int e = require_at_path(dirfd, path); e)
		return e;
	return sysdep<Open>(path, flags, mode, fd);
}

int Sysdeps<Readlinkat>::operator()(int dirfd, const char *path, void *buffer, size_t max_size, ssize_t *length) {
	if(int e = require_at_path(dirfd, path); e)
		return e;
	return sysdep<Readlink>(path, buffer, max_size, length);
}

int Sysdeps<Mkdirat>::operator()(int dirfd, const char *path, mode_t mode) {
	if(int e = require_at_path(dirfd, path); e)
		return e;
	return sysdep<Mkdir>(path, mode);
}

int Sysdeps<Linkat>::operator()(int olddirfd, const char *old_path, int newdirfd, const char *new_path, int flags) {
	if(flags & ~AT_SYMLINK_FOLLOW)
		return EINVAL;
	if(flags)
		return ENOSYS;
	if(int e = require_at_path(olddirfd, old_path); e)
		return e;
	if(int e = require_at_path(newdirfd, new_path); e)
		return e;
	return sysdep<Link>(old_path, new_path);
}

int Sysdeps<Symlinkat>::operator()(const char *target_path, int dirfd, const char *link_path) {
	if(!target_path)
		return EINVAL;
	if(int e = require_at_path(dirfd, link_path); e)
		return e;
	return sysdep<Symlink>(target_path, link_path);
}

int Sysdeps<Renameat>::operator()(int olddirfd, const char *old_path, int newdirfd, const char *new_path) {
	if(int e = require_at_path(olddirfd, old_path); e)
		return e;
	if(int e = require_at_path(newdirfd, new_path); e)
		return e;
	return sysdep<Rename>(old_path, new_path);
}

int Sysdeps<Fchmodat>::operator()(int dirfd, const char *pathname, mode_t mode, int flags) {
	if(flags & ~AT_SYMLINK_NOFOLLOW)
		return EINVAL;
	if(flags)
		return ENOSYS;
	if(int e = require_at_path(dirfd, pathname); e)
		return e;
	return sysdep<Chmod>(pathname, mode);
}

int Sysdeps<Unlinkat>::operator()(int dirfd, const char *path, int flags) {
	if(flags & ~AT_REMOVEDIR)
		return EINVAL;
	if(int e = require_at_path(dirfd, path); e)
		return e;
	if(flags & AT_REMOVEDIR)
		return sysdep<Rmdir>(path);
	return syscall_call(SYS_unlink, path).error();
}

int Sysdeps<Socket>::operator()(int family, int type, int protocol, int *fd) {
	int flags = type & (SOCK_CLOEXEC | SOCK_NONBLOCK);
	int kernel_type = type & ~(SOCK_CLOEXEC | SOCK_NONBLOCK);
	int bootstrap_fd = open_socket_bootstrap();
	if(bootstrap_fd < 0)
		return -bootstrap_fd;

	socksysreq req = make_socksys_request(kSockSocketSubcode, family, kernel_type, protocol, 0, 0, 0);
	long new_socket_fd;
	if(int e = socksys_ioctl(bootstrap_fd, &req, &new_socket_fd); e) {
		(void)syscall_call(SYS_close, bootstrap_fd);
		return e;
	}

	int result_fd = bootstrap_fd;
	if(new_socket_fd != bootstrap_fd) {
		long duplicated_fd;
		if(int e = syscall_call(SYS_fcntl, new_socket_fd, F_DUPFD, bootstrap_fd).store(&duplicated_fd); e) {
			(void)syscall_call(SYS_close, new_socket_fd);
			(void)syscall_call(SYS_close, bootstrap_fd);
			return e;
		}
		(void)syscall_call(SYS_close, new_socket_fd);
		result_fd = duplicated_fd;
	}

	if(int e = set_socket_flags(result_fd, flags); e) {
		(void)syscall_call(SYS_close, result_fd);
		return e;
	}

	*fd = result_fd;
	return 0;
}

int Sysdeps<Accept>::operator()(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
	socksysreq req = make_socksys_request(kSockAcceptSubcode, fd, addr_ptr, addr_length, 0, 0, 0);
	long accepted_fd;
	if(int e = socksys_ioctl(fd, &req, &accepted_fd); e)
		return e;
	if(int e = set_socket_flags(accepted_fd, flags); e) {
		(void)syscall_call(SYS_close, accepted_fd);
		return e;
	}
	*newfd = accepted_fd;
	return 0;
}

int Sysdeps<Bind>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	socksysreq req = make_socksys_request(kSockBindSubcode, fd, addr_ptr, addr_length, 0, 0, 0);
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<Connect>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	socksysreq req = make_socksys_request(kSockConnectSubcode, fd, addr_ptr, addr_length, 0, 0, 0);
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<Sockname>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t, socklen_t *actual_length) {
	socksysreq req = make_socksys_request(kSockGetsocknameSubcode, fd, addr_ptr, actual_length, 0, 0, 0);
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<Peername>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t, socklen_t *actual_length) {
	socksysreq req = make_socksys_request(kSockGetPeernameSubcode, fd, addr_ptr, actual_length, 0, 0, 0);
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<GetSockopt>::operator()(int fd, int layer, int number, void *buffer, socklen_t *size) {
	socksysreq req = make_socksys_request(kSockGetsockoptSubcode, fd, layer, number, buffer, size, 0);
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<SetSockopt>::operator()(int fd, int layer, int number, const void *buffer, socklen_t size) {
	socksysreq req = make_socksys_request(kSockSetsockoptSubcode, fd, layer, number, buffer, size, 0);
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<Listen>::operator()(int fd, int backlog) {
	socksysreq req = make_socksys_request(kSockListenSubcode, fd, backlog, 0, 0, 0, 0);
	return socksys_ioctl(fd, &req, nullptr);
}

int Sysdeps<Recvfrom>::operator()(int fd, void *buffer, size_t size, int flags, struct sockaddr *sock_addr, socklen_t *addr_length, ssize_t *length) {
	socksysreq req = make_socksys_request(kSockRecvfromSubcode, fd, buffer, size, flags, sock_addr, addr_length);
	long ret;
	if(int e = socksys_ioctl(fd, &req, &ret); e)
		return e;
	*length = ret;
	return 0;
}

int Sysdeps<Sendto>::operator()(int fd, const void *buffer, size_t size, int flags, const struct sockaddr *sock_addr, socklen_t addr_length, ssize_t *length) {
	int subcode = sock_addr ? kSockSendtoSubcode : kSockSendSubcode;
	socksysreq req = make_socksys_request(subcode, fd, buffer, size, flags, sock_addr, addr_length);
	long ret;
	if(int e = socksys_ioctl(fd, &req, &ret); e)
		return e;
	*length = ret;
	return 0;
}

int Sysdeps<Shutdown>::operator()(int sockfd, int how) {
	socksysreq req = make_socksys_request(kSockShutdownSubcode, sockfd, how, 0, 0, 0, 0);
	return socksys_ioctl(sockfd, &req, nullptr);
}

int Sysdeps<Sockatmark>::operator()(int sockfd, int *out) {
	return syscall_call(SYS_ioctl, sockfd, kSiocAtmark, out).error();
}

int Sysdeps<Semget>::operator()(key_t key, int n, int fl, int *id) {
	return syscall_call(SYS_semsys, kSemgetSubcode, key, n, fl).store(id);
}

int Sysdeps<Semctl>::operator()(int semid, int semnum, int cmd, void *semun, int *ret_value) {
	return syscall_call(SYS_semsys, kSemctlSubcode, semid, semnum, cmd, semun).store(ret_value);
}

int Sysdeps<Semop>::operator()(int semid, struct sembuf *sops, size_t nsops) {
	return syscall_call(SYS_semsys, kSemopSubcode, semid, sops, nsops).error();
}

int Sysdeps<Shmat>::operator()(void **seg_start, int shmid, const void *shmaddr, int shmflg) {
	return syscall_call(SYS_shmsys, kShmatSubcode, shmid, shmaddr, shmflg).store(seg_start);
}

int Sysdeps<Shmctl>::operator()(int *idx, int shmid, int cmd, struct shmid_ds *buf) {
	return syscall_call(SYS_shmsys, kShmctlSubcode, shmid, cmd, buf).store(idx);
}

int Sysdeps<Shmdt>::operator()(const void *shmaddr) {
	return syscall_call(SYS_shmsys, kShmdtSubcode, shmaddr).error();
}

int Sysdeps<Shmget>::operator()(int *shm_id, key_t key, size_t size, int shmflg) {
	return syscall_call(SYS_shmsys, kShmgetSubcode, key, size, shmflg).store(shm_id);
}

int Sysdeps<Msgctl>::operator()(int q, int cmd, struct msqid_ds *buf) {
	return syscall_call(SYS_msgsys, kMsgctlSubcode, q, cmd, buf).error();
}

int Sysdeps<Msgget>::operator()(key_t k, int flag, int *out) {
	return syscall_call(SYS_msgsys, kMsggetSubcode, k, flag).store(out);
}

int Sysdeps<Msgrcv>::operator()(int msqid, void *msgp, size_t msgsz, long msgtyp, int msgflg, ssize_t *out) {
	return syscall_call(SYS_msgsys, kMsgrcvSubcode, msqid, msgp, msgsz, msgtyp, msgflg).store(out);
}

int Sysdeps<Msgsnd>::operator()(int msqid, const void *msgp, size_t msgsz, int msgflg) {
	return syscall_call(SYS_msgsys, kMsgsndSubcode, msqid, msgp, msgsz, msgflg).error();
}

} // namespace mlibc