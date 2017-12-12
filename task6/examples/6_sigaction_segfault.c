#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

//signal handler; don't use printf in signal handlers on production
void segfunc(int signal, siginfo_t *si, void *arg)
{
	printf("Segfault at %p\n", si->si_addr);
	exit(0);
}

int main()
{
	int *p = NULL;
	struct sigaction sa;

	memset(&sa, 0, sizeof(struct sigaction));
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = segfunc;

	sigaction(SIGSEGV, &sa, NULL);
	
	*p = 42;
	return 0;
}
