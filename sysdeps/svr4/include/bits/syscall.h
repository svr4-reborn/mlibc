#ifndef _MLIBC_SYSCALL_H
#define _MLIBC_SYSCALL_H

#include <errno.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef long __sc_word_t;

#ifdef __MLIBC_BUILDING_MLIBC

#define __MLIBC_SYSCALL_HIDDEN __attribute__((__visibility__("hidden")))

/* These functions are implemented in arch-syscall.cpp. */
uint64_t __do_syscall0(long) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall0_dual(long, __sc_word_t *) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall1(long, __sc_word_t) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall1_dual(long, __sc_word_t, __sc_word_t *) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall2(long, __sc_word_t, __sc_word_t) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall2_dual(long, __sc_word_t, __sc_word_t, __sc_word_t *) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall3(long, __sc_word_t, __sc_word_t, __sc_word_t) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall3_dual(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t *) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall4(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall4_dual(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t *) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall5(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t,
		__sc_word_t) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall5_dual(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t,
		__sc_word_t, __sc_word_t *) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall6(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t,
		__sc_word_t, __sc_word_t) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall6_dual(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t,
		__sc_word_t, __sc_word_t, __sc_word_t *) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall7(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t,
		__sc_word_t, __sc_word_t, __sc_word_t) __MLIBC_SYSCALL_HIDDEN;
uint64_t __do_syscall7_dual(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t,
		__sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t *) __MLIBC_SYSCALL_HIDDEN;

static inline long __mlibc_syscall_ret(uint64_t state) {
	long value = (long)(int32_t)state;
	if(state >> 32) {
		errno = value;
		return -1;
	}
	return value;
}

#ifdef __cplusplus
extern "C++" {

/* Defining a syscall as a macro is more problematic in C++, since there's a high chance of
 * a name collision e.g foo.syscall() or foo::syscall.
 */
inline long syscall(long n) {
	return __mlibc_syscall_ret(__do_syscall0(n));
}
template<typename Arg0>
long syscall(long n, Arg0 a0) {
	return __mlibc_syscall_ret(__do_syscall1(n, (long)a0));
}
template<typename Arg0, typename Arg1>
long syscall(long n, Arg0 a0, Arg1 a1) {
	return __mlibc_syscall_ret(__do_syscall2(n, (long)a0, (long)a1));
}
template<typename Arg0, typename Arg1, typename Arg2>
long syscall(long n, Arg0 a0, Arg1 a1, Arg2 a2) {
	return __mlibc_syscall_ret(__do_syscall3(n, (long)a0, (long)a1, (long)a2));
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
long syscall(long n, Arg0 a0, Arg1 a1, Arg2 a2, Arg3 a3) {
	return __mlibc_syscall_ret(__do_syscall4(n, (long)a0, (long)a1, (long)a2, (long)a3));
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
long syscall(long n, Arg0 a0, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4) {
	return __mlibc_syscall_ret(__do_syscall5(n, (long)a0, (long)a1, (long)a2, (long)a3, (long)a4));
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
long syscall(long n, Arg0 a0, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5) {
	return __mlibc_syscall_ret(__do_syscall6(n, (long)a0, (long)a1, (long)a2, (long)a3, (long)a4, (long)a5));
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
long syscall(long n, Arg0 a0, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6) {
	return __mlibc_syscall_ret(__do_syscall7(n, (long)a0, (long)a1, (long)a2, (long)a3, (long)a4, (long)a5, (long)a6));
}

} /* extern C++ */
#else

/*
 * Variadic macros are not supported in C89.
 * glibc implements syscall() as a variadic function, which we've ruled out.
 * musl uses them without checking the C standard in use. So suppressing
 * the check here seems reasonable.
 */
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvariadic-macros"

/* These syscall macros were copied from musl. */
#define __scc(x) ((__sc_word_t)(x))
#define __syscall0(n) __do_syscall0(n)
#define __syscall1(n,a) __do_syscall1(n,__scc(a))
#define __syscall2(n,a,b) __do_syscall2(n,__scc(a),__scc(b))
#define __syscall3(n,a,b,c) __do_syscall3(n,__scc(a),__scc(b),__scc(c))
#define __syscall4(n,a,b,c,d) __do_syscall4(n,__scc(a),__scc(b),__scc(c),__scc(d))
#define __syscall5(n,a,b,c,d,e) __do_syscall5(n,__scc(a),__scc(b),__scc(c),__scc(d),__scc(e))
#define __syscall6(n,a,b,c,d,e,f) __do_syscall6(n,__scc(a),__scc(b),__scc(c),__scc(d),__scc(e),__scc(f))
#define __syscall7(n,a,b,c,d,e,f,g) __do_syscall7(n,__scc(a),__scc(b),__scc(c),__scc(d),__scc(e),__scc(f),__scc(g))
#define __SYSCALL_NARGS_X(a,b,c,d,e,f,g,h,n,...) n
#define __SYSCALL_NARGS(...) __SYSCALL_NARGS_X(__VA_ARGS__,7,6,5,4,3,2,1,0,)
#define __SYSCALL_CONCAT_X(a,b) a##b
#define __SYSCALL_CONCAT(a,b) __SYSCALL_CONCAT_X(a,b)
#define __SYSCALL_DISP(b,...) __SYSCALL_CONCAT(b,__SYSCALL_NARGS(__VA_ARGS__))(__VA_ARGS__)
#define __syscall(...) __SYSCALL_DISP(__syscall,__VA_ARGS__)
#define syscall(...) __mlibc_syscall_ret(__syscall(__VA_ARGS__))

#pragma GCC diagnostic pop

#endif

#undef __MLIBC_SYSCALL_HIDDEN

#else

long __mlibc_syscall0(long);
long __mlibc_syscall1(long, __sc_word_t);
long __mlibc_syscall2(long, __sc_word_t, __sc_word_t);
long __mlibc_syscall3(long, __sc_word_t, __sc_word_t, __sc_word_t);
long __mlibc_syscall4(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t);
long __mlibc_syscall5(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t,
		__sc_word_t);
long __mlibc_syscall6(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t,
		__sc_word_t, __sc_word_t);
long __mlibc_syscall7(long, __sc_word_t, __sc_word_t, __sc_word_t, __sc_word_t,
		__sc_word_t, __sc_word_t, __sc_word_t);

#ifdef __cplusplus
extern "C++" {

inline long syscall(long n) {
	return __mlibc_syscall0(n);
}
template<typename Arg0>
long syscall(long n, Arg0 a0) {
	return __mlibc_syscall1(n, (long)a0);
}
template<typename Arg0, typename Arg1>
long syscall(long n, Arg0 a0, Arg1 a1) {
	return __mlibc_syscall2(n, (long)a0, (long)a1);
}
template<typename Arg0, typename Arg1, typename Arg2>
long syscall(long n, Arg0 a0, Arg1 a1, Arg2 a2) {
	return __mlibc_syscall3(n, (long)a0, (long)a1, (long)a2);
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
long syscall(long n, Arg0 a0, Arg1 a1, Arg2 a2, Arg3 a3) {
	return __mlibc_syscall4(n, (long)a0, (long)a1, (long)a2, (long)a3);
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4>
long syscall(long n, Arg0 a0, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4) {
	return __mlibc_syscall5(n, (long)a0, (long)a1, (long)a2, (long)a3, (long)a4);
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5>
long syscall(long n, Arg0 a0, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5) {
	return __mlibc_syscall6(n, (long)a0, (long)a1, (long)a2, (long)a3, (long)a4, (long)a5);
}
template<typename Arg0, typename Arg1, typename Arg2, typename Arg3, typename Arg4, typename Arg5, typename Arg6>
long syscall(long n, Arg0 a0, Arg1 a1, Arg2 a2, Arg3 a3, Arg4 a4, Arg5 a5, Arg6 a6) {
	return __mlibc_syscall7(n, (long)a0, (long)a1, (long)a2, (long)a3, (long)a4, (long)a5, (long)a6);
}

} /* extern C++ */
#else

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wvariadic-macros"

#define __scc(x) ((__sc_word_t)(x))
#define __syscall0(n) __mlibc_syscall0(n)
#define __syscall1(n,a) __mlibc_syscall1(n,__scc(a))
#define __syscall2(n,a,b) __mlibc_syscall2(n,__scc(a),__scc(b))
#define __syscall3(n,a,b,c) __mlibc_syscall3(n,__scc(a),__scc(b),__scc(c))
#define __syscall4(n,a,b,c,d) __mlibc_syscall4(n,__scc(a),__scc(b),__scc(c),__scc(d))
#define __syscall5(n,a,b,c,d,e) __mlibc_syscall5(n,__scc(a),__scc(b),__scc(c),__scc(d),__scc(e))
#define __syscall6(n,a,b,c,d,e,f) __mlibc_syscall6(n,__scc(a),__scc(b),__scc(c),__scc(d),__scc(e),__scc(f))
#define __syscall7(n,a,b,c,d,e,f,g) __mlibc_syscall7(n,__scc(a),__scc(b),__scc(c),__scc(d),__scc(e),__scc(f),__scc(g))
#define __SYSCALL_NARGS_X(a,b,c,d,e,f,g,h,n,...) n
#define __SYSCALL_NARGS(...) __SYSCALL_NARGS_X(__VA_ARGS__,7,6,5,4,3,2,1,0,)
#define __SYSCALL_CONCAT_X(a,b) a##b
#define __SYSCALL_CONCAT(a,b) __SYSCALL_CONCAT_X(a,b)
#define __SYSCALL_DISP(b,...) __SYSCALL_CONCAT(b,__SYSCALL_NARGS(__VA_ARGS__))(__VA_ARGS__)
#define __syscall(...) __SYSCALL_DISP(__syscall,__VA_ARGS__)
#define syscall(...) __syscall(__VA_ARGS__)

#pragma GCC diagnostic pop

#endif

#endif /* __MLIBC_BUILDING_MLIBC */

#ifdef __cplusplus
}
#endif

#endif /* _MLIBC_SYSCALL_H */
