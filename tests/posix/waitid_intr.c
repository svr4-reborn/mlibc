#include <assert.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

static void handle_alarm(int sig) {
	(void)sig;
}

int main(void) {
	struct sigaction sa = {0};
	sa.sa_handler = handle_alarm;
	assert(!sigemptyset(&sa.sa_mask));
	assert(!sigaction(SIGALRM, &sa, NULL));

	pid_t pid = fork();
	assert(pid >= 0);

	if(!pid) {
		pause();
		_Exit(0);
	}

	alarm(1);

	siginfo_t info = {0};
	errno = 0;
	assert(waitid(P_PID, pid, &info, WEXITED) == -1);
	assert(errno == EINTR);

	assert(!kill(pid, SIGKILL));
	assert(waitpid(pid, NULL, 0) == pid);
	return 0;
}