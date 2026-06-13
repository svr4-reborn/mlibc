#ifndef _ABIBITS_FCNTL_H
#define _ABIBITS_FCNTL_H

#include <abi-bits/pid_t.h>

#define O_RDONLY 0
#define O_WRONLY 1
#define O_RDWR 2
#define O_ACCMODE 3

#define O_NDELAY 0x04
#define O_APPEND 0x08
#define O_SYNC 0x10
#define O_RAIOSIG 0x20
#define O_NONBLOCK 0x80

#define O_CREAT 0x100
#define O_TRUNC 0x200
#define O_EXCL 0x400
#define O_NOCTTY 0x800

#define O_CLOEXEC 0x2000000 /* TODO: implement this in the kernel */
#define O_NOFOLLOW 0x4000000 /* TODO: implement this in the kernel */
#define O_DIRECTORY 0x8000000 /* TODO: implement this in the kernel */

#define F_DUPFD  0
#define F_GETFD  1
#define F_SETFD  2
#define F_GETFL  3
#define F_SETFL  4

#define F_GETLK 14
#define F_SETLK 6
#define F_SETLKW 7

#define F_CHKFL 8
#define F_ALLOCSP 10
#define F_FREESP 11
#define F_RSETLK 20
#define F_RGETLK 21
#define F_RSETLKW 22
#define F_GETOWN 23
#define F_SETOWN 24
#define F_DUP2FD 25
#define F_DUP2FD_CLOEXEC 26

#define F_CHSIZE 0x6000
#define F_RDCHK 0x6001

#define F_LK_UNLCK 0x6300
#define F_LK_LOCK 0x7200
#define F_LK_NBLCK 0x6200
#define F_LK_RLCK 0x7100
#define F_LK_NBRLCK 0x6100

#define F_RDLCK 01
#define F_WRLCK 02
#define F_UNLCK 03

#define FD_CLOEXEC 1

// The kernel doesn't implement `fadvise`, but we need these for the POSIX interface.
// The fadvise function will correctly return ENOSYS so it's not that bad.
#define POSIX_FADV_NORMAL 0
#define POSIX_FADV_RANDOM 1
#define POSIX_FADV_SEQUENTIAL 2
#define POSIX_FADV_WILLNEED 3
#define POSIX_FADV_DONTNEED 4
#define POSIX_FADV_NOREUSE 5

#define AT_FDCWD -100
#define AT_SYMLINK_NOFOLLOW 0x100
#define AT_REMOVEDIR 0x200
#define AT_EACCESS 0x200
#define AT_SYMLINK_FOLLOW 0x400
#define AT_EMPTY_PATH 0x1000

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

#endif /* _ABIBITS_FCNTL_H */
