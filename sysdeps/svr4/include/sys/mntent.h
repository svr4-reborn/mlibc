#ifndef _SYS_MNTENT_H
#define _SYS_MNTENT_H

#define MNTTAB "/etc/mnttab"
#define VFSTAB "/etc/vfstab"

#define MNTTYPE_UFS "ufs"
#define MNTTYPE_SWAP "swap"
#define MNTTYPE_IGNORE "ignore"
#define MNTTYPE_LO "lo"

#define MNTOPT_RO "ro"
#define MNTMAXSTR 128
#define MNTOPT_RW "rw"
#define MNTOPT_RQ "rq"
#define MNTOPT_QUOTA "quota"
#define MNTOPT_NOQUOTA "noquota"
#define MNTOPT_SOFT "soft"
#define MNTOPT_HARD "hard"
#define MNTOPT_NOSUID "nosuid"
#define MNTOPT_NOAUTO "noauto"
#define MNTOPT_GRPID "grpid"
#define MNTOPT_REMOUNT "remount"
#define MNTOPT_NOSUB "nosub"
#define MNTOPT_MULTI "multi"
#define MNTOPT_IGNORE "ignore"

#ifdef __cplusplus
extern "C" {
#endif

struct mnttab;

#ifndef __MLIBC_ABI_ONLY
char *hasmntopt(struct mnttab *__mnt, const char *__opt);
#endif

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MNTENT_H */