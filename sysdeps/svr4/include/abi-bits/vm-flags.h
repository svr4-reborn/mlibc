#ifndef _ABIBITS_VM_FLAGS_H
#define _ABIBITS_VM_FLAGS_H

#define PROT_NONE  0x00
#define PROT_READ  0x01
#define PROT_WRITE 0x02
#define PROT_EXEC  0x04

#define MAP_FAILED ((void *)(-1))
#define MAP_FILE    0x00
#define MAP_SHARED    0x01
#define MAP_PRIVATE   0x02
#define MAP_FIXED     0x10
#define MAP_RENAME    0x20
#define MAP_NORESERVE 0x40
#define MAP_ANON      0x80
#define MAP_ANONYMOUS MAP_ANON
#define _MAP_NEW      0x80000000u

#define MS_SYNC 0x00
#define MS_ASYNC 0x01
#define MS_INVALIDATE 0x02

#define MCL_CURRENT 0x01
#define MCL_FUTURE 0x02

#define POSIX_MADV_NORMAL 0
#define POSIX_MADV_RANDOM 1
#define POSIX_MADV_SEQUENTIAL 2
#define POSIX_MADV_WILLNEED 3
#define POSIX_MADV_DONTNEED 4

#endif /* _ABIBITS_VM_FLAGS_H */
