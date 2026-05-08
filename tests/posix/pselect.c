#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

static void sig_handler(int sig) {
	(void)sig;
}

int main() {
	int fds[2];
	assert(pipe(fds) == 0);

	fd_set read_set;
	FD_ZERO(&read_set);
	FD_SET(fds[0], &read_set);

	struct timespec timeout;
	timeout.tv_sec = 0;
	timeout.tv_nsec = 1000000;

	int ret = pselect(fds[0] + 1, &read_set, NULL, NULL, &timeout, NULL);
	assert(ret == 0);
	assert(!FD_ISSET(fds[0], &read_set));

	assert(write(fds[1], "a", 1) == 1);
	FD_ZERO(&read_set);
	FD_SET(fds[0], &read_set);
	timeout.tv_sec = 1;
	timeout.tv_nsec = 0;

	ret = pselect(fds[0] + 1, &read_set, NULL, NULL, &timeout, NULL);
	assert(ret == 1);
	assert(FD_ISSET(fds[0], &read_set));

	char byte;
	assert(read(fds[0], &byte, 1) == 1);

	struct sigaction sa = {0};
	sa.sa_handler = sig_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	assert(sigaction(SIGUSR1, &sa, NULL) == 0);

	sigset_t block_mask;
	sigset_t original_mask;
	sigset_t wait_mask;
	sigset_t current_mask;
	sigemptyset(&block_mask);
	sigaddset(&block_mask, SIGUSR1);
	assert(sigprocmask(SIG_BLOCK, &block_mask, &original_mask) == 0);

	sigemptyset(&wait_mask);

	pid_t pid = fork();
	assert(pid >= 0);

	if(pid == 0) {
		usleep(100000);
		assert(kill(getppid(), SIGUSR1) == 0);
		_exit(0);
	}

	FD_ZERO(&read_set);
	FD_SET(fds[0], &read_set);
	timeout.tv_sec = 1;
	timeout.tv_nsec = 0;

	errno = 0;
	ret = pselect(fds[0] + 1, &read_set, NULL, NULL, &timeout, &wait_mask);
	assert(ret == -1);
	assert(errno == EINTR);

	assert(sigprocmask(SIG_SETMASK, NULL, &current_mask) == 0);
	assert(sigismember(&current_mask, SIGUSR1) == 1);

	int status;
	assert(waitpid(pid, &status, 0) == pid);
	assert(WIFEXITED(status) && WEXITSTATUS(status) == 0);

	assert(sigprocmask(SIG_SETMASK, &original_mask, NULL) == 0);

	close(fds[0]);
	close(fds[1]);
	return 0;
}