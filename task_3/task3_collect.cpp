#include <pthread.h>
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <math.h>
#include <time.h>
#include <fstream>
#include <string>
#include <sstream>
#include <cstring>
#include <unistd.h>

using namespace std;

char* my_to_string(int num)
{
  stringstream ss;
  ss << num;
  string s;
  ss >> s;
  char* c = new char[s.size() + 1];
  strcpy(c, s.c_str());
  ss.clear();
  return c;
}

int main()
{
  ofstream out("stat.txt");
  int stat_check, num_points, pid;
  int* retcode;
  cin >> stat_check;
  if(stat_check)
    cin >> num_points;
  key_t key = 0;
  int shmid = 0, ch = 0, semid = 0;
  pair <double, double>* shared;
  sembuf buf;
  long long clocks;
  key = ftok("task3_int.cpp", 1);

  if(key < 0)
  {
    cout << "Problem with key generation" << endl;
    exit(-1);
  }
  semid = semget(key, 1, 0666 | IPC_CREAT);

  if(semid < 0)
  {
    cout << "Problem with semget" << endl;
    exit(-1);
  }
  shmid = shmget(key, sizeof(pair <double, double>), 0666 | IPC_CREAT);

  if(shmid < 0)
  {
    cout << "Problem with shmget" << endl;
    exit(-1);
  }
  shared = (pair <double, double>*)shmat(shmid, NULL, 0);

  if(shared == (pair <double, double>*)-1)
  {
    cout << "Problem with shmat" << endl;
    exit(-1);
  }
  buf.sem_op = -1;
  buf.sem_flg = 0;
  buf.sem_num = 0;
  if(!stat_check)
  {
    ch = semop(semid, &buf, 1);
    if(ch < 0)
    {
      cout << "Problem with semop" << endl;
      exit(-1);
    }
    cout << "result: " << (*shared).first << endl << "time: " << (*shared).second << endl;
  }
  else
  {
    for(int i = 1; i <= 100; ++i)
    {
      clocks = time(NULL);
      pid = fork();
      if(pid != 0)
      {
        wait(retcode);
      }
      else
      {
        execlp("./task3_int.out", "stat\n", my_to_string(i), my_to_string(num_points), NULL);
      }
      clocks = time(NULL) - clocks;
      cout << clocks << endl;
      out << i << " " << (*shared).first << " " << clocks * 1.0 << endl;
      cout << i << " completed " << (*shared).first << endl;
    }
  }
  ch = shmdt(shared);
  if(ch < 0)
  {
    cout << "Problem with shmdt" << endl;
  }
  ch = shmctl(shmid, IPC_RMID, NULL);
  if(ch < 0)
  {
    cout << "Problem with shmctl" << endl;
  }
  if(!stat_check)
    ch = semctl(semid, 0,  IPC_RMID, 0);
  if(ch < 0)
  {
    cout << "Problem with semctl" << endl;
  }
  return 0;
}
