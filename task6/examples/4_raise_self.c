#include <signal.h>
#include <stdio.h>

void handler(int sig) {
    printf("HANDLED\n");
}
 
void main() {
    //register the SIGINT handler
    signal(SIGINT, handler);
    printf("begin\n");
    raise(SIGINT);
    printf("end\n");
    printf("Note that: the execution wasn't interrupted: the handler is alternative\n");
    return;
}
