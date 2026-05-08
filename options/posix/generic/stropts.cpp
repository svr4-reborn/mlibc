#include <errno.h>
#include <stropts.h>

#include <mlibc/all-sysdeps.hpp>

extern "C" int isastream(int fd) {
	if(int e = mlibc::sysdep_or_enosys<Isastream>(fd); e) {
		if(e == ENOSYS)
			return 0;
		errno = e;
		return -1;
	}
	return 1;
}
