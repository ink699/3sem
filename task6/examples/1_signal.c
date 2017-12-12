#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void signal_callback_handler(int signum) 
{
   printf("Caught signal %d\n", signum);
   // Terminate program normally
   exit(signum);
}

int main()
{
   // Register signal and signal handler
   signal(SIGINT, signal_callback_handler);

   while(1) {
      sleep(1);
   }
   return EXIT_SUCCESS;
}
