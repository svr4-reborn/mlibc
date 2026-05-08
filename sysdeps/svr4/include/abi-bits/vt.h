#ifndef _ABIBITS_VT_H
#define _ABIBITS_VT_H

#define VTIOC (('v' << 8))

#define VT_OPENQRY (VTIOC | 1)
#define VT_SETMODE (VTIOC | 2)
#define VT_GETMODE (VTIOC | 3)
#define VT_RELDISP (VTIOC | 4)
#define VT_ACTIVATE (VTIOC | 5)
#define VT_WAITACTIVE (VTIOC | 6)
#define VT_GETSTATE (VTIOC | 100)
#define VT_SENDSIG (VTIOC | 101)

#define VT_AUTO 0
#define VT_PROCESS 1
#define VT_ACKACQ 2

struct vt_mode {
	char mode;
	char waitv;
	short relsig;
	short acqsig;
	short frsig;
};

struct vt_stat {
	unsigned short v_active;
	unsigned short v_signal;
	unsigned short v_state;
};

#define TIOCVTNAME ((('T' << 8)) | 250)
#define VTNAMESZ 32

#endif /* _ABIBITS_VT_H */