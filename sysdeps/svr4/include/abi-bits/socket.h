#ifndef _ABIBITS_SOCKET_H
#define _ABIBITS_SOCKET_H

#include <abi-bits/dev_t.h>
#include <bits/size_t.h>
#include <bits/posix/iovec.h>

#ifdef __cplusplus
extern "C" {
#endif

struct msghdr {
	void *msg_name;
	int msg_namelen;
	struct iovec *msg_iov;
	int msg_iovlen;
	void *msg_accrights;
	int msg_accrightslen;
};

struct sockproto {
	unsigned short sp_family;
	unsigned short sp_protocol;
};

struct opthdr {
	long level;
	long name;
	long len;
};

struct socksysreq {
	int args[7];
};

struct socknewproto {
	int family;
	int type;
	int proto;
	dev_t dev;
	int flags;
};

#ifdef __cplusplus
}
#endif

#define __MLIBC_ABI_HAS_CMSG 0
#define __MLIBC_ABI_HAS_MMSGHDR 0

#define SHUT_RD 0
#define SHUT_WR 1
#define SHUT_RDWR 2

#define SOCK_STREAM 2
#define SOCK_DGRAM 1
#define SOCK_RAW 4
#define SOCK_RDM 5
#define SOCK_SEQPACKET 6

#ifndef SOCK_CLOEXEC
#define SOCK_CLOEXEC 02000000
#define SOCK_NONBLOCK 04000
#endif

#define AF_UNSPEC 0
#define AF_UNIX 1
#define AF_INET 2
#define AF_IMPLINK 3
#define AF_PUP 4
#define AF_CHAOS 5
#define AF_NS 6
#define AF_NBS 7
#define AF_ECMA 8
#define AF_DATAKIT 9
#define AF_CCITT 10
#define AF_SNA 11
#define AF_DECnet 12
#define AF_DLI 13
#define AF_LAT 14
#define AF_HYLINK 15
#define AF_APPLETALK 16
#define AF_NIT 17
#define AF_802 18
#define AF_OSI 19
#define AF_X25 20
#define AF_OSINET 21
#define AF_GOSIP 22
#define AF_MAX 22

#define PF_UNSPEC AF_UNSPEC
#define PF_UNIX AF_UNIX
#define PF_INET AF_INET
#define PF_IMPLINK AF_IMPLINK
#define PF_PUP AF_PUP
#define PF_CHAOS AF_CHAOS
#define PF_NS AF_NS
#define PF_NBS AF_NBS
#define PF_ECMA AF_ECMA
#define PF_DATAKIT AF_DATAKIT
#define PF_CCITT AF_CCITT
#define PF_SNA AF_SNA
#define PF_DECnet AF_DECnet
#define PF_DLI AF_DLI
#define PF_LAT AF_LAT
#define PF_HYLINK AF_HYLINK
#define PF_APPLETALK AF_APPLETALK
#define PF_NIT AF_NIT
#define PF_802 AF_802
#define PF_OSI AF_OSI
#define PF_X25 AF_X25
#define PF_OSINET AF_OSINET
#define PF_GOSIP AF_GOSIP
#define PF_MAX AF_MAX

#define SO_DEBUG 0x0001
#define SO_ACCEPTCONN 0x0002
#define SO_REUSEADDR 0x0004
#define SO_KEEPALIVE 0x0008
#define SO_DONTROUTE 0x0010
#define SO_BROADCAST 0x0020
#define SO_USELOOPBACK 0x0040
#define SO_LINGER 0x0080
#define SO_OOBINLINE 0x0100
#define SO_ORDREL 0x0200
#define SO_IMASOCKET 0x0400
#define SO_DONTLINGER (~SO_LINGER)

#define SO_SNDBUF 0x1001
#define SO_RCVBUF 0x1002
#define SO_SNDLOWAT 0x1003
#define SO_RCVLOWAT 0x1004
#define SO_SNDTIMEO 0x1005
#define SO_RCVTIMEO 0x1006
#define SO_ERROR 0x1007
#define SO_TYPE 0x1008
#define SO_PROTOTYPE 0x1009

#define SOL_SOCKET 0xffff

#define OPTLEN(x) ((((x) + sizeof(long) - 1) / sizeof(long)) * sizeof(long))
#define OPTVAL(opt) ((char *)((opt) + 1))

#define SOMAXCONN 5

#define MSG_OOB 0x1
#define MSG_PEEK 0x2
#define MSG_DONTROUTE 0x4
#define MSG_MAXIOVLEN 16

#define SO_ACCEPT 1
#define SO_BIND 2
#define SO_CONNECT 3
#define SO_GETPEERNAME 4
#define SO_GETSOCKNAME 5
#define SO_GETSOCKOPT 6
#define SO_LISTEN 7
#define SO_RECV 8
#define SO_RECVFROM 9
#define SO_SEND 10
#define SO_SENDTO 11
#define SO_SETSOCKOPT 12
#define SO_SHUTDOWN 13
#define SO_SOCKET 14
#define SO_SOCKPOLL 15
#define SO_GETIPDOMAIN 16
#define SO_SETIPDOMAIN 17
#define SO_ADJTIME 18

#endif
