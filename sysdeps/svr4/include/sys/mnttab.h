#ifndef _SYS_MNTTAB_H
#define _SYS_MNTTAB_H

#include <stdio.h>

#define MNTTAB "/etc/mnttab"
#define MNT_LINE_MAX 1024

#define MNT_TOOLONG 1
#define MNT_TOOMANY 2
#define MNT_TOOFEW 3

#define mntnull(mp) \
	((mp)->mnt_special = (mp)->mnt_mountp = \
	 (mp)->mnt_fstype = (mp)->mnt_mntopts = \
	 (mp)->mnt_time = NULL)

#define putmntent(fd, mp) \
	fprintf((fd), "%s\t%s\t%s\t%s\t%s\n", \
		(mp)->mnt_special ? (mp)->mnt_special : "-", \
		(mp)->mnt_mountp ? (mp)->mnt_mountp : "-", \
		(mp)->mnt_fstype ? (mp)->mnt_fstype : "-", \
		(mp)->mnt_mntopts ? (mp)->mnt_mntopts : "-", \
		(mp)->mnt_time ? (mp)->mnt_time : "-")

struct mnttab {
	char *mnt_special;
	char *mnt_mountp;
	char *mnt_fstype;
	char *mnt_mntopts;
	char *mnt_time;
};

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __MLIBC_ABI_ONLY
int getmntent(FILE *__stream, struct mnttab *__entry);
int getmntany(FILE *__stream, struct mnttab *__entry, struct mnttab *__ref);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MNTTAB_H */