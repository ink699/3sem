#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <fstream>

using namespace std;

#define last 255
#define send 1
#define recieve 0

int main()
{
  char c;
  char path[] = "send.cpp";
  int id = 0 , len = 0, counter = 0;
  key_t key;
  string filename;
  cin >> filename;
  ifstream in(filename);
  struct mymsgbuf
  {
    long type;
    char text [4096];
  } mybuf;
  if((key = ftok(path, 0)) < 0)
  {
    exit(-1);
  }
  if((id = msgget(key, 0666 | IPC_CREAT)) < 0)
  {
    exit (-1);
  }
  while(in.get(c))
  {
    mybuf.type = send;
    (mybuf.text)[counter] = c;
    ++counter;
    if(counter == 4096)
    {
      if (msgsnd(id, &mybuf, 4096, 0) < 0)
      {
        cout << "probsnd";
        msgctl(id, IPC_RMID, NULL);
        exit(-1);
      }
      counter = 0;
    }
  }
  if(counter != 0)
  {
    if (msgsnd(id, &mybuf, counter, 0) < 0)
    {
      cout << "probsnd";
      msgctl(id, IPC_RMID, NULL);
      exit(-1);
    }
  }
  mybuf.type = last;
  (mybuf.text)[0] = '0';
  if (msgsnd(id, &mybuf, 1, 0) < 0)
  {
    cout << "probsnd";
    msgctl(id, IPC_RMID, NULL);
    exit(-1);
  }
  return 0;
}
