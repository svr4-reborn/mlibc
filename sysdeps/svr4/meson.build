sysdep_supported_options = {
	'glibc': true,
	'posix': true,
}

normalized_cpu_family = host_machine.cpu_family()
if normalized_cpu_family.startswith('i') and normalized_cpu_family.endswith('86')
	normalized_cpu_family = 'x86'
endif

if normalized_cpu_family != 'x86'
	error('sysdeps/svr4 currently only scaffolds the i386 SVR4 ABI')
endif

svr4_include_dirs = [
	include_directories('include'),
]

rtld_include_dirs += svr4_include_dirs
libc_include_dirs += svr4_include_dirs

rtld_sources += files(
	normalized_cpu_family / 'arch-syscall.S',
	normalized_cpu_family / 'signals.S',
	'generic/sysdeps.cpp',
)

libc_sources += files(
	normalized_cpu_family / 'arch-syscall.S',
	normalized_cpu_family / 'assembly-asserts.cpp',
	normalized_cpu_family / 'signals.S',
	'generic/entry.cpp',
	'generic/mount.cpp',
	'generic/mnttab.cpp',
	'generic/sysdeps.cpp',
)

if get_option('posix_option').allowed()
	libc_sources += files(
		normalized_cpu_family / 'cp_syscall.S',
		normalized_cpu_family / 'thread_entry.S',
	)
endif

if not no_headers
	install_headers(
		'include/abi-bits/access.h',
		'include/abi-bits/auxv.h',
		'include/abi-bits/blkcnt_t.h',
		'include/abi-bits/blksize_t.h',
		'include/abi-bits/clockid_t.h',
		'include/abi-bits/dev_t.h',
		'include/abi-bits/errno.h',
		'include/abi-bits/fcntl.h',
		'include/abi-bits/fd_set.h',
		'include/abi-bits/fsblkcnt_t.h',
		'include/abi-bits/fsfilcnt_t.h',
		'include/abi-bits/gid_t.h',
		'include/abi-bits/in.h',
		'include/abi-bits/ino_t.h',
		'include/abi-bits/ioctls.h',
		'include/abi-bits/ipc.h',
		'include/abi-bits/limits.h',
		'include/abi-bits/mode_t.h',
		'include/abi-bits/mqueue.h',
		'include/abi-bits/msg.h',
		'include/abi-bits/nlink_t.h',
		'include/abi-bits/pid_t.h',
		'include/abi-bits/poll.h',
		'include/abi-bits/resource.h',
		'include/abi-bits/rlim_t.h',
		'include/abi-bits/sa_family_t.h',
		'include/abi-bits/seek-whence.h',
		'include/abi-bits/sem.h',
		'include/abi-bits/shm.h',
		'include/abi-bits/sigevent.h',
		'include/abi-bits/sig-limits.h',
		'include/abi-bits/signal.h',
		'include/abi-bits/sigset_t.h',
		'include/abi-bits/sigval.h',
		'include/abi-bits/socket.h',
		'include/abi-bits/sockaddr_storage.h',
		'include/abi-bits/socklen_t.h',
		'include/abi-bits/stat.h',
		'include/abi-bits/statvfs.h',
		'include/abi-bits/suseconds_t.h',
		'include/abi-bits/termios.h',
		'include/abi-bits/time.h',
		'include/abi-bits/uid_t.h',
		'include/abi-bits/utmp-defines.h',
		'include/abi-bits/utmpx.h',
		'include/abi-bits/utsname.h',
		'include/abi-bits/vm-flags.h',
		'include/abi-bits/wait.h',
		subdir: 'abi-bits'
	)

	install_headers('include/syscall.h')
	install_headers('include/sys/mount.h', subdir: 'sys')
	install_headers('include/sys/mntent.h', subdir: 'sys')
	install_headers('include/sys/mnttab.h', subdir: 'sys')
	install_headers('include/sys/sysmacros.h', subdir: 'sys')
	install_headers('include/sys/syscall.h', subdir: 'sys')
	install_headers('include/bits/syscall.h', subdir: 'bits')
endif

if not headers_only
	custom_target('crt1',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: normalized_cpu_family / 'crt-src/crt1.S',
		output: 'crt1.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('Scrt1',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-fPIE', '-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: normalized_cpu_family / 'crt-src/Scrt1.S',
		output: 'Scrt1.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('crti',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: normalized_cpu_family / 'crt-src/crti.S',
		output: 'crti.o',
		install: true,
		install_dir: get_option('libdir')
	)

	custom_target('crtn',
		build_by_default: true,
		command: c_compiler.cmd_array() + ['-c', '-o', '@OUTPUT@', '@INPUT@'],
		input: normalized_cpu_family / 'crt-src/crtn.S',
		output: 'crtn.o',
		install: true,
		install_dir: get_option('libdir')
	)
endif