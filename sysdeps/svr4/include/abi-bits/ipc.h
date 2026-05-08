#ifndef _ABIBITS_IPC_H
#define _ABIBITS_IPC_H

#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>
#include <abi-bits/mode_t.h>

#ifdef __cplusplus
extern "C" {
#endif

#define IPC_ALLOC 0100000
#define IPC_CREAT 01000
#define IPC_EXCL 02000
#define IPC_NOWAIT 04000

#define IPC_RMID 10
#define IPC_SET 11
#define IPC_STAT 12

#define IPC_PRIVATE ((key_t) 0)

typedef int key_t;

struct ipc_perm {
	uid_t uid;
	gid_t gid;
	uid_t cuid;
	gid_t cgid;
	mode_t mode;
	unsigned long seq;
	key_t key;
	long pad[4];
};

#ifdef __cplusplus
}
#endif

#endif
