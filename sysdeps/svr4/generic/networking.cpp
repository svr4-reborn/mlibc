#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stropts.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/syscall.h>
#include <sys/un.h>
#include <unistd.h>

#include <abi-bits/errno.h>
#include <abi-bits/ioctls.h>
#include <bits/syscall.h>
#include <netinet/in.h>
#include <mlibc/all-sysdeps.hpp>
#include <mlibc/debug.hpp>

namespace {

// Keep the syscall glue local to this file so the socket implementation does
// not add more networking-specific machinery to generic/sysdeps.cpp.
template<typename T>
constexpr long syscall_arg(T value) {
	if constexpr(__is_pointer(T))
		return reinterpret_cast<long>(value);
	else
		return static_cast<long>(value);
}

uint64_t syscall_state(long number) {
	return __do_syscall0(number);
}

template<typename Arg0>
uint64_t syscall_state(long number, Arg0 arg0) {
	return __do_syscall1(number, syscall_arg(arg0));
}

template<typename Arg0, typename Arg1>
uint64_t syscall_state(long number, Arg0 arg0, Arg1 arg1) {
	return __do_syscall2(number, syscall_arg(arg0), syscall_arg(arg1));
}

template<typename Arg0, typename Arg1, typename Arg2>
uint64_t syscall_state(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2) {
	return __do_syscall3(number, syscall_arg(arg0), syscall_arg(arg1), syscall_arg(arg2));
}

template<typename Arg0, typename Arg1, typename Arg2, typename Arg3>
uint64_t syscall_state(long number, Arg0 arg0, Arg1 arg1, Arg2 arg2, Arg3 arg3) {
	return __do_syscall4(number, syscall_arg(arg0), syscall_arg(arg1), syscall_arg(arg2), syscall_arg(arg3));
}

constexpr long syscall_state_value(uint64_t state) {
	return static_cast<long>(static_cast<int32_t>(state));
}

constexpr bool syscall_state_carry(uint64_t state) {
	return static_cast<bool>(state >> 32);
}

struct syscall_ret {
	long value;
	bool failed;

	int error() const {
		if(!failed)
			return 0;
		return static_cast<int>(value);
	}

	template<typename Result>
	int store(Result *result) const {
		if(int e = error(); e)
			return e;
		*result = static_cast<Result>(value);
		return 0;
	}
};

constexpr bool syscall_should_restart(long number) {
	switch(number) {
		case SYS_read:
		case SYS_write:
		case SYS_open:
		case SYS_ioctl:
		case SYS_getmsg:
		case SYS_putmsg:
			return true;
		default:
			return false;
	}
}

template<typename... Args>
syscall_ret syscall_call(long number, Args... args) {
	while(true) {
		auto state = syscall_state(number, args...);
		auto ret = syscall_ret{syscall_state_value(state), syscall_state_carry(state)};
		if(ret.failed && ret.value == ERESTART) {
			if(syscall_should_restart(number))
				continue;
			ret.value = EINTR;
		}
		return ret;
	}
}

// These are the TPI primitives that sockmod expects to see on the STREAMS
// control side. They mirror uts/i386/sys/tihdr.h, but are kept private here
// since mlibc does not currently install public TPI headers.
constexpr int kTransportConnectRequest = 0;
constexpr int kTransportConnectResponse = 1;
constexpr int kTransportDataRequest = 3;
constexpr int kTransportBindRequest = 6;
constexpr int kTransportUnitdataRequest = 8;
constexpr int kTransportOptionManagementRequest = 9;
constexpr int kTransportConnectIndication = 11;
constexpr int kTransportConnectConfirmation = 12;
constexpr int kTransportDisconnectIndication = 13;
constexpr int kTransportErrorAck = 18;
constexpr int kTransportOkAck = 19;
constexpr int kTransportUnitdataIndication = 20;
constexpr int kTransportOptionManagementAck = 22;

constexpr int kRsHiPri = 0x01;

constexpr int kTiOptionManagement = ('T' << 8) | 141;
constexpr int kTiBind = ('T' << 8) | 142;
constexpr int kTiGetMyName = ('T' << 8) | 144;
constexpr int kTiGetPeerName = ('T' << 8) | 145;

// sockmod-private I_STR commands from uts/i386/sys/sockmod.h.
constexpr int kSocketModule = ('I' << 8);
constexpr int kSiGetUdata = kSocketModule | 101;
constexpr int kSiShutdown = kSocketModule | 102;
constexpr int kSiListen = kSocketModule | 103;

constexpr int kTNegotiate = 0x004;
constexpr int kTCheck = 0x008;
constexpr int kTCots = 01;
constexpr int kTCotsOrd = 02;
constexpr int kTClts = 03;
constexpr int kSocketStateBound = 0x080;

constexpr unsigned long kSiocAtmark = 0x40047307;

// TPI structures are defined in terms of long fields for historical ABI
// alignment. Keep the layouts byte-compatible with the kernel headers.
struct T_bind_req {
	long PRIM_type;
	long ADDR_length;
	long ADDR_offset;
	unsigned long CONIND_number;
};

struct T_conn_req {
	long PRIM_type;
	long DEST_length;
	long DEST_offset;
	long OPT_length;
	long OPT_offset;
};

struct T_conn_res {
	long PRIM_type;
	void *QUEUE_ptr;
	long OPT_length;
	long OPT_offset;
	long SEQ_number;
};

struct T_conn_ind {
	long PRIM_type;
	long SRC_length;
	long SRC_offset;
	long OPT_length;
	long OPT_offset;
	long SEQ_number;
};

struct T_discon_ind {
	long PRIM_type;
	long DISCON_reason;
	long SEQ_number;
};

struct T_error_ack {
	long PRIM_type;
	long ERROR_prim;
	long TLI_error;
	long UNIX_error;
};

struct netbuf {
	unsigned int maxlen;
	unsigned int len;
	char *buf;
};

struct T_ok_ack {
	long PRIM_type;
	long CORRECT_prim;
};

struct T_unitdata_req {
	long PRIM_type;
	long DEST_length;
	long DEST_offset;
	long OPT_length;
	long OPT_offset;
};

struct T_unitdata_ind {
	long PRIM_type;
	long SRC_length;
	long SRC_offset;
	long OPT_length;
	long OPT_offset;
};

struct T_optmgmt_req {
	long PRIM_type;
	long OPT_length;
	long OPT_offset;
	long MGMT_flags;
};

struct si_udata {
	int tidusize;
	int addrsize;
	int optsize;
	int etsdusize;
	int servtype;
	int so_state;
	int so_options;
};

struct bind_buffer {
	T_bind_req request;
	sockaddr_storage address;
};

struct connect_buffer {
	T_conn_req request;
	sockaddr_storage address;
};

struct unitdata_buffer {
	T_unitdata_req request;
	sockaddr_storage address;
};

// AF_UNIX rides over the local TLI transports. sockmod keeps a user-visible
// sockaddr_un, but the transport provider routes by this private address.
struct ux_dev {
	dev_t dev;
	ino_t ino;
};

union ux_extaddr_value {
	ux_dev tu_addr;
	int tp_addr;
};

struct ux_extaddr {
	size_t size;
	ux_extaddr_value addr;
};

struct unix_transport_address {
	sockaddr_un name;
	ux_extaddr extaddr;
};

struct unix_bind_buffer {
	T_bind_req request;
	unix_transport_address address;
};

struct unix_connect_buffer {
	T_conn_req request;
	unix_transport_address address;
};

struct unix_unitdata_buffer {
	T_unitdata_req request;
	ux_dev address;
};

struct listen_buffer {
	T_bind_req request;
	char address[sizeof(unix_transport_address)];
};

constexpr size_t kUnixPathOffset = offsetof(sockaddr_un, sun_path);
constexpr size_t kUnixPathCapacity = sizeof(((sockaddr_un *)0)->sun_path);

struct unix_path {
	bool named;
	char path[kUnixPathCapacity + 1];
};

struct socket_metadata {
	bool valid;
	int family;
	int type;
};

constexpr int kTrackedSocketLimit = 1024;
socket_metadata tracked_sockets[kTrackedSocketLimit];

void remember_socket(int fd, int family, int type) {
	if(fd >= 0 && fd < kTrackedSocketLimit)
		tracked_sockets[fd] = {true, family, type};
}

bool lookup_socket(int fd, int *family, int *type) {
	if(fd < 0 || fd >= kTrackedSocketLimit || !tracked_sockets[fd].valid)
		return false;
	if(family)
		*family = tracked_sockets[fd].family;
	if(type)
		*type = tracked_sockets[fd].type;
	return true;
}

// SVR4 socket options use struct opthdr followed by a long-aligned payload.
size_t option_length(size_t size) {
	return ((size + sizeof(long) - 1) / sizeof(long)) * sizeof(long);
}

char *option_value(struct opthdr *option) {
	return reinterpret_cast<char *>(option + 1);
}

int check_socket_family(int family) {
	if(family == AF_INET6)
		return EAFNOSUPPORT;
	if(family != AF_INET && family != AF_UNIX)
		return EAFNOSUPPORT;
	return 0;
}

int check_socket_address(const struct sockaddr *addr_ptr, socklen_t addr_length) {
	if(!addr_ptr)
		return 0;
	if(addr_length < static_cast<socklen_t>(sizeof(sa_family_t)))
		return EINVAL;
	if(addr_ptr->sa_family == AF_INET6)
		return EAFNOSUPPORT;
	if(addr_ptr->sa_family == AF_UNIX)
		return addr_length <= static_cast<socklen_t>(sizeof(sockaddr_un)) ? 0 : EINVAL;
	if(addr_ptr->sa_family != AF_INET)
		return EAFNOSUPPORT;
	return 0;
}

int socket_address_family(const struct sockaddr *addr_ptr, socklen_t addr_length, int *family) {
	if(!addr_ptr || addr_length < static_cast<socklen_t>(sizeof(sa_family_t)))
		return EINVAL;
	if(int e = check_socket_address(addr_ptr, addr_length); e)
		return e;
	*family = addr_ptr->sa_family;
	return 0;
}

int check_socket_option_layer(int layer) {
	if(layer == SOL_IPV6 || layer == IPPROTO_IPV6)
		return ENOPROTOOPT;
	return 0;
}

// I_STR wraps STREAMS module ioctls. sockmod uses it for TLI-style control
// operations such as bind, getsockname, listen, shutdown, and SOL_SOCKET opts.
int stream_ioctl(int fd, int command, void *buffer, int length, int *actual_length = nullptr) {
	strioctl ioctl_data{};
	ioctl_data.ic_cmd = command;
	ioctl_data.ic_timout = 0;
	ioctl_data.ic_len = length;
	ioctl_data.ic_dp = static_cast<char *>(buffer);
	if(int e = syscall_call(SYS_ioctl, fd, I_STR, &ioctl_data).error(); e)
		return e;
	if(actual_length)
		*actual_length = ioctl_data.ic_len;
	return 0;
}

int set_socket_flags(int fd, int flags) {
	if(flags & ~(SOCK_CLOEXEC | SOCK_NONBLOCK))
		return EINVAL;

	if(flags & SOCK_CLOEXEC) {
		if(int e = syscall_call(SYS_fcntl, fd, F_SETFD, FD_CLOEXEC).error(); e)
			return e;
	}

	if(flags & SOCK_NONBLOCK) {
		long current;
		if(int e = syscall_call(SYS_fcntl, fd, F_GETFL, 0).store(&current); e)
			return e;
		if(int e = syscall_call(SYS_fcntl, fd, F_SETFL, current | O_NONBLOCK).error(); e)
			return e;
	}

	return 0;
}

int get_socket_udata(int fd, si_udata *udata) {
	memset(udata, 0, sizeof(*udata));
	return stream_ioctl(fd, kSiGetUdata, udata, sizeof(*udata));
}

int socket_type_from_udata(const si_udata &udata, int *type) {
	switch(udata.servtype) {
		case kTClts:
			*type = SOCK_DGRAM;
			return 0;
		case kTCots:
			*type = SOCK_SEQPACKET;
			return 0;
		case kTCotsOrd:
			*type = SOCK_STREAM;
			return 0;
		default:
			return EPROTONOSUPPORT;
	}
}

int transport_path(int family, int type, int protocol, const char **path) {
	if(int e = check_socket_family(family); e)
		return e;

	// socket() opens the TPI transport directly through the generated clone nodes.
	// The old /dev/sock SIOCSOCKSYS multiplexor is not implemented by this kernel.
	if(family == AF_UNIX) {
		if(protocol)
			return EPROTONOSUPPORT;
		switch(type) {
			case SOCK_STREAM:
				*path = "/dev/ticotsord";
				return 0;
			case SOCK_DGRAM:
				*path = "/dev/ticlts";
				return 0;
			case SOCK_SEQPACKET:
				*path = "/dev/ticots";
				return 0;
			default:
				return EPROTOTYPE;
		}
	}

	switch(type) {
		case SOCK_STREAM:
			if(protocol && protocol != IPPROTO_TCP)
				return EPROTONOSUPPORT;
			*path = "/dev/tcp";
			return 0;
		case SOCK_DGRAM:
			if(protocol && protocol != IPPROTO_UDP)
				return EPROTONOSUPPORT;
			*path = "/dev/udp";
			return 0;
		case SOCK_RAW:
			if(protocol == IPPROTO_ICMP) {
				*path = "/dev/icmp";
				return 0;
			}
			if(!protocol || protocol == IPPROTO_RAW || protocol == IPPROTO_IP) {
				*path = "/dev/rawip";
				return 0;
			}
			return EPROTONOSUPPORT;
		default:
			return EPROTOTYPE;
	}
}

int option_management(int fd, int management_flags, int layer, int number, void *buffer,
		socklen_t *size);

int set_socket_prototype(int fd, int protocol) {
	if(!protocol)
		return 0;
	int value = protocol;
	socklen_t size = sizeof(value);
	// sockmod tracks SO_PROTOTYPE itself, which lets raw sockets preserve the
	// requested protocol number after the transport stream has been opened.
	return option_management(fd, kTNegotiate, SOL_SOCKET, SO_PROTOTYPE, &value, &size);
}

int open_socket_stream(int family, int type, int protocol, int *fd) {
	const char *path;
	if(int e = transport_path(family, type, protocol, &path); e)
		return e;

	long opened_fd;
	if(int e = syscall_call(SYS_open, path, O_RDWR, 0).store(&opened_fd); e)
		return e;

	// Pushing sockmod is what turns the raw TPI stream into a BSD-like socket
	// endpoint; the module handles connection state, SOL_SOCKET options, and
	// saved peer/local addresses above the transport provider.
	if(int e = syscall_call(SYS_ioctl, opened_fd, I_PUSH, const_cast<char *>("sockmod")).error(); e) {
		(void)syscall_call(SYS_close, opened_fd);
		return e;
	}

	if(int e = set_socket_prototype(opened_fd, protocol); e) {
		(void)syscall_call(SYS_close, opened_fd);
		return e;
	}

	*fd = static_cast<int>(opened_fd);
	return 0;
}

int copy_unix_path(const struct sockaddr *addr_ptr, socklen_t addr_length, unix_path *path) {
	memset(path, 0, sizeof(*path));
	if(addr_length <= static_cast<socklen_t>(kUnixPathOffset))
		return 0;

	auto unix_addr = reinterpret_cast<const sockaddr_un *>(addr_ptr);
	size_t available = addr_length - kUnixPathOffset;
	if(available > kUnixPathCapacity)
		available = kUnixPathCapacity;
	if(!available || !unix_addr->sun_path[0])
		return 0;

	size_t path_length = available;
	if(auto terminator = static_cast<const char *>(memchr(unix_addr->sun_path, '\0', available)))
		path_length = terminator - unix_addr->sun_path;
	if(!path_length)
		return 0;

	memcpy(path->path, unix_addr->sun_path, path_length);
	path->path[path_length] = '\0';
	path->named = true;
	return 0;
}

void copy_unix_name(const struct sockaddr *addr_ptr, socklen_t addr_length,
		sockaddr_un *name) {
	memset(name, 0, sizeof(*name));
	name->sun_family = AF_UNIX;
	if(addr_ptr) {
		socklen_t to_copy = addr_length < static_cast<socklen_t>(sizeof(*name))
			? addr_length : sizeof(*name);
		memcpy(name, addr_ptr, to_copy);
		name->sun_family = AF_UNIX;
	}
}

int stat_unix_path(const char *path, ux_dev *device) {
	struct stat statbuf;
	if(int e = syscall_call(SYS_xstat, _STAT_VER, path, &statbuf).error(); e)
		return e;
	device->dev = statbuf.st_dev;
	device->ino = statbuf.st_ino;
	return 0;
}

int create_unix_placeholder(const char *path, bool *created) {
	*created = false;
	long fd;
	int e = syscall_call(SYS_open, path, O_RDWR | O_CREAT | O_EXCL, 0666).store(&fd);
	if(e)
		return e == EEXIST ? EADDRINUSE : e;
	*created = true;
	if(int close_error = syscall_call(SYS_close, fd).error(); close_error) {
		(void)syscall_call(SYS_unlink, path);
		*created = false;
		return close_error;
	}
	return 0;
}

int prepare_unix_transport_address(const struct sockaddr *addr_ptr, socklen_t addr_length,
		bool create_path, unix_transport_address *address, unix_path *path, bool *created) {
	if(created)
		*created = false;
	int family;
	if(int e = socket_address_family(addr_ptr, addr_length, &family); e)
		return e;
	if(family != AF_UNIX)
		return EAFNOSUPPORT;

	copy_unix_name(addr_ptr, addr_length, &address->name);
	memset(&address->extaddr, 0, sizeof(address->extaddr));
	if(int e = copy_unix_path(addr_ptr, addr_length, path); e)
		return e;
	if(!path->named)
		return 0;

	bool made_path = false;
	if(create_path) {
		if(int e = create_unix_placeholder(path->path, &made_path); e)
			return e;
		if(created)
			*created = made_path;
	}

	ux_dev device;
	if(int e = stat_unix_path(path->path, &device); e) {
		if(made_path)
			(void)syscall_call(SYS_unlink, path->path);
		if(created)
			*created = false;
		return e;
	}
	address->extaddr.size = sizeof(device);
	address->extaddr.addr.tu_addr = device;
	return 0;
}

int prepare_unix_peer_address(const struct sockaddr *addr_ptr, socklen_t addr_length,
		unix_transport_address *address) {
	unix_path path;
	bool created;
	if(int e = prepare_unix_transport_address(addr_ptr, addr_length, false, address, &path, &created); e)
		return e;
	return path.named ? 0 : EINVAL;
}

int bind_unix_transport(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length,
		unsigned long backlog) {
	unix_bind_buffer buffer{};
	buffer.request.PRIM_type = kTransportBindRequest;
	buffer.request.CONIND_number = backlog;

	bool created = false;
	unix_path path;
	int length;
	if(addr_ptr) {
		buffer.request.ADDR_length = sizeof(buffer.address);
		buffer.request.ADDR_offset = offsetof(unix_bind_buffer, address);
		if(int e = prepare_unix_transport_address(addr_ptr, addr_length, true,
				&buffer.address, &path, &created); e)
			return e;
		length = sizeof(T_bind_req) + buffer.request.ADDR_length;
	} else {
		buffer.request.ADDR_length = sizeof(buffer.address);
		buffer.request.ADDR_offset = offsetof(unix_bind_buffer, address);
		buffer.address.name.sun_family = AF_UNIX;
		length = sizeof(T_bind_req) + buffer.request.ADDR_length;
	}

	int e = stream_ioctl(fd, kTiBind, &buffer, length);
	if(e && created)
		(void)syscall_call(SYS_unlink, path.path);
	return e;
}

int bind_transport(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length, unsigned long backlog) {
	if(int e = check_socket_address(addr_ptr, addr_length); e)
		return e;
	if(addr_length > static_cast<socklen_t>(sizeof(sockaddr_storage)))
		return EINVAL;
	if(addr_ptr && addr_ptr->sa_family == AF_UNIX)
		return bind_unix_transport(fd, addr_ptr, addr_length, backlog);

	bind_buffer buffer{};
	buffer.request.PRIM_type = kTransportBindRequest;
	buffer.request.CONIND_number = backlog;
	if(addr_ptr && addr_length) {
		// TI_BIND carries a T_BIND_REQ followed by the sockaddr payload at ADDR_offset.
		buffer.request.ADDR_length = addr_length;
		buffer.request.ADDR_offset = offsetof(bind_buffer, address);
		memcpy(&buffer.address, addr_ptr, addr_length);
	}

	int length = sizeof(T_bind_req) + buffer.request.ADDR_length;
	return stream_ioctl(fd, kTiBind, &buffer, length);
}

int get_name(int fd, int command, struct sockaddr *addr_ptr, socklen_t max_addr_length,
		socklen_t *actual_length) {
	if(!addr_ptr || !actual_length)
		return EINVAL;
	if(max_addr_length > static_cast<socklen_t>(sizeof(sockaddr_storage)))
		max_addr_length = sizeof(sockaddr_storage);

	sockaddr_storage storage{};
	netbuf name{};
	name.maxlen = max_addr_length;
	name.buf = reinterpret_cast<char *>(&storage);
	if(int e = syscall_call(SYS_ioctl, fd, command, &name).error(); e)
		return e;

	socklen_t reported = static_cast<socklen_t>(name.len);
	socklen_t to_copy = reported < max_addr_length ? reported : max_addr_length;
	memcpy(addr_ptr, &storage, to_copy);
	*actual_length = reported;
	return 0;
}

int auto_bind(int fd) {
	sockaddr_storage storage{};
	socklen_t length = sizeof(storage);
	if(!get_name(fd, kTiGetMyName, reinterpret_cast<sockaddr *>(&storage), sizeof(storage), &length))
		return 0;
	// TCP connect expects the stream to be bound first. A zero-length bind asks
	// the transport to choose the local address/port.
	return bind_transport(fd, nullptr, 0, 0);
}

int ensure_unix_bound(int fd) {
	sockaddr_storage storage{};
	socklen_t length = sizeof(storage);
	if(!get_name(fd, kTiGetMyName, reinterpret_cast<sockaddr *>(&storage), sizeof(storage), &length)
			&& length >= static_cast<socklen_t>(sizeof(sa_family_t))
			&& reinterpret_cast<sockaddr *>(&storage)->sa_family == AF_UNIX)
		return 0;
	return bind_unix_transport(fd, nullptr, 0, 0);
}

int auto_bind(int fd, int family) {
	if(family == AF_UNIX)
		return ensure_unix_bound(fd);
	return auto_bind(fd);
}

int stream_is_nonblock(int fd, bool *nonblock) {
	long flags;
	if(int e = syscall_call(SYS_fcntl, fd, F_GETFL, 0).store(&flags); e)
		return e;
	*nonblock = flags & O_NONBLOCK;
	return 0;
}

int tpi_error(const T_error_ack *ack) {
	if(ack->UNIX_error)
		return ack->UNIX_error;
	if(ack->TLI_error)
		return EPROTO;
	return EIO;
}

// Connection setup is asynchronous at the TPI layer: after T_CONN_REQ, the
// provider answers with a control message rather than a syscall return value.
int wait_for_connect(int fd) {
	for(;;) {
		char control[sizeof(T_conn_ind) + sizeof(sockaddr_storage)] = {};
		strbuf ctlbuf{static_cast<int>(sizeof(control)), 0, control};
		strbuf databuf{-1, 0, nullptr};
		int flags = 0;
		if(int e = syscall_call(SYS_getmsg, fd, &ctlbuf, &databuf, &flags).error(); e)
			return e;
		if(ctlbuf.len < static_cast<int>(sizeof(long)))
			return EPROTO;

		long primitive = *reinterpret_cast<long *>(control);
		switch(primitive) {
			case kTransportConnectConfirmation:
				return 0;
			case kTransportOkAck:
				continue;
			case kTransportErrorAck:
				if(ctlbuf.len < static_cast<int>(sizeof(T_error_ack)))
					return EPROTO;
				return tpi_error(reinterpret_cast<T_error_ack *>(control));
			case kTransportDisconnectIndication:
				if(ctlbuf.len < static_cast<int>(sizeof(T_discon_ind)))
					return ECONNREFUSED;
				return reinterpret_cast<T_discon_ind *>(control)->DISCON_reason
					? reinterpret_cast<T_discon_ind *>(control)->DISCON_reason : ECONNREFUSED;
			default:
				return EPROTO;
		}
	}
}

int connect_transport(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	if(int e = check_socket_address(addr_ptr, addr_length); e)
		return e;
	if(!addr_ptr || addr_length == 0 || addr_length > static_cast<socklen_t>(sizeof(sockaddr_storage)))
		return EINVAL;

	int family;
	if(int e = socket_address_family(addr_ptr, addr_length, &family); e)
		return e;
	if(int e = auto_bind(fd, family); e)
		return e;

	if(family == AF_UNIX) {
		unix_connect_buffer buffer{};
		unix_transport_address address{};
		if(int e = prepare_unix_peer_address(addr_ptr, addr_length, &address); e)
			return e;
		buffer.request.PRIM_type = kTransportConnectRequest;
		buffer.request.DEST_length = sizeof(buffer.address);
		buffer.request.DEST_offset = offsetof(unix_connect_buffer, address);
		buffer.address = address;

		bool nonblock = false;
		if(int e = stream_is_nonblock(fd, &nonblock); e)
			return e;
		if(nonblock)
			buffer.request.OPT_length = -1;

		strbuf ctlbuf{static_cast<int>(sizeof(T_conn_req) + sizeof(buffer.address)), 0,
			reinterpret_cast<char *>(&buffer)};
		ctlbuf.len = static_cast<int>(sizeof(T_conn_req) + sizeof(buffer.address));
		strbuf databuf{-1, 0, nullptr};
		if(int e = syscall_call(SYS_putmsg, fd, &ctlbuf, &databuf, 0).error(); e)
			return e;
		if(nonblock)
			return EINPROGRESS;
		return wait_for_connect(fd);
	}

	connect_buffer buffer{};
	buffer.request.PRIM_type = kTransportConnectRequest;
	buffer.request.DEST_length = addr_length;
	buffer.request.DEST_offset = offsetof(connect_buffer, address);
	memcpy(&buffer.address, addr_ptr, addr_length);

	bool nonblock = false;
	if(int e = stream_is_nonblock(fd, &nonblock); e)
		return e;
	if(nonblock) {
		// sockmod treats OPT_length == -1 on T_CONN_REQ as the nonblocking connect
		// hint and leaves completion reporting to later stream state changes.
		buffer.request.OPT_length = -1;
	}

	strbuf ctlbuf{static_cast<int>(sizeof(T_conn_req) + addr_length), 0,
		reinterpret_cast<char *>(&buffer)};
	ctlbuf.len = static_cast<int>(sizeof(T_conn_req) + addr_length);
	strbuf databuf{-1, 0, nullptr};
	if(int e = syscall_call(SYS_putmsg, fd, &ctlbuf, &databuf, 0).error(); e)
		return e;
	if(nonblock)
		return EINPROGRESS;
	return wait_for_connect(fd);
}

int listen_transport(int fd, int backlog) {
	if(backlog < 0)
		backlog = 0;

	listen_buffer buffer{};
	buffer.request.PRIM_type = kTransportBindRequest;
	buffer.request.CONIND_number = static_cast<unsigned long>(backlog);

	// For an already-bound endpoint, sockmod's SI_LISTEN unbinds and rebinds to
	// update the connection queue length. It fills this buffer from its saved
	// local address; libc just has to provide enough room for either sockaddr_in
	// or the private AF_UNIX bind_ux payload.
	si_udata udata;
	if(!get_socket_udata(fd, &udata) && (udata.so_state & kSocketStateBound)) {
		buffer.request.ADDR_length = sizeof(buffer.address);
		buffer.request.ADDR_offset = offsetof(listen_buffer, address);
	}

	int ioctl_length = sizeof(T_bind_req) + buffer.request.ADDR_length;
	return stream_ioctl(fd, kSiListen, &buffer, ioctl_length);
}

void copy_sockaddr_out(const char *source, socklen_t source_length, struct sockaddr *addr_ptr,
		socklen_t *addr_length) {
	if(!addr_ptr || !addr_length)
		return;
	socklen_t max_length = *addr_length;
	socklen_t to_copy = source_length < max_length ? source_length : max_length;
	memcpy(addr_ptr, source, to_copy);
	*addr_length = source_length;
}

int accept_socket_info(int listener_fd, int *family, int *type) {
	si_udata udata;
	if(int e = get_socket_udata(listener_fd, &udata); e)
		return e;
	// SI_GETUDATA exposes the transport service type that sockmod learned from
	// T_INFO_ACK when it was pushed. Use it to create a matching accept endpoint.
	if(int e = socket_type_from_udata(udata, type); e)
		return e;
	if(*type != SOCK_STREAM && *type != SOCK_SEQPACKET)
		return EOPNOTSUPP;

	*family = AF_INET;
	sockaddr_storage storage{};
	socklen_t length = sizeof(storage);
	if(!get_name(listener_fd, kTiGetMyName, reinterpret_cast<sockaddr *>(&storage),
			sizeof(storage), &length) && length >= static_cast<socklen_t>(sizeof(sa_family_t)))
		*family = reinterpret_cast<sockaddr *>(&storage)->sa_family;
	return 0;
}

int open_accepted_socket(int family, int type, int flags, int *accepted_fd) {
	int fd;
	if(int e = open_socket_stream(family, type, 0, &fd); e)
		return e;
	// T_CONN_RES requires the accepting endpoint to be bound/idle. A zero-length
	// bind lets the provider choose its temporary transport address; sockmod
	// replaces the user-visible local/peer names when the response is processed.
	if(int e = auto_bind(fd, family); e) {
		(void)syscall_call(SYS_close, fd);
		return e;
	}
	if(int e = set_socket_flags(fd, flags); e) {
		(void)syscall_call(SYS_close, fd);
		return e;
	}
	*accepted_fd = fd;
	return 0;
}

int option_management(int fd, int management_flags, int layer, int number, void *buffer,
		socklen_t *size) {
	if(!size)
		return EINVAL;
	if(*size && !buffer)
		return EINVAL;

	size_t value_size = *size;
	size_t padded_value_size = option_length(value_size);
	size_t total_size = sizeof(T_optmgmt_req) + sizeof(opthdr) + padded_value_size;
	char *storage = static_cast<char *>(__builtin_alloca(total_size));
	memset(storage, 0, total_size);

	auto request = reinterpret_cast<T_optmgmt_req *>(storage);
	request->PRIM_type = kTransportOptionManagementRequest;
	request->OPT_length = sizeof(opthdr) + padded_value_size;
	request->OPT_offset = sizeof(T_optmgmt_req);
	request->MGMT_flags = management_flags;

	// sockmod handles SOL_SOCKET options itself when possible and forwards the
	// same T_OPTMGMT_REQ downstream for provider-specific options.
	auto option = reinterpret_cast<opthdr *>(storage + request->OPT_offset);
	option->level = layer;
	option->name = number;
	option->len = padded_value_size;
	if(management_flags == kTNegotiate && value_size)
		memcpy(option_value(option), buffer, value_size);

	int actual_length = total_size;
	if(int e = stream_ioctl(fd, kTiOptionManagement, storage, total_size, &actual_length); e)
		return e;
	if(actual_length < static_cast<int>(sizeof(T_optmgmt_req) + sizeof(opthdr)))
		return EPROTO;
	if(request->PRIM_type != kTransportOptionManagementAck)
		return EPROTO;

	if(management_flags == kTCheck) {
		socklen_t returned_size = static_cast<socklen_t>(option->len);
		socklen_t to_copy = returned_size < *size ? returned_size : *size;
		memcpy(buffer, option_value(option), to_copy);
		*size = returned_size;
	}
	return 0;
}

int send_unitdata(int fd, const void *buffer, size_t size, const struct sockaddr *sock_addr,
		socklen_t addr_length, ssize_t *length) {
	unitdata_buffer control{};
	// Unconnected datagram sends need a T_UNITDATA_REQ control part naming the
	// destination, plus the user payload as the data part of the STREAMS message.
	control.request.PRIM_type = kTransportUnitdataRequest;
	control.request.DEST_length = addr_length;
	control.request.DEST_offset = offsetof(unitdata_buffer, address);
	memcpy(&control.address, sock_addr, addr_length);

	strbuf ctlbuf{static_cast<int>(sizeof(T_unitdata_req) + addr_length),
		static_cast<int>(sizeof(T_unitdata_req) + addr_length), reinterpret_cast<char *>(&control)};
	strbuf databuf{static_cast<int>(size), static_cast<int>(size), const_cast<char *>(static_cast<const char *>(buffer))};
	if(int e = syscall_call(SYS_putmsg, fd, &ctlbuf, &databuf, 0).error(); e)
		return e;
	*length = size;
	return 0;
}

int send_unix_unitdata(int fd, const void *buffer, size_t size, const struct sockaddr *sock_addr,
		socklen_t addr_length, ssize_t *length) {
	if(int e = ensure_unix_bound(fd); e)
		return e;

	unix_transport_address peer{};
	if(int e = prepare_unix_peer_address(sock_addr, addr_length, &peer); e)
		return e;

	unix_unitdata_buffer control{};
	control.request.PRIM_type = kTransportUnitdataRequest;
	control.request.DEST_length = sizeof(control.address);
	control.request.DEST_offset = offsetof(unix_unitdata_buffer, address);
	control.address = peer.extaddr.addr.tu_addr;

	strbuf ctlbuf{static_cast<int>(sizeof(T_unitdata_req) + sizeof(control.address)),
		static_cast<int>(sizeof(T_unitdata_req) + sizeof(control.address)),
		reinterpret_cast<char *>(&control)};
	strbuf databuf{static_cast<int>(size), static_cast<int>(size),
		const_cast<char *>(static_cast<const char *>(buffer))};
	if(int e = syscall_call(SYS_putmsg, fd, &ctlbuf, &databuf, 0).error(); e)
		return e;
	*length = size;
	return 0;
}

// recvfrom() needs getmsg() so we can see T_UNITDATA_IND and recover the
// source sockaddr that a plain read() would discard.
int recv_with_name(int fd, void *buffer, size_t size, struct sockaddr *sock_addr,
		socklen_t *addr_length, ssize_t *length) {
	char control[sizeof(T_unitdata_ind) + sizeof(sockaddr_storage)] = {};
	strbuf ctlbuf{static_cast<int>(sizeof(control)), 0, control};
	strbuf databuf{static_cast<int>(size), 0, static_cast<char *>(buffer)};
	int flags = 0;
	if(int e = syscall_call(SYS_getmsg, fd, &ctlbuf, &databuf, &flags).error(); e)
		return e;

	if(ctlbuf.len >= static_cast<int>(sizeof(long))) {
		long primitive = *reinterpret_cast<long *>(control);
		if(primitive == kTransportUnitdataIndication) {
			if(ctlbuf.len < static_cast<int>(sizeof(T_unitdata_ind)))
				return EPROTO;
			auto indication = reinterpret_cast<T_unitdata_ind *>(control);
			if(indication->SRC_offset < 0 || indication->SRC_length < 0
					|| indication->SRC_offset + indication->SRC_length > ctlbuf.len)
				return EPROTO;
			copy_sockaddr_out(control + indication->SRC_offset,
				static_cast<socklen_t>(indication->SRC_length), sock_addr, addr_length);
		}
	}

	*length = databuf.len < 0 ? 0 : databuf.len;
	return 0;
}

int receive_peek_or_oob(int fd, void *buffer, size_t size, int flags, ssize_t *length) {
	int actual_length = size > static_cast<size_t>(INT_MAX) ? INT_MAX : static_cast<int>(size);
	// sockmod overloads a small I_STR command for the historical MSG_PEEK/MSG_OOB
	// receive path; normal receives use read() or getmsg().
	if(int e = stream_ioctl(fd, flags & (MSG_OOB | MSG_PEEK), buffer, actual_length, &actual_length); e)
		return e;
	*length = actual_length < 0 ? 0 : actual_length;
	return 0;
}

} // namespace

namespace mlibc {

int Sysdeps<Socket>::operator()(int family, int type, int protocol, int *fd) {
	int flags = type & (SOCK_CLOEXEC | SOCK_NONBLOCK);
	int kernel_type = type & ~(SOCK_CLOEXEC | SOCK_NONBLOCK);

	int result_fd;
	if(int e = open_socket_stream(family, kernel_type, protocol, &result_fd); e)
		return e;
	if(int e = set_socket_flags(result_fd, flags); e) {
		(void)syscall_call(SYS_close, result_fd);
		return e;
	}

	remember_socket(result_fd, family, kernel_type);
	*fd = result_fd;
	return 0;
}

int Sysdeps<Accept>::operator()(int fd, int *newfd, struct sockaddr *addr_ptr, socklen_t *addr_length, int flags) {
	int family = AF_UNSPEC;
	int type = 0;
	bool have_metadata = lookup_socket(fd, &family, &type);

	// accept() consumes a T_CONN_IND from the listener, then responds with
	// T_CONN_RES. I_FDINSERT lets the stream head fill in the accepted endpoint's
	// queue pointer at QUEUE_ptr before the response is sent downstream.
	char control[sizeof(T_conn_ind) + sizeof(sockaddr_storage)] = {};
	strbuf ctlbuf{static_cast<int>(sizeof(control)), 0, control};
	strbuf databuf{-1, 0, nullptr};
	int getmsg_flags = 0;
	if(int e = syscall_call(SYS_getmsg, fd, &ctlbuf, &databuf, &getmsg_flags).error(); e)
		return e;
	if(ctlbuf.len < static_cast<int>(sizeof(T_conn_ind)))
		return EPROTO;

	auto indication = reinterpret_cast<T_conn_ind *>(control);
	if(indication->PRIM_type != kTransportConnectIndication)
		return EPROTO;
	if(indication->SRC_offset < 0 || indication->SRC_length < 0
			|| indication->SRC_offset + indication->SRC_length > ctlbuf.len)
		return EPROTO;
	if(!have_metadata) {
		if(indication->SRC_length >= static_cast<long>(sizeof(sa_family_t)))
			family = reinterpret_cast<sockaddr *>(control + indication->SRC_offset)->sa_family;
		else
			family = AF_INET;
		// Duplicated listener fds do not have local metadata. In that fallback case,
		// both supported accept families use stream endpoints for the common path.
		type = SOCK_STREAM;
	}

	int accepted_fd;
	if(int e = open_accepted_socket(family, type, flags, &accepted_fd); e)
		return e;

	T_conn_res response{};
	response.PRIM_type = kTransportConnectResponse;
	response.SEQ_number = indication->SEQ_number;
	strbuf response_ctl{static_cast<int>(sizeof(response)), static_cast<int>(sizeof(response)),
		reinterpret_cast<char *>(&response)};
	strbuf response_data{-1, 0, nullptr};
	strfdinsert insert{};
	insert.ctlbuf = response_ctl;
	insert.databuf = response_data;
	insert.flags = 0;
	insert.fildes = accepted_fd;
	insert.offset = offsetof(T_conn_res, QUEUE_ptr);
	if(int e = syscall_call(SYS_ioctl, fd, I_FDINSERT, &insert).error(); e) {
		(void)syscall_call(SYS_close, accepted_fd);
		mlibc::infoLogger() << "mlibc: Accept(" << fd << ") I_FDINSERT error: " << e << frg::endlog;
		return e;
	}

	// accept() consumes a T_CONN_IND, sends T_CONN_RES, and then waits for the local
	// T_OK_ACK/T_ERROR_ACK response from the transport provider on the listener socket.
	// We temporarily disable non-blocking on the listener socket to block on this
	// acknowledgment, matching SVR4 libsocket's behavior.
	{
		long flags_val = 0;
		bool clear_nonblock = false;
		if(syscall_call(SYS_fcntl, fd, F_GETFL, 0).store(&flags_val) == 0) {
			if(flags_val & O_NONBLOCK) {
				clear_nonblock = true;
				(void)syscall_call(SYS_fcntl, fd, F_SETFL, flags_val & ~O_NONBLOCK);
			}
		}

		int err = 0;
		for(;;) {
			char control[sizeof(T_error_ack)] = {};
			strbuf ctlbuf{static_cast<int>(sizeof(control)), 0, control};
			strbuf databuf{-1, 0, nullptr};
			int getmsg_flags = kRsHiPri;
			if(int e = syscall_call(SYS_getmsg, fd, &ctlbuf, &databuf, &getmsg_flags).error(); e) {
				if(e == EINTR)
					continue;
				err = e;
				break;
			}

			if(ctlbuf.len < static_cast<int>(sizeof(long))) {
				err = EPROTO;
				break;
			}

			long primitive = *reinterpret_cast<long *>(control);
			if(primitive == kTransportOkAck) {
				auto ack = reinterpret_cast<T_ok_ack *>(control);
				if(ctlbuf.len < static_cast<int>(sizeof(T_ok_ack)) || ack->CORRECT_prim != kTransportConnectResponse)
					err = EPROTO;
				break;
			} else if(primitive == kTransportErrorAck) {
				auto ack = reinterpret_cast<T_error_ack *>(control);
				if(ctlbuf.len < static_cast<int>(sizeof(T_error_ack)) || ack->ERROR_prim != kTransportConnectResponse) {
					err = EPROTO;
				} else {
					err = tpi_error(ack);
				}
				break;
			} else {
				err = EPROTO;
				break;
			}
		}

		if(clear_nonblock) {
			(void)syscall_call(SYS_fcntl, fd, F_SETFL, flags_val);
		}

		if(err) {
			(void)syscall_call(SYS_close, accepted_fd);
			mlibc::infoLogger() << "mlibc: Accept(" << fd << ") TPI ack error: " << err << frg::endlog;
			return err;
		}
	}

	copy_sockaddr_out(control + indication->SRC_offset,
		static_cast<socklen_t>(indication->SRC_length), addr_ptr, addr_length);
	remember_socket(accepted_fd, family, type);
	*newfd = accepted_fd;
	mlibc::infoLogger() << "mlibc: Accept(" << fd << ") success accepted_fd=" << accepted_fd << frg::endlog;
	return 0;
}

int Sysdeps<Bind>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	mlibc::infoLogger() << "mlibc: Bind(" << fd << ") family=" << (addr_ptr ? addr_ptr->sa_family : -1) << frg::endlog;
	int e = bind_transport(fd, addr_ptr, addr_length, 0);
	mlibc::infoLogger() << "mlibc: Bind(" << fd << ") returned " << e << frg::endlog;
	return e;
}

int Sysdeps<Connect>::operator()(int fd, const struct sockaddr *addr_ptr, socklen_t addr_length) {
	mlibc::infoLogger() << "mlibc: Connect(" << fd << ") family=" << (addr_ptr ? addr_ptr->sa_family : -1) << frg::endlog;
	int e = connect_transport(fd, addr_ptr, addr_length);
	mlibc::infoLogger() << "mlibc: Connect(" << fd << ") returned " << e << frg::endlog;
	return e;
}

int Sysdeps<Sockname>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
		socklen_t *actual_length) {
	return get_name(fd, kTiGetMyName, addr_ptr, max_addr_length, actual_length);
}

int Sysdeps<Peername>::operator()(int fd, struct sockaddr *addr_ptr, socklen_t max_addr_length,
		socklen_t *actual_length) {
	return get_name(fd, kTiGetPeerName, addr_ptr, max_addr_length, actual_length);
}

int Sysdeps<GetSockopt>::operator()(int fd, int layer, int number, void *buffer, socklen_t *size) {
	if(int e = check_socket_option_layer(layer); e)
		return e;
	if(layer == SOL_SOCKET && number == SO_TYPE) {
		if(!size || !buffer || *size < static_cast<socklen_t>(sizeof(int)))
			return EINVAL;
		si_udata udata;
		if(int e = get_socket_udata(fd, &udata); e)
			return e;
		int type;
		if(int e = socket_type_from_udata(udata, &type); e)
			return e;
		memcpy(buffer, &type, sizeof(type));
		*size = sizeof(type);
		return 0;
	}
	return option_management(fd, kTCheck, layer, number, buffer, size);
}

int Sysdeps<SetSockopt>::operator()(int fd, int layer, int number, const void *buffer, socklen_t size) {
	if(int e = check_socket_option_layer(layer); e)
		return e;
	return option_management(fd, kTNegotiate, layer, number, const_cast<void *>(buffer), &size);
}

int Sysdeps<Listen>::operator()(int fd, int backlog) {
	return listen_transport(fd, backlog);
}

int Sysdeps<Recvfrom>::operator()(int fd, void *buffer, size_t size, int flags,
		struct sockaddr *sock_addr, socklen_t *addr_length, ssize_t *length) {
	if(flags & ~(MSG_PEEK | MSG_OOB))
		return EOPNOTSUPP;
	if(flags)
		return receive_peek_or_oob(fd, buffer, size, flags, length);
	if(sock_addr && addr_length)
		return recv_with_name(fd, buffer, size, sock_addr, addr_length, length);
	return syscall_call(SYS_read, fd, buffer, size).store(length);
}

int Sysdeps<Sendto>::operator()(int fd, const void *buffer, size_t size, int flags,
		const struct sockaddr *sock_addr, socklen_t addr_length, ssize_t *length) {
	if(int e = check_socket_address(sock_addr, addr_length); e)
		return e;
	if(flags & ~MSG_DONTROUTE)
		return EOPNOTSUPP;
	if(sock_addr) {
		if(addr_length > static_cast<socklen_t>(sizeof(sockaddr_storage)))
			return EINVAL;
		if(sock_addr->sa_family == AF_UNIX)
			return send_unix_unitdata(fd, buffer, size, sock_addr, addr_length, length);
		return send_unitdata(fd, buffer, size, sock_addr, addr_length, length);
	}
	return syscall_call(SYS_write, fd, buffer, size).store(length);
}

int Sysdeps<MsgRecv>::operator()(int fd, struct msghdr *msg, int flags, ssize_t *length) {
	int e = 0;
	if (flags & ~(MSG_PEEK | MSG_OOB)) {
		e = EOPNOTSUPP;
	} else if (flags) {
		if (msg->msg_iovlen == 1) {
			e = receive_peek_or_oob(fd, msg->msg_iov[0].iov_base, msg->msg_iov[0].iov_len, flags, length);
		} else {
			e = EOPNOTSUPP;
		}
	} else if (msg->msg_name && msg->msg_namelen) {
		if (msg->msg_iovlen == 1) {
			socklen_t addr_len = msg->msg_namelen;
			e = recv_with_name(fd, msg->msg_iov[0].iov_base, msg->msg_iov[0].iov_len,
				reinterpret_cast<struct sockaddr *>(msg->msg_name), &addr_len, length);
			msg->msg_namelen = addr_len;
		} else {
			msg->msg_namelen = 0;
			e = syscall_call(SYS_readv, fd, msg->msg_iov, msg->msg_iovlen).store(length);
		}
	} else {
		e = syscall_call(SYS_readv, fd, msg->msg_iov, msg->msg_iovlen).store(length);
	}
	return e;
}

int Sysdeps<MsgSend>::operator()(int fd, const struct msghdr *msg, int flags, ssize_t *length) {
	int e = 0;
	if (flags & ~MSG_DONTROUTE) {
		e = EOPNOTSUPP;
	} else if (msg->msg_name) {
		if (msg->msg_iovlen == 1) {
			e = Sysdeps<Sendto>()(fd, msg->msg_iov[0].iov_base, msg->msg_iov[0].iov_len, flags,
				reinterpret_cast<const struct sockaddr *>(msg->msg_name), msg->msg_namelen, length);
		} else {
			e = EOPNOTSUPP;
		}
	} else {
		e = syscall_call(SYS_writev, fd, msg->msg_iov, msg->msg_iovlen).store(length);
	}
	return e;
}

int Sysdeps<Shutdown>::operator()(int sockfd, int how) {
	return stream_ioctl(sockfd, kSiShutdown, &how, sizeof(how));
}

int Sysdeps<Sockatmark>::operator()(int sockfd, int *out) {
	return syscall_call(SYS_ioctl, sockfd, kSiocAtmark, out).error();
}

extern "C" {
	extern bool (*mlibc_is_socket_ptr)(int fd);
	extern void (*mlibc_clear_socket_ptr)(int fd);
}

static bool local_is_socket(int fd) {
	int family, type;
	return lookup_socket(fd, &family, &type);
}

static void local_clear_socket(int fd) {
	if(fd >= 0 && fd < kTrackedSocketLimit)
		tracked_sockets[fd].valid = false;
}

__attribute__((constructor))
static void init_is_socket_ptr() {
	mlibc_is_socket_ptr = local_is_socket;
	mlibc_clear_socket_ptr = local_clear_socket;
}

} // namespace mlibc
