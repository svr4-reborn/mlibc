#ifndef _ABIBITS_KD_H
#define _ABIBITS_KD_H

#define KIOC ('K' << 8)

#define KDDISPTYPE (KIOC | 1)
#define KDMAPDISP (KIOC | 2)
#define KDUNMAPDISP (KIOC | 3)
#define KDGKBENT (KIOC | 4)
#define KDSKBENT (KIOC | 5)
#define KDGKBMODE (KIOC | 6)
#define KDSKBMODE (KIOC | 7)
#define KDMKTONE (KIOC | 8)
#define KDGETMODE (KIOC | 9)
#define KDSETMODE (KIOC | 10)
#define KDADDIO (KIOC | 11)
#define KDDELIO (KIOC | 12)
#define KDSBORDER (KIOC | 13)
#define KDQUEMODE (KIOC | 15)
#define KIOCDOSMODE (KIOC | 16)
#define KIOCNONDOSMODE (KIOC | 17)
#define KDDISPINFO (KIOC | 18)
#define KDGKBSTATE (KIOC | 19)
#define KDSETRAD (KIOC | 20)
#define KDSCROLL (KIOC | 21)

#define KDENABIO (KIOC | 60)
#define KDDISABIO (KIOC | 61)
#define KIOCINFO (KIOC | 62)
#define KIOCSOUND (KIOC | 63)
#define KDGKBTYPE (KIOC | 64)
#define KDGETLED (KIOC | 65)
#define KDSETLED (KIOC | 66)

#define KD_MONO 01
#define KD_HERCULES 02
#define KD_CGA 03
#define KD_EGA 04
#define KD_VGA 05
#define KD_VDC400 06
#define KD_VDC750 07
#define KD_VDC600 010

#define K_RAW 0x00
#define K_XLATE 0x01

#define KD_TEXT 0
#define KD_TEXT0 0
#define KD_TEXT1 2
#define KD_GRAPHICS 1

#endif /* _ABIBITS_KD_H */