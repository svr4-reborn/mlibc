#ifndef _ABIBITS_IN_H
#define _ABIBITS_IN_H

#include <bits/posix/in_addr_t.h>
#include <bits/posix/in_port_t.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct in_addr {
	union {
		struct {
			uint8_t s_b1;
			uint8_t s_b2;
			uint8_t s_b3;
			uint8_t s_b4;
		} S_un_b;
		struct {
			uint16_t s_w1;
			uint16_t s_w2;
		} S_un_w;
		in_addr_t S_addr;
	} S_un;
};

#define s_addr S_un.S_addr
#define s_host S_un.S_un_b.s_b2
#define s_net S_un.S_un_b.s_b1
#define s_imp S_un.S_un_w.s_w2
#define s_impno S_un.S_un_b.s_b4
#define s_lh S_un.S_un_b.s_b3

struct sockaddr_in {
	short sin_family;
	in_port_t sin_port;
	struct in_addr sin_addr;
	char sin_zero[8];
};

#ifdef __cplusplus
}
#endif

#define INADDR_ANY        ((in_addr_t) 0x00000000)
#define INADDR_BROADCAST  ((in_addr_t) 0xffffffff)
#define INADDR_NONE       ((in_addr_t) 0xffffffff)
#define INADDR_LOOPBACK   ((in_addr_t) 0x7f000001)

#define INET_ADDRSTRLEN  16

#define IPPORT_ECHO 7
#define IPPORT_DISCARD 9
#define IPPORT_SYSTAT 11
#define IPPORT_DAYTIME 13
#define IPPORT_NETSTAT 15
#define IPPORT_FTP 21
#define IPPORT_TELNET 23
#define IPPORT_SMTP 25
#define IPPORT_TIMESERVER 37
#define IPPORT_NAMESERVER 42
#define IPPORT_WHOIS 43
#define IPPORT_MTP 57
#define IPPORT_TFTP 69
#define IPPORT_RJE 77
#define IPPORT_FINGER 79
#define IPPORT_TTYLINK 87
#define IPPORT_SUPDUP 95
#define IPPORT_EXECSERVER 512
#define IPPORT_LOGINSERVER 513
#define IPPORT_CMDSERVER 514
#define IPPORT_EFSSERVER 520
#define IPPORT_BIFFUDP 512
#define IPPORT_WHOSERVER 513
#define IPPORT_ROUTESERVER 520
#define IPPORT_RESERVED 1024
#define IPPORT_USERRESERVED 5000

#define IPPROTO_IP       0
#define IPPROTO_ICMP     1
#define IPPROTO_IGMP     2
#define IPPROTO_GGP      3
#define IPPROTO_TCP      6
#define IPPROTO_EGP      8
#define IPPROTO_PUP      12
#define IPPROTO_UDP      17
#define IPPROTO_IDP      22
#define IPPROTO_HELLO    63
#define IPPROTO_ND       77
#define IPPROTO_RAW      255
#define IPPROTO_MAX      256

#define IN_LOOPBACKNET 127

#define IN_SET_LOOPBACK_ADDR(a) do { \
	(a)->sin_addr.s_addr = htonl(INADDR_LOOPBACK); \
	(a)->sin_family = AF_INET; \
} while(0)

#define IN_MINADDRLEN (sizeof(struct sockaddr_in) - 8)
#define IN_MAXADDRLEN (sizeof(struct sockaddr_in))
#define in_chkaddrlen(x) ((x) >= IN_MINADDRLEN && (x) <= IN_MAXADDRLEN)

#define IP_OPTIONS 1

#endif /* _ABITBITS_IN_H */
