#ifndef _ABIBITS_WAIT_H
#define _ABIBITS_WAIT_H

#define WEXITED 0001
#define WTRAPPED 0002
#define WSTOPPED 0004
#define WCONTINUED 0010

#define WUNTRACED 0004
#define WNOHANG 0100
#define WNOWAIT 0200

#define WCONTFLG 0177777
#define WCOREFLG 0200
#define WWORD(stat) ((int)((stat)) & 0177777)
#define WSTOPFLG 0177
#define WSIGMASK 0177

#define WIFEXITED(stat) (((int)((stat) & 0377)) == 0)
#define WIFSIGNALED(stat) (((int)((stat) & 0377)) > 0 && ((int)(((stat) >> 8) & 0377)) == 0)
#define WIFSTOPPED(stat) (((int)((stat) & 0377)) == 0177 && ((int)(((stat) >> 8) & 0377)) != 0)
#define WIFCONTINUED(stat) (WWORD(stat) == WCONTFLG)

#define WEXITSTATUS(stat) ((int)(((stat) >> 8) & 0377))
#define WTERMSIG(stat) (((int)((stat) & 0377)) & 0177)
#define WSTOPSIG(stat) ((int)(((stat) >> 8) & 0377))

#define WCOREDUMP(stat) ((stat) & WCOREFLG)

#endif /*_ABIBITS_WAIT_H */
