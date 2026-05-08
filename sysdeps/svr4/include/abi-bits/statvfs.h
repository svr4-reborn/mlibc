#ifndef _ABIBITS_STATVFS_H
#define _ABIBITS_STATVFS_H

#include <mlibc-config.h>

#include <abi-bits/fsblkcnt_t.h>
#include <abi-bits/fsfilcnt_t.h>

#define FSTYPSZ 16

#define ST_RDONLY 1
#define ST_NOSUID 2
#define ST_NOTRUNC 4

struct statvfs {
	unsigned long f_bsize;
	unsigned long f_frsize;
	fsblkcnt_t f_blocks;
	fsblkcnt_t f_bfree;
	fsblkcnt_t f_bavail;
	fsfilcnt_t f_files;
	fsfilcnt_t f_ffree;
	fsfilcnt_t f_favail;
	unsigned long f_fsid;
	char f_basetype[FSTYPSZ];
	unsigned long f_flag;
	unsigned long f_namemax;
	char f_fstr[32];
	unsigned long f_filler[16];
};

#if __MLIBC_LINUX_OPTION && defined(_LARGEFILE64_SOURCE)
typedef struct statvfs statvfs64;
#endif /* __MLIBC_LINUX_OPTION && defined(_LARGEFILE64_SOURCE) */

#endif /* _ABIBITS_STATVFS_H */

