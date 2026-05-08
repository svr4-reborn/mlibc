#ifndef _ABIBITS_STAT_H
#define _ABIBITS_STAT_H

#include <abi-bits/uid_t.h>
#include <abi-bits/gid_t.h>
#include <bits/off_t.h>
#include <abi-bits/mode_t.h>
#include <abi-bits/dev_t.h>
#include <abi-bits/ino_t.h>
#include <abi-bits/blksize_t.h>
#include <abi-bits/blkcnt_t.h>
#include <abi-bits/nlink_t.h>
#include <bits/ansi/time_t.h>
#include <bits/ansi/timespec.h>

#define _ST_FSTYPSZ 16

#define S_IFMT 0x0F000
#define S_IFBLK 0x06000
#define S_IFCHR 0x02000
#define S_IFIFO 0x01000
#define S_IFREG 0x08000
#define S_IFDIR 0x04000
#define S_IFLNK 0x0A000
#define S_IFSOCK 0x0C000

#define S_IRWXU 0700
#define S_IRUSR 0400
#define S_IWUSR 0200
#define S_IXUSR 0100
#define S_IRWXG 070
#define S_IRGRP 040
#define S_IWGRP 020
#define S_IXGRP 010
#define S_IRWXO 07
#define S_IROTH 04
#define S_IWOTH 02
#define S_IXOTH 01
#define S_ISUID 04000
#define S_ISGID 02000
#define S_ISVTX 01000

#define S_IREAD  S_IRUSR
#define S_IWRITE S_IWUSR
#define S_IEXEC  S_IXUSR

#define _R3_STAT_VER 1
#define _STAT_VER 2

#ifdef __cplusplus
extern "C" {
#endif

struct stat {
	dev_t st_dev;
	long st_pad1[3];
	ino_t st_ino;
	mode_t st_mode;
	nlink_t st_nlink;
	uid_t st_uid;
	gid_t st_gid;
	dev_t st_rdev;
	long st_pad2[2];
	off_t st_size;
	long st_pad3;
	struct timespec st_atim;
	struct timespec st_mtim;
	struct timespec st_ctim;
	long st_blksize;
	long st_blocks;
	char st_fstype[_ST_FSTYPSZ];
	long st_pad4[8];
};

#define st_atime st_atim.tv_sec
#define st_mtime st_mtim.tv_sec
#define st_ctime st_ctim.tv_sec

#if defined(_DEFAULT_SOURCE) || defined(_LARGEFILE64_SOURCE)
#define stat64 stat
#endif

#ifdef __cplusplus
}
#endif

#endif /* _ABIBITS_STAT_H */
