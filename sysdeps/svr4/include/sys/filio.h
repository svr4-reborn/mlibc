#ifndef _SYS_FILIO_H
#define _SYS_FILIO_H

/* SVR4 provides the file ioctls in this file rather than in <sys/ioctl.h>. */

#define FIOCLEX   0x20006601
#define FIONCLEX  0x20006602
#define FIONREAD  0x4004667f
#define FIONBIO   0x8004667e
#define FIOASYNC  0x8004667d
#define FIOSETOWN 0x8004667c
#define FIOGETOWN 0x4004667b

#endif /* _SYS_FILIO_H */