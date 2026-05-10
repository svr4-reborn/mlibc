#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>

int main(void) {
	int pipefd[2];
	assert(!pipe(pipefd));

	int duplicate = dup(pipefd[0]);
	assert(duplicate >= 0);
	assert(!fcntl(duplicate, F_SETFD, FD_CLOEXEC));
	assert(fcntl(duplicate, F_GETFD) & FD_CLOEXEC);

	assert(dup2(pipefd[1], duplicate) == duplicate);
	assert(!(fcntl(duplicate, F_GETFD) & FD_CLOEXEC));

	assert(dup3(pipefd[0], duplicate, O_CLOEXEC) == duplicate);
	assert(fcntl(duplicate, F_GETFD) & FD_CLOEXEC);

	assert(!fcntl(pipefd[0], F_SETFD, FD_CLOEXEC));
	assert(dup2(pipefd[0], pipefd[0]) == pipefd[0]);
	assert(fcntl(pipefd[0], F_GETFD) & FD_CLOEXEC);

	errno = 0;
	assert(dup3(pipefd[0], pipefd[0], O_CLOEXEC) == -1);
	assert(errno == EINVAL);

	assert(!close(duplicate));
	assert(!close(pipefd[0]));
	assert(!close(pipefd[1]));
	return 0;
}