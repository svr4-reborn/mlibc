#ifndef _ABIBITS_SIGSET_T_H
#define _ABIBITS_SIGSET_T_H

typedef struct {
	union {
		unsigned long sigbits[4];
		unsigned long __sig[4];
	};
} sigset_t;

#endif /* _ABIBITS_SIGSET_T_H */
