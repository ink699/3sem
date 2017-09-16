/* Read the "4_exec_dir.c" carefully and correct the mistakes. Read about fork's return codes. */
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>

int main()
{
	pid_t pid;
  int status, died;
  switch(pid = fork())
	{
  	case 0:
			execl("/bin/ls","ls","-l",NULL); // this is the code the child runs
			break;
		case -1:
			printf("err");
			break;
  	default:
			wait(&status); // this is the code the parent runs
	}
		return 0;
}
