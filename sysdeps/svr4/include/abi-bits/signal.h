#ifndef _ABIBITS_SIGNAL_H
#define _ABIBITS_SIGNAL_H

#include <mlibc-config.h>

#include <abi-bits/pid_t.h>
#include <abi-bits/sigevent.h>
#include <abi-bits/sigset_t.h>
#include <abi-bits/uid_t.h>
#include <bits/ansi/clock_t.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*__sighandler)(int);

struct siginfo;

#define SIG_DFL ((__sighandler)(void *)(0))
#define SIG_ERR ((__sighandler)(void *)(-1))
#define SIG_IGN ((__sighandler)(void *)(1))
#define SIG_HOLD ((__sighandler)(void *)(2))

#define SIGHUP 1
#define SIGINT 2
#define SIGQUIT 3
#define SIGILL 4
#define SIGTRAP 5
#define SIGIOT 6
#define SIGABRT 6
#define SIGEMT 7
#define SIGFPE 8
#define SIGKILL 9
#define SIGBUS 10
#define SIGSEGV 11
#define SIGSYS 12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM 15
#define SIGUSR1 16
#define SIGUSR2 17
#define SIGCLD 18
#define SIGCHLD 18
#define SIGPWR 19
#define SIGWINCH 20
#define SIGURG 21
#define SIGPOLL 22
#define SIGIO 22
#define SIGSTOP 23
#define SIGTSTP 24
#define SIGCONT 25
#define SIGTTIN 26
#define SIGTTOU 27
#define SIGVTALRM 28
#define SIGPROF 29
#define SIGXCPU 30
#define SIGXFSZ 31

#define SIG_BLOCK 1
#define SIG_UNBLOCK 2
#define SIG_SETMASK 3

#define SIGNO_MASK 0xFF
#define SIGDEFER 0x100
#define SIGHOLD 0x200
#define SIGRELSE 0x400
#define SIGIGNORE 0x800
#define SIGPAUSE 0x1000

struct sigaction {
	int sa_flags;
	union {
		__sighandler sa_handler;
		void (*sa_sigaction)(int, struct siginfo *, void *);
	} __sa_handler;
	sigset_t sa_mask;
	int sa_resv[2];
};

#define sa_handler __sa_handler.sa_handler
#define sa_sigaction __sa_handler.sa_sigaction

#define SA_ONSTACK 0x00000001
#define SA_RESETHAND 0x00000002
#define SA_RESTART 0x00000004
#define SA_SIGINFO 0x00000008
#define SA_NODEFER 0x00000010
#define SA_NOCLDWAIT 0x00010000
#define SA_NOCLDSTOP 0x00020000

#define SA_NOMASK SA_NODEFER
#define SA_ONESHOT SA_RESETHAND

#define NSIG 32
#define MAXSIG 32

#define MINSIGSTKSZ 512
#define SIGSTKSZ 8192

#define SS_ONSTACK 0x00000001
#define SS_DISABLE 0x00000002

struct sigaltstack {
	char *ss_sp;
	int ss_size;
	int ss_flags;
};

typedef struct sigaltstack stack_t;

#define SIGEV_SIGNAL 0
#define SIGEV_NONE 1
#define SIGEV_THREAD 2
#define SIGEV_THREAD_ID 4

#if defined(_DEFAULT_SOURCE) || (__MLIBC_POSIX1 && !__MLIBC_POSIX2024)
#define POLL_IN 1
#define POLL_OUT 2
#define POLL_MSG 3
#define POLL_ERR 4
#define POLL_PRI 5
#define POLL_HUP 6
#endif

#define SI_FROMUSER(sip) ((sip)->si_code <= 0)
#define SI_FROMKERNEL(sip) ((sip)->si_code > 0)

#define SI_ASYNCIO (-4)
#define SI_MESGQ (-3)
#define SI_TIMER (-2)
#define SI_QUEUE (-1)
#define SI_USER 0

#define ILL_ILLOPC 1
#define ILL_ILLOPN 2
#define ILL_ILLADR 3
#define ILL_ILLTRP 4
#define ILL_PRVOPC 5
#define ILL_PRVREG 6
#define ILL_COPROC 7
#define ILL_BADSTK 8

#define FPE_INTDIV 1
#define FPE_INTOVF 2
#define FPE_FLTDIV 3
#define FPE_FLTOVF 4
#define FPE_FLTUND 5
#define FPE_FLTRES 6
#define FPE_FLTINV 7
#define FPE_FLTSUB 8

#define SEGV_MAPERR 1
#define SEGV_ACCERR 2

#define BUS_ADRALN 1
#define BUS_ADRERR 2
#define BUS_OBJERR 3

#define TRAP_BRKPT 1
#define TRAP_TRACE 2

#define CLD_EXITED 1
#define CLD_KILLED 2
#define CLD_DUMPED 3
#define CLD_TRAPPED 4
#define CLD_STOPPED 5
#define CLD_CONTINUED 6

#define SI_MAXSZ 128
#define SI_PAD ((SI_MAXSZ / sizeof(int)) - 3)

typedef struct siginfo {
	int si_signo;
	int si_code;
	int si_errno;
	union {
		int _pad[SI_PAD];
		struct {
			pid_t _pid;
			union {
				struct {
					uid_t _uid;
					union sigval _value;
				} _kill;
				struct {
					clock_t _utime;
					int _status;
					clock_t _stime;
				} _cld;
			} _pdata;
		} _proc;
		struct {
			void *_addr;
		} _fault;
		struct {
			int _fd;
			long _band;
		} _file;
	} _data;
} siginfo_t;

#define si_pid _data._proc._pid
#define si_status _data._proc._pdata._cld._status
#define si_stime _data._proc._pdata._cld._stime
#define si_utime _data._proc._pdata._cld._utime
#define si_uid _data._proc._pdata._kill._uid
#define si_value _data._proc._pdata._kill._value
#define si_ptr si_value.sival_ptr
#define si_int si_value.sival_int
#define si_addr _data._fault._addr
#define si_fd _data._file._fd
#define si_band _data._file._band

#if !defined(__i386__)
#error "Missing SVR4 signal ABI definitions for this architecture."
#endif

typedef int greg_t;

#define REG_GS 0
#define REG_FS 1
#define REG_ES 2
#define REG_DS 3
#define REG_EDI 4
#define REG_ESI 5
#define REG_EBP 6
#define REG_ESP 7
#define REG_EBX 8
#define REG_EDX 9
#define REG_ECX 10
#define REG_EAX 11
#define REG_TRAPNO 12
#define REG_ERR 13
#define REG_EIP 14
#define REG_CS 15
#define REG_EFL 16
#define REG_UESP 17
#define REG_SS 18
#define NGREG 19

typedef greg_t gregset_t[NGREG];

typedef struct fpregset {
	union {
		struct {
			int state[27];
			int status;
		} fpchip_state;
		struct {
			char fp_emul[246];
			char fp_epad[2];
		} fp_emul_space;
		int f_fpregs[62];
	} fp_reg_set;
	long f_wregs[33];
} fpregset_t;

typedef struct {
	gregset_t gregs;
	fpregset_t fpregs;
} mcontext_t;

typedef struct ucontext {
	unsigned long uc_flags;
	struct ucontext *uc_link;
	sigset_t uc_sigmask;
	stack_t uc_stack;
	mcontext_t uc_mcontext;
	long uc_filler[5];
} ucontext_t;

#define GETCONTEXT 0
#define SETCONTEXT 1

#define UC_SIGMASK 001
#define UC_STACK 002
#define UC_CPU 004
#define UC_FP 010
#define UC_MCONTEXT (UC_CPU | UC_FP)
#define UC_ALL (UC_SIGMASK | UC_STACK | UC_MCONTEXT)

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_SIGNAL_H */