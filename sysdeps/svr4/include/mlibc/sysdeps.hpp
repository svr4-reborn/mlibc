#pragma once

#include <mlibc/sysdep-signatures.hpp>

namespace mlibc {

struct Svr4SysdepTags :
	Access,
	AnonAllocate,
	AnonFree,
	ClockGet,
	Close,
	Dup2,
	Exit,
	FutexTid,
	FutexWait,
	FutexWake,
	GetSockopt,
	Ioctl,
	Msgctl,
	Msgget,
	Msgrcv,
	Msgsnd,
	LibcLog,
	LibcPanic,
	Open,
	Peername,
	Read,
	Recvfrom,
	Semctl,
	Semget,
	Semop,
	SetSockopt,
	Seek,
	Shutdown,
	Socket,
	Sockatmark,
	Sockname,
	Shmat,
	Shmctl,
	Shmdt,
	Shmget,
	Stat,
	TcbSet,
	Bind,
	Connect,
	Listen,
	Accept,
	Sendto,
	VmMap,
	VmProtect,
	VmUnmap,
	Write
#ifdef __MLIBC_POSIX_OPTION
	,
	Fstatvfs,
	Isatty,
	Statvfs
#endif
{};

template<typename Tag>
using Sysdeps = SysdepOf<Svr4SysdepTags, Tag>;

} // namespace mlibc