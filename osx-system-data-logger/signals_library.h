#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>

#if defined(__cplusplus)
extern "C" {
#endif

int setHandler(void (*f)(int), int sigNo);
void sigchldHandler(int sig);
inline void waitForAllChildren();
int sendSignal(pid_t process, int signo);
int setHandlerRt( void (*f)(int,siginfo_t *,void *), int sigNo);

#if defined(__cplusplus)
}
#endif