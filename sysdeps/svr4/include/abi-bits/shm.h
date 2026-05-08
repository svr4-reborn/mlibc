#ifndef _ABIBITS_SHM_H
#define _ABIBITS_SHM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bits/ansi/time_t.h>
#include <abi-bits/pid_t.h>
#include <sys/ipc.h>

#define SHM_R 0400
#define SHM_W 0200

#define SHM_RDONLY 010000
#define SHM_RND 020000
#define SHM_INIT 01000
#define SHM_DEST 02000

#define SHM_LOCKED 001000
#define SHM_LOCKWAIT 010000

#define SHM_LOCK 3
#define SHM_UNLOCK 4

typedef unsigned long shmatt_t;
struct shmid_ds {
	struct ipc_perm shm_perm;
	int shm_segsz;
	void *shm_amp;
	unsigned short shm_lkcnt;
	pid_t shm_lpid;
	pid_t shm_cpid;
	unsigned long shm_nattch;
	unsigned long shm_cnattch;
	time_t shm_atime;
	long shm_pad1;
	time_t shm_dtime;
	long shm_pad2;
	time_t shm_ctime;
	long shm_pad3;
	long shm_pad4[4];
};

struct shminfo {
	int shmmax;
	int shmmin;
	int shmmni;
	int shmseg;
};

#define SHMLBA 4096

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_SHM_H */
