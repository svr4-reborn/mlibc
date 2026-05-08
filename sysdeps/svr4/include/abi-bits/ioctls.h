#ifndef _ABIBITS_IOCTLS_H
#define _ABIBITS_IOCTLS_H

#include <abi-bits/termios.h>

#define FIOCLEX   0x20006601
#define FIONCLEX  0x20006602
#define FIONREAD  0x4004667f
#define FIONBIO   0x8004667e
#define FIOASYNC  0x8004667d
#define FIOSETOWN 0x8004667c
#define FIOGETOWN 0x4004667b

#define STR       ('S' << 8)

#define I_NREAD   (STR | 0x01)
#define I_PUSH    (STR | 0x02)
#define I_POP     (STR | 0x03)
#define I_LOOK    (STR | 0x04)
#define I_FLUSH   (STR | 0x05)
#define I_SRDOPT  (STR | 0x06)
#define I_GRDOPT  (STR | 0x07)
#define I_STR     (STR | 0x08)
#define I_SETSIG  (STR | 0x09)
#define I_GETSIG  (STR | 0x0a)
#define I_FIND    (STR | 0x0b)
#define I_LINK    (STR | 0x0c)
#define I_UNLINK  (STR | 0x0d)
#define I_PEEK    (STR | 0x0f)
#define I_FDINSERT (STR | 0x10)
#define I_SENDFD  (STR | 0x11)
#define I_RECVFD  (STR | 0x12)
#define I_SWROPT  (STR | 0x13)
#define I_GWROPT  (STR | 0x14)
#define I_LIST    (STR | 0x15)
#define I_PLINK   (STR | 0x16)
#define I_PUNLINK (STR | 0x17)
#define I_SETEV   (STR | 0x18)
#define I_GETEV   (STR | 0x19)
#define I_STREV   (STR | 0x1a)
#define I_UNSTREV (STR | 0x1b)
#define I_FLUSHBAND (STR | 0x1c)
#define I_CKBAND  (STR | 0x1d)
#define I_GETBAND (STR | 0x1e)
#define I_ATMARK  (STR | 0x1f)
#define I_SETCLTIME (STR | 0x20)
#define I_GETCLTIME (STR | 0x21)
#define I_CANPUT  (STR | 0x22)

#endif /* _ABIBITS_IOCTLS_H */