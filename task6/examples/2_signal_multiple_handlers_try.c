#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

//multiple signal handler
void sig_handler(int signo)
{
      if (signo == SIGINT)
      printf("received SIGINT 1\n"); 
}
void sig(int signo)
{
     if (signo == SIGINT)
        printf("received SIGINT 2\n");
}

int main(void)
{
      if(signal(SIGINT, sig_handler) == SIG_ERR)
          printf("\ncan't catch SIGINT\n");

      if (signal(SIGINT, sig) == SIG_ERR)
          printf("\ncan't catch SIGINT\n");
  // A long long wait so that we can easily issue a signal to this process
     while(1) 
       sleep(1);
     return 0;
}
