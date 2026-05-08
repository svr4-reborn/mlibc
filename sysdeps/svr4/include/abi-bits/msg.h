#ifndef _ABIBITS_MSG_H
#define _ABIBITS_MSG_H

#include <sys/ipc.h>
#include <bits/ansi/time_t.h>
#include <abi-bits/pid_t.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long msglen_t;
typedef unsigned long msgqnum_t;

struct msqid_ds {
	struct ipc_perm msg_perm;
	void *msg_first;
	void *msg_last;
	unsigned long msg_cbytes;
	msgqnum_t msg_qnum;
	msglen_t msg_qbytes;
	pid_t msg_lspid;
	pid_t msg_lrpid;
	time_t msg_stime;
	long msg_pad1;
	time_t msg_rtime;
	long msg_pad2;
	time_t msg_ctime;
	long msg_pad3;
	long msg_pad4[4];
};

struct msgbuf {
	long mtype;
	char mtext[1];
};

#define MSG_R 0400
#define MSG_W 0200
#define MSG_RWAIT 01000
#define MSG_WWAIT 02000
#define MSG_NOERROR 010000

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_MSG_H */

