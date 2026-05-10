#ifndef _SYS_MOUNT_H
#define _SYS_MOUNT_H

#ifdef __cplusplus
extern "C" {
#endif

// Keep the public shape close to Linux userspace while using the native SVR4
// flag values where the semantics line up.
#define MS_RDONLY 1
#define MS_HADBAD 8
#define MS_NOSUID 16
#define MS_REMOUNT 32
#define MS_NOTRUNC 64

int mount(const char *source, const char *target,
		const char *fstype, unsigned long flags, const void *data);
int umount(const char *target);
int umount2(const char *target, int flags);

#ifdef __cplusplus
}
#endif

#endif /* _SYS_MOUNT_H */