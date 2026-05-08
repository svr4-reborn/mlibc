#ifndef _ABIBITS_TERMIOS_H
#define _ABIBITS_TERMIOS_H

typedef unsigned char cc_t;
typedef unsigned long speed_t;
typedef unsigned long tcflag_t;

/* baud rate constants for speed_t */
#define B0     0
#define B50    1
#define B75    2
#define B110   3
#define B134   4
#define B150   5
#define B200   6
#define B300   7
#define B600   8
#define B1200  9
#define B1800  10
#define B2400  11
#define B4800  12
#define B9600  13
#define B19200 14
#define B38400 15

/* constants for tcsetattr() */
#define TCSANOW   (('T' << 8) | 14)
#define TCSADRAIN (('T' << 8) | 15)
#define TCSAFLUSH (('T' << 8) | 16)

/* constants for tcflush() */
#define TCIFLUSH  0
#define TCOFLUSH  1
#define TCIOFLUSH 2

/* constants for tcflow() */
#define TCOOFF 0
#define TCOON  1
#define TCIOFF 2
#define TCION  3

/* indices for the c_cc array in struct termios */
#define NCCS   19
#define VINTR  0
#define VQUIT  1
#define VERASE 2
#define VKILL  3
#define VEOF   4
#define VEOL   5
#define VMIN   4
#define VTIME  5
#define VEOL2  6
#define VSWTCH 7
#define VSWTC  VSWTCH
#define VSTART 8
#define VSTOP  9
#define VSUSP  10
#define VREPRINT 12
#define VDISCARD 13
#define VWERASE  14
#define VLNEXT   15

/* bitwise flags for c_iflag in struct termios */
#define IGNBRK 0000001
#define BRKINT 0000002
#define IGNPAR 0000004
#define PARMRK 0000010
#define INPCK  0000020
#define ISTRIP 0000040
#define INLCR  0000100
#define IGNCR  0000200
#define ICRNL  0000400

#define IUCLC 0001000

#define IXON 0002000
#define IXANY 0004000

#define IXOFF 0010000

#if defined(_DEFAULT_SOURCE)
#define IMAXBEL 0020000
#define DOSMODE 0100000
#endif

/* bitwise flags for c_oflag in struct termios */
#define OPOST 0000001
#define OLCUC  0000002
#define ONLCR  0000004
#define OCRNL  0000010
#define ONOCR  0000020
#define ONLRET 0000040
#define OFILL  0000100
#define OFDEL  0000200

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE) || defined(_XOPEN_SOURCE)

#define NLDLY  0000400
#define NL0    0
#define NL1    0000400

#define CRDLY  0003000
#define CR0    0
#define CR1    0001000
#define CR2    0002000
#define CR3    0003000

#define TABDLY 0014000
#define TAB0   0
#define TAB1   0004000
#define TAB2   0010000
#define TAB3   0014000

#define BSDLY  0020000
#define BS0    0
#define BS1    0020000

#define FFDLY  0100000
#define FF0    0
#define FF1    0100000
#endif

#define VTDLY  0040000
#define VT0    0
#define VT1    0040000

#if defined(_DEFAULT_SOURCE)
#define XTABS   0014000
#define PAGEOUT 0200000
#define WRAP    0400000
#endif

/* bitwise constants for c_cflag in struct termios */
#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define CBAUD 0000017
#define CIBAUD 03600000
#define PAREXT 04000000
#endif

#define CSIZE  0000060
#define CS5    0
#define CS6    0000020
#define CS7    0000040
#define CS8    0000060
#define CSTOPB 0000100
#define CREAD  0000200
#define PARENB 0000400
#define PARODD 0001000
#define HUPCL  0002000
#define CLOCAL 0004000

#if defined(_DEFAULT_SOURCE)
#define RCV1EN 0010000
#define XMT1EN 0020000
#define LOBLK  0040000
#define XCLUDE 0100000
#endif

/* bitwise constants for c_lflag in struct termios */
#define ISIG   0000001
#define ICANON 0000002

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define XCASE 0000004
#endif

#define ECHO   0000010
#define ECHOE  0000020
#define ECHOK  0000040
#define ECHONL 0000100
#define NOFLSH 0000200
#define TOSTOP 0000400

#if defined(_GNU_SOURCE) || defined(_BSD_SOURCE)
#define ECHOCTL 0001000
#define ECHOPRT 0002000
#define ECHOKE  0004000
#define FLUSHO  0020000
#define PENDIN  0040000
#endif

#if defined(_DEFAULT_SOURCE)
#define DEFECHO 0010000
#endif

#define IEXTEN 0100000

struct termios {
	tcflag_t c_iflag;
	tcflag_t c_oflag;
	tcflag_t c_cflag;
	tcflag_t c_lflag;
	cc_t c_cc[NCCS];
};

#if defined(_DEFAULT_SOURCE)
#define TCGETA    (('T' << 8) | 1)
#define TCSETA    (('T' << 8) | 2)
#define TCSETAW   (('T' << 8) | 3)
#define TCSETAF   (('T' << 8) | 4)
#define TCSBRK    (('T' << 8) | 5)
#define TCXONC    (('T' << 8) | 6)
#define TCFLSH    (('T' << 8) | 7)

#define TIOCGWINSZ (('T' << 8) | 104)
#define TIOCSWINSZ (('T' << 8) | 103)

#define TCGETS    (('T' << 8) | 13)
#define TCSETS    (('T' << 8) | 14)
#define TCSETSW   (('T' << 8) | 15)
#define TCSETSF   (('T' << 8) | 16)

#define TIOCGETD  (('t' << 8) | 0)
#define TIOCSETD  (('t' << 8) | 1)
#define TIOCHPCL  (('t' << 8) | 2)
#define TIOCGETP  (('t' << 8) | 8)
#define TIOCSETP  (('t' << 8) | 9)
#define TIOCSETN  (('t' << 8) | 10)
#define TIOCEXCL  (('t' << 8) | 13)
#define TIOCNXCL  (('t' << 8) | 14)
#define TIOCFLUSH (('t' << 8) | 16)
#define TIOCSETC  (('t' << 8) | 17)
#define TIOCGETC  (('t' << 8) | 18)

#define TIOCLBIS  (('t' << 8) | 127)
#define TIOCLBIC  (('t' << 8) | 126)
#define TIOCLSET  (('t' << 8) | 125)
#define TIOCLGET  (('t' << 8) | 124)
#define TIOCSBRK  (('t' << 8) | 123)
#define TIOCCBRK  (('t' << 8) | 122)
#define TIOCSDTR  (('t' << 8) | 121)
#define TIOCCDTR  (('t' << 8) | 120)
#define TIOCSLTC  (('t' << 8) | 117)
#define TIOCGLTC  (('t' << 8) | 116)
#define TIOCOUTQ  (('t' << 8) | 115)
#define TIOCNOTTY (('t' << 8) | 113)
#define TIOCSTOP  (('t' << 8) | 111)
#define TIOCSTART (('t' << 8) | 110)

#define TIOCGPGRP (('t' << 8) | 20)
#define TIOCSPGRP (('t' << 8) | 21)
#define TIOCGSID  (('t' << 8) | 22)
#define TIOCSTI   (('t' << 8) | 23)
#define TIOCSSID  (('t' << 8) | 24)

#define TIOCMSET  (('t' << 8) | 26)
#define TIOCMBIS  (('t' << 8) | 27)
#define TIOCMBIC  (('t' << 8) | 28)
#define TIOCMGET  (('t' << 8) | 29)

#define TIOCM_LE  0001
#define TIOCM_DTR 0002
#define TIOCM_RTS 0004
#define TIOCM_ST  0010
#define TIOCM_SR  0020
#define TIOCM_CTS 0040
#define TIOCM_CAR 0100
#define TIOCM_CD  TIOCM_CAR
#define TIOCM_RNG 0200
#define TIOCM_RI  TIOCM_RNG
#define TIOCM_DSR 0400

#define TIOCREMOTE (('t' << 8) | 30)
#define TIOCSIGNAL (('t' << 8) | 31)

#define LDOPEN   (('D' << 8) | 0)
#define LDCLOSE  (('D' << 8) | 1)
#define LDCHG    (('D' << 8) | 2)
#define LDGETT   (('D' << 8) | 8)
#define LDSETT   (('D' << 8) | 9)
#define LDSMAP   (('D' << 8) | 10)
#define LDGMAP   (('D' << 8) | 11)
#define LDNMAP   (('D' << 8) | 12)

#define DIOCGETP (('d' << 8) | 8)
#define DIOCSETP (('d' << 8) | 9)
#define FIORDCHK (('f' << 8) | 3)
#endif

#endif
