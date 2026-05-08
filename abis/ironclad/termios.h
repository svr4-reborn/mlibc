#ifndef _ABIBITS_TERMIOS_H
#define _ABIBITS_TERMIOS_H

typedef unsigned int cc_t;
typedef unsigned int speed_t;
typedef unsigned int tcflag_t;

/* baud rate constants for speed_t */
#define B0       0
#define B50      1
#define B75      2
#define B110     3
#define B134     4
#define B150     5
#define B200     6
#define B300     7
#define B600     8
#define B1200    9
#define B1800    10
#define B2400    11
#define B4800    12
#define B9600    13
#define B19200   14
#define B38400   15
#define B57600   0010001
#define B115200  0010002
#define B230400  0010003
#define B460800  0010004
#define B500000  0010005
#define B576000  0010006
#define B921600  0010007
#define B1000000 0010010
#define B1152000 0010011
#define B1500000 0010012
#define B2000000 0010013
#define B2500000 0010014
#define B3000000 0010015
#define B3500000 0010016
#define B4000000 0010017

/* constants for tcsetattr() */
#define TCSANOW 0
#define TCSADRAIN 1
#define TCSAFLUSH 2

/* constants for tcflush() */
#define TCIFLUSH 0
#define TCOFLUSH 1
#define TCIOFLUSH 2

/* constants for tcflow() */
#define TCOOFF 0
#define TCOON 1
#define TCIOFF 2
#define TCION 3

#if defined(_DEFAULT_SOURCE)
#define TIOCM_DTR 0x002
#define TIOCM_RTS 0x004
#define TIOCM_CTS 0x020
#define TIOCM_CAR 0x040
#define TIOCM_RNG 0x080
#define TIOCM_DSR 0x100
#define TIOCM_RI TIOCM_RNG
#define TIOCM_CD TIOCM_CAR
#endif /* defined(_DEFAULT_SOURCE) */

/* indices for the c_cc array in struct termios */
#define NCCS    11
#define VEOF     0
#define VEOL     1
#define VERASE   2
#define VINTR    3
#define VKILL    4
#define VMIN     5
#define VQUIT    6
#define VSTART   7
#define VSTOP    8
#define VSUSP    9
#define VTIME   10

/* bitwise flags for c_iflag in struct termios */
#define BRKINT  0x0001
#define ICRNL   0x0002
#define IGNBRK  0x0004
#define IGNCR   0x0008
#define IGNPAR  0x0010
#define INLCR   0x0020
#define INPCK   0x0040
#define ISTRIP  0x0080
#define IXANY   0x0100
#define IXOFF   0x0200
#define IXON    0x0400
#define PARMRK  0x0800

/* bitwise flags for c_oflag in struct termios */
#define OPOST   0x0001
#define ONLCR   0x0002
#define OCRNL   0x0004
#define ONOCR   0x0008
#define ONLRET  0x0010
#define OFDEL   0x0020
#define OFILL   0x0040

#define NLDLY   0x0080
#define NL0     0x0000
#define NL1     0x0080

#define CRDLY   0x0300
#define CR0     0x0000
#define CR1     0x0100
#define CR2     0x0200
#define CR3     0x0300

#define TABDLY  0x0C00
#define TAB0    0x0000
#define TAB1    0x0400
#define TAB2    0x0800
#define TAB3    0x0C00

#define BSDLY   0x1000
#define BS0     0x0000
#define BS1     0x1000

#define VTDLY   0x2000
#define VT0     0x0000
#define VT1     0x2000

#define FFDLY   0x4000
#define FF0     0x0000
#define FF1     0x4000

/* bitwise constants for c_cflag in struct termios */
#define CSIZE   0x0003
#define CS5     0x0000
#define CS6     0x0001
#define CS7     0x0002
#define CS8     0x0003

#define CSTOPB  0x0004
#define CREAD   0x0008
#define PARENB  0x0010
#define PARODD  0x0020
#define HUPCL   0x0040
#define CLOCAL  0x0080

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define CBAUD   0x100F
#endif

/* bitwise constants for c_lflag in struct termios */
#define ECHO    0x0001
#define ECHOE   0x0002
#define ECHOK   0x0004
#define ECHONL  0x0008
#define ICANON  0x0010
#define IEXTEN  0x0020
#define ISIG    0x0040
#define NOFLSH  0x0080
#define TOSTOP  0x0100

struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_cc[NCCS];
	speed_t c_ibaud;
	speed_t c_obaud;
};

#if defined(_DEFAULT_SOURCE)
#define TIOCSCTTY 0x540E
#define TIOCGPGRP 0x540F
#define TIOCSPGRP 0x5410
#define TIOCGWINSZ 0x5413
#define TIOCSWINSZ 0x5414
#define TIOCGSID 0x5429
#endif /* defined(_DEFAULT_SOURCE) */

#endif
