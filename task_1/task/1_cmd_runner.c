/*
  Write a program which allow you to:
- run another programs via command line.
- get exit codes of terminated programs

## TIPS:
1. Use "2_fork_wait_exit.c" and "4_exec_dir.c" from examples. Combine them.
2. Parse input string according to the type of exec* (see "man exec").
   a) if execvp is used, string splitting into "path" and "args" is all you need.
3. Collect exit codes via waitpid/WEXITSTATUS.
*/
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char* argv[])
{
  pid_t pid = 1;
  int ch = 1;
  int i = 0, j = 0;
  char c;
  int ret_code = 0;
  int args_num = 0;
  char ret = ' ';
  char adr[1000];
  char* name;
  char** arg;
  name = (char*)malloc(2097152*sizeof(char));
  arg = (char**)malloc(2097152*sizeof(char*));
  for(i = 0; i < 2097152; ++i)
  {
    arg[i] = (char*)malloc(1*sizeof(char));
  }
  char t;
  while(tolower(ret) != 'n')
  {
    switch(pid)
    {
      case 0:
        execvp(adr, arg);
        printf("Exec failed (not found)\n");
        return 0;
      default:
        if(arg[i + 1] == NULL)
            arg[i + 1] = (char*)malloc(1*sizeof(char));
        if(arg[i] == NULL)
            arg[i] = (char*)malloc(1*sizeof(char));
        i = 0;
        j = 0;
        printf("Enter address, enter quit for exit\n");
        scanf("%s", adr);
        if(strcmp(adr, "quit") == 0)
          return 0;
        args_num = argc;
        printf("Enter arguments without line breaks (including name)\n");
        c = getchar();
        c = getchar();
        if(c != '\n')
        {
          while(c != '\n')
          {
            if(c == ' ' && ch)
            {
              arg[i] = (char*)realloc(arg[i], (j+1)*sizeof(char));
              *(arg[i] + j) = '\0';
              ++i;
              j = 0;
              ch = 0;
            }
            if(c != ' ')
            {
              arg[i] = (char*)realloc(arg[i], (j+1)*sizeof(char));
              ch = 1;
              *(arg[i] + j) = c;
              ++j;
            }
            c = getchar();
          }
          arg[i] = (char*)realloc(arg[i], (j+1)*sizeof(char));
          *(arg[i] + j) = '\0';
          free(arg[i + 1]);
          arg[i + 1] = NULL;
        }
        else
        {
          free(arg[i]);
          arg[i] = NULL;
        }
        pid = fork();
        if(pid == -1)
        {
          printf("err");
          return 0;
        }
        if(pid != 0)
        {
          wait(&ret_code);
          printf("Return code: %d\n", WEXITSTATUS(ret_code));
        }
      }
  }
}
