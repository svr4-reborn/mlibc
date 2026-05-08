#ifndef _STROPTS_H
#define _STROPTS_H

#include <sys/types.h>
#include <abi-bits/ioctls.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RNORM      0x000
#define RMSGD      0x001
#define RMSGN      0x002

#define RPROTDAT   0x004
#define RPROTDIS   0x008
#define RPROTNORM  0x010

#define FLUSHR     0x01
#define FLUSHW     0x02
#define FLUSHRW    0x03
#define FLUSHBAND  0x04

#define ANYMARK    0x01
#define LASTMARK   0x02

#define MSG_HIPRI  0x01
#define MSG_ANY    0x02
#define MSG_BAND   0x04

#define MORECTL    0x01
#define MOREDATA   0x02

#define FMNAMESZ   8
#define INFTIM     (-1)

struct strbuf {
	int maxlen;
	int len;
	char *buf;
};

struct strioctl {
	int ic_cmd;
	int ic_timout;
	int ic_len;
	char *ic_dp;
};

struct strpeek {
	struct strbuf ctlbuf;
	struct strbuf databuf;
	long flags;
};

struct strfdinsert {
	struct strbuf ctlbuf;
	struct strbuf databuf;
	long flags;
	int fildes;
	int offset;
};

struct strrecvfd {
	int fd;
	uid_t uid;
	gid_t gid;
	char fill[8];
};

struct str_mlist {
	char l_name[FMNAMESZ + 1];
};

struct str_list {
	int sl_nmods;
	struct str_mlist *sl_modlist;
};

#ifdef __cplusplus
}
#endif

#endif /* _STROPTS_H */