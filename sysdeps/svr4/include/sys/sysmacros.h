#ifndef _SYS_SYSMACROS_H
#define _SYS_SYSMACROS_H

#include <abi-bits/dev_t.h>

#define O_BITSMAJOR 7
#define O_BITSMINOR 8
#define O_MAXMAJ 0x7f
#define O_MAXMIN 0xff

#define L_BITSMAJOR 14
#define L_BITSMINOR 18
#define L_MAXMAJ 0xff
#define L_MAXMIN 0x3ffff

#define major(x) ((int)(((unsigned long)(x) >> O_BITSMINOR) & O_MAXMAJ))
#define minor(x) ((int)((unsigned long)(x) & O_MAXMIN))

#define makedev(x, y) \
	((dev_t)((((unsigned long)(x)) << O_BITSMINOR) \
		| (((unsigned long)(y)) & O_MAXMIN)))

#define getmajor(x) ((int)(((unsigned long)(x) >> L_BITSMINOR) & L_MAXMAJ))
#define getminor(x) ((int)((unsigned long)(x) & L_MAXMIN))

#define makedevice(x, y) \
	((dev_t)((((unsigned long)(x)) << L_BITSMINOR) \
		| (((unsigned long)(y)) & L_MAXMIN)))

#endif /* _SYS_SYSMACROS_H */