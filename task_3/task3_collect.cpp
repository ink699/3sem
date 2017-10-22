#include <pthread.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <math.h>
#include <time.h>

using namespace std;


int main()
{
  key_t key = 0;
  int shmid = 0, ch = 0, semid = 0;
  pair <double, double>* shared;
  sembuf buf;
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
  ch = semop(semid, &buf, 1);
  if(ch < 0)
  {
    cout << "Problem with semop" << endl;
    exit(-1);
  }
  cout << "result: " << (*shared).first << endl << "time: " << (*shared).second << endl;
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
  ch = semctl(semid, 0,  IPC_RMID, 0);
  if(ch < 0)
  {
    cout << "Problem with semctl" << endl;
  }
  return 0;
}
