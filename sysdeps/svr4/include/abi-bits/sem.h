#ifndef _ABIBITS_SEM_H
#define _ABIBITS_SEM_H

#include <abi-bits/time.h>
#include <abi-bits/ipc.h>
#include <abi-bits/pid_t.h>

#define SEM_A 0200
#define SEM_R 0400

#define GETNCNT 3
#define GETPID 4
#define GETVAL 5
#define GETALL 6
#define GETZCNT 7
#define SETVAL 8
#define SETALL 9

#define SEM_UNDO 0x1000

struct sem {
	unsigned short semval;
	pid_t sempid;
	unsigned short semncnt;
	unsigned short semzcnt;
};

struct sembuf {
	unsigned short int sem_num;
	short int sem_op;
	short int sem_flg;
};

struct semid_ds {
	struct ipc_perm sem_perm;
	struct sem *sem_base;
	unsigned short sem_nsems;
	time_t sem_otime;
	long sem_pad1;
	time_t sem_ctime;
	long sem_pad2;
	long sem_pad3[4];
};

#endif /* _ABIBITS_SEM_H */
