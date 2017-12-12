#include <signal.h>
// signal function has at least 2 drawbacks (or features? ;):
// 1) non-blocking another signals receiving -> current handler interrupting
// 2) SIG_DFL setting after first signal handling
// Trying to construct signal handling more accurately using sigaction(...)

sigaction_t hdl(int num)
{
	return;
}

int main()
{
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = hdl;
	sigset_t   set; 
	sigemptyset(&set);                                                             
	sigaddset(&set, SIGUSR1); 
	sigaddset(&set, SIGUSR2);
	act.sa_mask = set;
	sigaction(SIGUSR1, &act, 0);
	sigaction(SIGUSR2, &act, 0);
	return 0;
}
