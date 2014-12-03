#include "signals_library.h"
#include "errors.h"

int setHandler (void (*f)(int), int sigNo)
{
	struct sigaction act;

	memset (&act, 0, sizeof (struct sigaction));
	act.sa_handler = f;
	if (-1 == sigaction(sigNo, &act, NULL)) 
		return -1;
	else
		return 0;
}

void sigchldHandler(int sig)
{
	pid_t pid;

	while (1)
	{
		pid = waitpid (0, NULL, WNOHANG);
		if (0 == pid) return;
		if (0 > pid) 
		{
			if (ECHILD == errno) 
				return;
			else
				FILE_ERR("waitpid");
		}
	}
}

int setHandlerRt( void (*f)(int,siginfo_t *,void *), int sigNo)
{
	struct sigaction act;
	memset(&act, 0, sizeof(struct sigaction));
	act.sa_sigaction = f;
	act.sa_flags=SA_SIGINFO;
	if (-1==sigaction(sigNo, &act, NULL))
	{
		fprintf(stderr, "Error occured in sigaction function\n Error = %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

inline void WaitForAllChildren()
{
	while (TEMP_FAILURE_RETRY (wait (NULL)) > 0);
}

int sendSignal(pid_t process, int signo)
{
	if(-1 == kill(process, signo))
	{
		fprintf(stderr, "Error occured when sending signal %d to process with id = %d\n", signo, process);
		fprintf(stderr, "Error = %s\n", strerror(errno));
		return -1;
	}
	return 0;
}
