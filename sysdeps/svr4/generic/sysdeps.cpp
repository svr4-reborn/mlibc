#include <errno.h>
#include <dirent.h>
#include <limits.h>
#include <poll.h>
#include <stdint.h>
#include <string.h>
#include <sys/select.h>

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

extern "C" [[gnu::visibility("hidden")]] void __mlibc_signal_restore(void);
extern "C" [[gnu::visibility("hidden")]] void __mlibc_signal_restore_rt(void);

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

constexpr bool syscall_should_restart(long number) {
	switch(number) {
		case SYS_read:
		case SYS_write:
		case SYS_readv:
		case SYS_writev:
		case SYS_fcntl:
		case SYS_ioctl:
		case SYS_poll:
		case SYS_wait:
		case SYS_waitsys:
		case SYS_getmsg:
		case SYS_putmsg:
		case SYS_getpmsg:
		case SYS_putpmsg:
			return true;
		default:
			return false;
	}
}

/// Run a syscall.
/// Returns a syscall_ret struct containing the return value and whether the syscall failed.
template<typename... Args>
syscall_ret syscall_call(long number, Args... args) {
	while(true) {
		auto state = syscall_state(number, args...);
		auto ret = syscall_ret{syscall_state_value(state), syscall_state_carry(state)};
		if(ret.failed && ret.value == ERESTART && syscall_should_restart(number))
			continue;
		return ret;
	}
}

template<typename... Args>
syscall_ret2 syscall_call_dual(long number, Args... args) {
	while(true) {
		__sc_word_t value2;
		auto state = syscall_state_dual(&value2, number, args...);
		auto ret = syscall_ret2{syscall_state_value(state), static_cast<long>(value2), syscall_state_carry(state)};
		if(ret.failed && ret.value == ERESTART && syscall_should_restart(number))
			continue;
		return ret;
	}
}

namespace {

struct svr4_dirent_wire {
	uint32_t d_ino;
	int32_t d_off;
	uint16_t d_reclen;
	char d_name[1];
};

struct translated_dirent_span {
	size_t raw_offset;
	size_t public_reclen;
};

constexpr size_t kSvr4DirentHeaderSize = offsetof(svr4_dirent_wire, d_name);
constexpr size_t kPublicDirentHeaderSize = offsetof(struct dirent, d_name);
constexpr size_t kMinSvr4DirentReclen = kSvr4DirentHeaderSize + 1;

static_assert(kSvr4DirentHeaderSize == 10);

struct ssd {
	unsigned int sel;
	unsigned int bo;
	unsigned int ls;
	unsigned int acc1;
	unsigned int acc2;
};
	struct svr4_interval {
		unsigned long word1;
		unsigned long word2;
		int clock;
	};
	
	struct svr4_hrtime {
		unsigned long secs;
		long rem;
		unsigned long res;
	};

constexpr int kSi86Dscr = 75;
constexpr unsigned int kUserDataAcc1 = 0xF2;
constexpr unsigned int kDataAcc2 = 0xC;
constexpr unsigned int kFirstTlsSelectorIndex = 7;
constexpr unsigned int kMaxLdtIndex = 8192;
constexpr int kHrtsysCntlOpcode = 0;
constexpr int kHrtGetResCommand = 0;
constexpr int kHrtTofdCommand = 1;
constexpr int kHrtStartItCommand = 2;
constexpr int kHrtGetItCommand = 3;
constexpr int kClockStd = 0x0001;
constexpr unsigned long kNanosecondsPerSecond = 1000000000UL;
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
constexpr int kClocalDebugconWriteSubcode = 1;
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
constexpr long kClockTicksPerSecond = 100;

void timeval_from_clock_ticks(clock_t ticks, timeval *tv) {
	tv->tv_sec = ticks / kClockTicksPerSecond;
	tv->tv_usec = (ticks % kClockTicksPerSecond) * 1000000 / kClockTicksPerSecond;
}

int translate_hrtime_result(const svr4_hrtime &time, time_t *secs, long *nanos) {
	if(time.res != kNanosecondsPerSecond)
		return EIO;
	if(time.rem < 0 || static_cast<unsigned long>(time.rem) >= kNanosecondsPerSecond)
		return EIO;
	*secs = static_cast<time_t>(time.secs);
	*nanos = time.rem;
	return 0;
}

int fetch_realtime(time_t *secs, long *nanos) {
	svr4_hrtime time{0, 0, kNanosecondsPerSecond};
	if(int e = syscall_call(SYS_hrtsys, kHrtsysCntlOpcode, kHrtTofdCommand,
			kClockStd, static_cast<svr4_interval *>(nullptr), &time).error(); e)
		return e;
	return translate_hrtime_result(time, secs, nanos);
}

int fetch_monotonic(time_t *secs, long *nanos) {
	static svr4_interval start_interval;
	static int init_state;

	auto state = __atomic_load_n(&init_state, __ATOMIC_ACQUIRE);
	while(state != 2) {
		if(state == 0) {
			int expected = 0;
			if(__atomic_compare_exchange_n(&init_state, &expected, 1, false,
					__ATOMIC_ACQ_REL, __ATOMIC_ACQUIRE)) {
				if(int e = syscall_call(SYS_hrtsys, kHrtsysCntlOpcode,
						kHrtStartItCommand, kClockStd, &start_interval,
						static_cast<svr4_hrtime *>(nullptr)).error(); e) {
					__atomic_store_n(&init_state, 0, __ATOMIC_RELEASE);
					return e;
				}
				__atomic_store_n(&init_state, 2, __ATOMIC_RELEASE);
				break;
			}
		} else {
			__asm__ __volatile__("pause");
		}
		state = __atomic_load_n(&init_state, __ATOMIC_ACQUIRE);
	}

	svr4_hrtime time{0, 0, kNanosecondsPerSecond};
	if(int e = syscall_call(SYS_hrtsys, kHrtsysCntlOpcode, kHrtGetItCommand,
			kClockStd, &start_interval, &time).error(); e)
		return e;
	return translate_hrtime_result(time, secs, nanos);
}

void fill_wait_rusage(const siginfo_t &info, struct rusage *ru) {
	memset(ru, 0, sizeof(struct rusage));
	timeval_from_clock_ticks(info.si_utime, &ru->ru_utime);
	timeval_from_clock_ticks(info.si_stime, &ru->ru_stime);
}


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

size_t bounded_cstring_length(const char *string, size_t limit) {
	for(size_t i = 0; i < limit; ++i) {
		if(!string[i])
			return i;
	}
	return limit;
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

size_t getdents_raw_budget(size_t max_size) {
	constexpr size_t per_record_growth = kPublicDirentHeaderSize - kSvr4DirentHeaderSize;
	constexpr size_t max_kernel_getdents_count = INT_MAX;
	constexpr size_t total_record_growth = kMinSvr4DirentReclen + per_record_growth;
	if(!per_record_growth)
		return max_size < max_kernel_getdents_count ? max_size : max_kernel_getdents_count;

	auto quotient = max_size / total_record_growth;
	auto remainder = max_size % total_record_growth;
	auto budget = quotient * kMinSvr4DirentReclen;
	budget += (remainder * kMinSvr4DirentReclen) / total_record_growth;
	if(budget > max_kernel_getdents_count)
		budget = max_kernel_getdents_count;
	if(budget < kMinSvr4DirentReclen)
		budget = kMinSvr4DirentReclen;
	return budget;
}

int translate_svr4_dirents(const void *raw_buffer, size_t raw_bytes, void *buffer, size_t max_size,
		size_t *translated_bytes) {
	if(!raw_bytes) {
		*translated_bytes = 0;
		return 0;
	}

	auto raw_bytes_view = reinterpret_cast<const char *>(raw_buffer);
	auto bytes = reinterpret_cast<char *>(buffer);
	auto max_entries = raw_bytes / kMinSvr4DirentReclen + 1;
	auto spans = reinterpret_cast<translated_dirent_span *>(__builtin_alloca(max_entries * sizeof(translated_dirent_span)));

	size_t raw_offset = 0;
	size_t total_public_bytes = 0;
	size_t count = 0;

	while(raw_offset < raw_bytes) {
		if(raw_bytes - raw_offset < kSvr4DirentHeaderSize)
			return EIO;

		auto raw = reinterpret_cast<const svr4_dirent_wire *>(raw_bytes_view + raw_offset);
		if(raw->d_reclen < kMinSvr4DirentReclen || raw_offset + raw->d_reclen > raw_bytes)
			return EIO;

		auto raw_name_size = static_cast<size_t>(raw->d_reclen) - kSvr4DirentHeaderSize;
		auto name_length = bounded_cstring_length(raw->d_name, raw_name_size);
		if(name_length == raw_name_size)
			return EIO;

		auto public_reclen = kPublicDirentHeaderSize + name_length + 1;
		if(total_public_bytes > max_size - public_reclen)
			return EIO;

		spans[count++] = {raw_offset, public_reclen};
		total_public_bytes += public_reclen;
		raw_offset += raw->d_reclen;
	}

	size_t public_offset = total_public_bytes;
	while(count--) {
		auto raw = reinterpret_cast<const svr4_dirent_wire *>(raw_bytes_view + spans[count].raw_offset);
		auto raw_name_size = static_cast<size_t>(raw->d_reclen) - kSvr4DirentHeaderSize;
		auto name_length = bounded_cstring_length(raw->d_name, raw_name_size);

		public_offset -= spans[count].public_reclen;
		auto translated = reinterpret_cast<struct dirent *>(bytes + public_offset);
		translated->d_ino = raw->d_ino;
		translated->d_off = raw->d_off;
		translated->d_reclen = spans[count].public_reclen;
		translated->d_type = DT_UNKNOWN;
		memcpy(translated->d_name, raw->d_name, name_length + 1);
	}

	*translated_bytes = total_public_bytes;
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

int translate_waitsys_idtype(idtype_t idtype, int &kernel_idtype) {
	// SVR4 waitsys() uses the kernel's procset.h numbering rather than
	// mlibc's public POSIX-facing idtype_t enum values.
	switch(idtype) {
		case P_ALL:
			kernel_idtype = 7;
			return 0;
		case P_PID:
			kernel_idtype = 0;
			return 0;
		case P_PGID:
			kernel_idtype = 2;
			return 0;
		case P_PIDFD:
			return EINVAL;
		default:
			return EINVAL;
	}
}

int require_at_path(int dirfd, const char *path) {
	if(dirfd != AT_FDCWD)
		return ENOSYS;
	if(!path)
		return EINVAL;
	return 0;
}

bool local_fd_isset(int fd, const fd_set *set) {
	return set->fds_bits[fd / 8] & (1 << (fd % 8));
}

void local_fd_set(int fd, fd_set *set) {
	set->fds_bits[fd / 8] |= 1 << (fd % 8);
}

void local_fd_zero(fd_set *set) {
	memset(set->fds_bits, 0, sizeof(set->fds_bits));
}

} // namespace

#define SVR4_STUB() do { \
	mlibc::infoLogger() << "mlibc: " << __PRETTY_FUNCTION__ \
		<< " is stubbed for sysdeps/svr4" << frg::endlog; \
	return ENOSYS; \
} while(0)

namespace mlibc {

void Sysdeps<LibcLog>::operator()(const char *message) {
	size_t length = strlen(message);
	(void)syscall_call(SYS_clocal, kClocalDebugconWriteSubcode, message, length, 0, 0);
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
	if(max_size < kPublicDirentHeaderSize + 1)
		return EINVAL;

	auto raw_budget = getdents_raw_budget(max_size);
	auto raw_buffer = reinterpret_cast<char *>(__builtin_alloca(raw_budget));
	long result;
	if(int e = syscall_call(SYS_getdents, handle, raw_buffer, raw_budget).store(&result); e)
		return e;
	return translate_svr4_dirents(raw_buffer, static_cast<size_t>(result), buffer, max_size, bytes_read);
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
			MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0, pointer);
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
	if(ret_error) {
		mlibc::infoLogger() << "mlibc: mmap syscall failed in VmMap with error " << ret_error << frg::endlog;
		return ret_error;
	}
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
	struct sigaction translated_action;
	auto translated_ptr = act;
	if(act) {
		translated_action = *act;
		if(translated_action.sa_handler != SIG_DFL && translated_action.sa_handler != SIG_IGN) {
			auto restorer = (translated_action.sa_flags & SA_SIGINFO)
					? &__mlibc_signal_restore_rt
					: &__mlibc_signal_restore;
			translated_action.sa_resv[0] = static_cast<int>(reinterpret_cast<intptr_t>(restorer));
		}
		translated_ptr = &translated_action;
	}
	return syscall_call(SYS_sigaction, sn, translated_ptr, old).error();
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
	int kernel_idtype;
	if(int e = translate_waitsys_idtype(idtype, kernel_idtype); e)
		return e;
	return syscall_call(SYS_waitsys, kernel_idtype, id, info, options).error();
}

int Sysdeps<Waitpid>::operator()(pid_t pid, int *status, int flags, struct rusage *ru, pid_t *ret_pid) {
	int kernel_idtype;
	id_t id;

	if(pid > 0) {
		kernel_idtype = 0;
		id = pid;
	} else if(pid == -1) {
		kernel_idtype = 7;
		id = 0;
	} else if(pid == 0) {
		kernel_idtype = 2;
		if(int e = syscall_call(SYS_pgrpsys, kPgrpGetPgidSubcode, 0).store(&id); e)
			return e;
	} else {
		kernel_idtype = 2;
		id = -pid;
	}

	while(true) {
		siginfo_t info{};
		auto options = waitid_options_from_waitpid_flags(flags);
		if(int e = syscall_call(SYS_waitsys, kernel_idtype, id, &info, options).error(); e) {
			return e;
		}

		if(!info.si_pid) {
			if(ret_pid)
				*ret_pid = 0;
			if(ru)
				memset(ru, 0, sizeof(struct rusage));
			return 0;
		}

		if(ret_pid)
			*ret_pid = info.si_pid;
		if(status)
			*status = wait_status_from_siginfo(info);
		if(ru)
			fill_wait_rusage(info, ru);
		return 0;
	}
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

int Sysdeps<Ttyname>::operator()(int fd, char *buf, size_t size) {
	if(!buf)
		return EINVAL;
	if(!size)
		return ERANGE;

	if(int e = sysdep<Isatty>(fd); e)
		return e;

	struct stat target_stat;
	if(int e = sysdep<Stat>(fsfd_target::fd, fd, nullptr, 0, &target_stat); e)
		return e;

	int dir_handle;
	if(int e = sysdep<OpenDir>("/dev", &dir_handle); e)
		return e;

	char entry_buffer[2048];
	char fallback_path[PATH_MAX + 1];
	bool have_fallback = false;

	while(true) {
		size_t bytes_read;
		if(int e = sysdep<ReadEntries>(dir_handle, entry_buffer, sizeof(entry_buffer), &bytes_read); e) {
			(void)sysdep<Close>(dir_handle);
			return e;
		}

		if(!bytes_read)
			break;

		for(size_t offset = 0; offset < bytes_read; ) {
			auto ent = reinterpret_cast<struct dirent *>(entry_buffer + offset);
			if(!ent->d_reclen || offset + ent->d_reclen > bytes_read) {
				(void)sysdep<Close>(dir_handle);
				return EIO;
			}

			offset += ent->d_reclen;

			if(!__builtin_strcmp(ent->d_name, ".") || !__builtin_strcmp(ent->d_name, ".."))
				continue;

			size_t name_length = strlen(ent->d_name);
			size_t path_length = 5 + name_length;
			if(path_length + 1 > sizeof(fallback_path))
				continue;

			char candidate_path[PATH_MAX + 1];
			memcpy(candidate_path, "/dev/", 5);
			memcpy(candidate_path + 5, ent->d_name, name_length + 1);

			struct stat candidate_stat;
			if(int e = sysdep<Stat>(fsfd_target::path, -1, candidate_path, AT_SYMLINK_NOFOLLOW, &candidate_stat); e)
				continue;

			if(!S_ISCHR(candidate_stat.st_mode))
				continue;

			if(candidate_stat.st_dev == target_stat.st_dev && candidate_stat.st_ino == target_stat.st_ino) {
				(void)sysdep<Close>(dir_handle);
				if(path_length + 1 > size)
					return ERANGE;
				memcpy(buf, candidate_path, path_length + 1);
				return 0;
			}

			if(!have_fallback && candidate_stat.st_rdev == target_stat.st_rdev) {
				memcpy(fallback_path, candidate_path, path_length + 1);
				have_fallback = true;
			}
		}
	}

	(void)sysdep<Close>(dir_handle);

	if(!have_fallback)
		return ENODEV;

	size_t fallback_length = strlen(fallback_path);
	if(fallback_length + 1 > size)
		return ERANGE;
	memcpy(buf, fallback_path, fallback_length + 1);
	return 0;
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

int Sysdeps<Pselect>::operator()(int num_fds, fd_set *read_set, fd_set *write_set,
		fd_set *except_set, const struct timespec *timeout, const sigset_t *sigmask, int *num_events) {
	if(num_fds < 0 || !num_events)
		return EINVAL;
	if(num_fds > FD_SETSIZE)
		return EINVAL;

	int timeout_ms = -1;
	if(timeout) {
		if(timeout->tv_sec < 0 || timeout->tv_nsec < 0 || timeout->tv_nsec >= 1000000000L)
			return EINVAL;

		long long timeout_ll = static_cast<long long>(timeout->tv_sec) * 1000;
		timeout_ll += (timeout->tv_nsec + 999999) / 1000000;
		if(timeout_ll > INT_MAX)
			timeout_ms = INT_MAX;
		else
			timeout_ms = static_cast<int>(timeout_ll);
	}

	int active_fds = 0;
	for(int fd = 0; fd < num_fds; ++fd) {
		if((read_set && local_fd_isset(fd, read_set))
				|| (write_set && local_fd_isset(fd, write_set))
				|| (except_set && local_fd_isset(fd, except_set)))
			++active_fds;
	}

	pollfd *poll_fds = nullptr;
	if(active_fds)
		poll_fds = reinterpret_cast<pollfd *>(__builtin_alloca(active_fds * sizeof(pollfd)));

	int current = 0;
	for(int fd = 0; fd < num_fds; ++fd) {
		short events = 0;
		if(read_set && local_fd_isset(fd, read_set))
			events |= POLLIN;
		if(write_set && local_fd_isset(fd, write_set))
			events |= POLLOUT;
		if(except_set && local_fd_isset(fd, except_set))
			events |= POLLPRI;
		if(!events)
			continue;

		poll_fds[current].fd = fd;
		poll_fds[current].events = events;
		poll_fds[current].revents = 0;
		++current;
	}

	if(read_set)
		local_fd_zero(read_set);
	if(write_set)
		local_fd_zero(write_set);
	if(except_set)
		local_fd_zero(except_set);

	int mask_error = 0;
	sigset_t old_mask;
	if(sigmask)
		mask_error = sysdep<Sigprocmask>(SIG_SETMASK, sigmask, &old_mask);
	if(mask_error)
		return mask_error;

	int poll_events = 0;
	int poll_error = sysdep<Poll>(poll_fds, active_fds, timeout_ms, &poll_events);

	int restore_error = 0;
	if(sigmask)
		restore_error = sysdep<Sigprocmask>(SIG_SETMASK, &old_mask, nullptr);

	if(poll_error)
		return poll_error;
	if(restore_error)
		return restore_error;

	for(int index = 0; index < active_fds; ++index) {
		int fd = poll_fds[index].fd;
		short revents = poll_fds[index].revents;

		if(read_set && (revents & (POLLIN | POLLHUP | POLLERR)))
			local_fd_set(fd, read_set);
		if(write_set && (revents & (POLLOUT | POLLERR)))
			local_fd_set(fd, write_set);
		if(except_set && (revents & POLLPRI))
			local_fd_set(fd, except_set);
	}

	*num_events = poll_events;
	return 0;
}

int Sysdeps<Pause>::operator()() {
	auto ret = syscall_call(SYS_pause);
	if(int e = ret.error(); e)
		return e;
	__ensure(!"pause() unexpectedly returned successfully");
	__builtin_unreachable();
}

int Sysdeps<FutexWake>::operator()(int *, bool) {
	//SVR4_STUB();
	return 0;
}

int Sysdeps<FutexWait>::operator()(int *, int, const struct timespec *) {
	//SVR4_STUB();
	return 0;
}

int Sysdeps<ClockGet>::operator()(int clock, time_t* secs, long* nanos) {
	if(!secs || !nanos)
		return EINVAL;

	switch(clock) {
		case CLOCK_REALTIME:
			return fetch_realtime(secs, nanos);
		case CLOCK_MONOTONIC:
			return fetch_monotonic(secs, nanos);
		default:
			return EINVAL;
	}
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