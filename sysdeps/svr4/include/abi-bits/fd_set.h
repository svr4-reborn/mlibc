#ifndef _ABIBITS_FD_SET_H
#define _ABIBITS_FD_SET_H

#define FD_SETSIZE 1024

typedef long fd_mask;

typedef struct fd_set {
	fd_mask fds_bits[(FD_SETSIZE + ((sizeof(fd_mask) * 8) - 1)) / (sizeof(fd_mask) * 8)];
} fd_set;

#endif /* _ABIBITS_FD_SET_H */
