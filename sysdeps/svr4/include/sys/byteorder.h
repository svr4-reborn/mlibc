#ifndef _SYS_BYTEORDER_H
#define _SYS_BYTEORDER_H

/*
 * Old SVr4 provides this header, and some things (such as Xorg) actually handle old SVr4 properly.
 * Nowadays, the functions here (htonl, ntohl, htons, ntohs) are provided by the POSIX endian.h, but with different names.
 * To avoid breaking old SVr4 source compatability too hard (and since I'm far too lazy to patch things that properly handle old OSes lol),
 * we import the functions from endian.h and give them the old SVr4 names as defines.
 */

#include <endian.h>

#define htonl(x) htobe32(x)
#define ntohl(x) be32toh(x)
#define htons(x) htobe16(x)
#define ntohs(x) be16toh(x)

#endif