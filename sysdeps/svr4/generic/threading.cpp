#include <errno.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/syscall.h>
#include <time.h>
#include <type_traits>

#include <bits/syscall.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>
#include <mlibc/tcb.hpp>
#include <mlibc/thread.hpp>

namespace {

constexpr size_t kDefaultStackSize = 0x200000;
constexpr size_t kPageSize = 4096;

template<typename T>
constexpr long syscall_arg(T value) {
	if constexpr(std::is_pointer_v<T>)
		return reinterpret_cast<long>(value);
	else
		return static_cast<long>(value);
}

constexpr long syscall_state_value(uint64_t state) {
	return static_cast<long>(static_cast<int32_t>(state));
}

constexpr bool syscall_state_carry(uint64_t state) {
	return static_cast<bool>(state >> 32);
}

struct syscall_ret {
	long value;
	bool failed;

	int error() const {
		return failed ? static_cast<int>(value) : 0;
	}

	template<typename Result>
	int store(Result *result) const {
		if(int e = error(); e)
			return e;
		*result = static_cast<Result>(value);
		return 0;
	}
};

syscall_ret thread_syscall(long number) {
	auto state = __do_syscall0(number);
	return {syscall_state_value(state), syscall_state_carry(state)};
}

template<typename Arg0, typename Arg1>
syscall_ret thread_syscall(long number, Arg0 arg0, Arg1 arg1) {
	auto state = __do_syscall2(number, syscall_arg(arg0), syscall_arg(arg1));
	return {syscall_state_value(state), syscall_state_carry(state)};
}

uintptr_t align_down(uintptr_t value, uintptr_t alignment) {
	return value & ~(alignment - 1);
}

} // namespace

extern "C" [[noreturn]] void __mlibc_enter_thread(void *entry, void *user_arg, void *tcb_pointer) {
	auto tcb = reinterpret_cast<Tcb *>(tcb_pointer);
	if(int e = mlibc::sysdep<TcbSet>(tcb))
		mlibc::panicLogger() << "mlibc: failed to set SVR4 thread TCB: " << e << frg::endlog;

	while(!__atomic_load_n(&tcb->tid, __ATOMIC_ACQUIRE))
		mlibc::sysdep<FutexWait>(&tcb->tid, 0, nullptr);

	__atomic_fetch_or(&tcb->cancelBits, tcbCancelEnableBit, __ATOMIC_RELAXED);
	tcb->invokeThreadFunc(entry, user_arg);
	mlibc::thread_exit(tcb->returnValue);
}

namespace mlibc {

int Sysdeps<PrepareStack>::operator()(void **stack, void *entry, void *user_arg,
		void *tcb, size_t *stack_size, size_t *guard_size, void **stack_base) {
	if(!*stack_size)
		*stack_size = kDefaultStackSize;

	uintptr_t map;
	if(*stack) {
		map = reinterpret_cast<uintptr_t>(*stack);
		*guard_size = 0;
	} else {
		if(!*guard_size)
			*guard_size = kPageSize;

		void *window;
		if(int e = sysdep<VmMap>(nullptr, *stack_size + *guard_size, PROT_NONE,
				MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0, &window))
			return e;
		map = reinterpret_cast<uintptr_t>(window);

		if(int e = sysdep<VmProtect>(reinterpret_cast<void *>(map + *guard_size),
				*stack_size, PROT_READ | PROT_WRITE))
			return e;
	}

	*stack_base = reinterpret_cast<void *>(map);
	auto sp = reinterpret_cast<uintptr_t *>(align_down(map + *guard_size + *stack_size, 16));
	*--sp = reinterpret_cast<uintptr_t>(tcb);
	*--sp = reinterpret_cast<uintptr_t>(user_arg);
	*--sp = reinterpret_cast<uintptr_t>(entry);
	*--sp = 0;
	*stack = reinterpret_cast<void *>(sp);
	return 0;
}

int Sysdeps<Clone>::operator()(void *, pid_t *pid_out, void *stack) {
	pid_t tid;
	if(int e = thread_syscall(SYS_thread_create, __mlibc_enter_thread, stack).store(&tid))
		return e;
	*pid_out = tid;
	return 0;
}

[[noreturn]]
void Sysdeps<ThreadExit>::operator()() {
	(void)thread_syscall(SYS_thread_exit);
	__builtin_trap();
}

} // namespace mlibc