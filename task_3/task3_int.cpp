#include <pthread.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <math.h>
#include <time.h>

using namespace std;

double res = 0;
int num_threads = 0, dens = 0;

pair <double, double> get_ran_coord()
{
  pair <double, double> res;
  srand(clock());
  res.first = rand() * 1.0 / RAND_MAX;
  res.second = rand() * 3.0 / RAND_MAX;
  return res;
}

bool check_coords(pair <double, double> coords)
{
  if (coords.second <= coords.first * coords.first)
  {
    return true;
  }
  return false;
}

pthread_mutex_t mutex;

void* integrate(void* arg)
{
  pair <double, double> coords;
  int counter = 0;
  for(int i = 0; i < dens / num_threads; ++i)
  {
    coords = get_ran_coord();
    if(check_coords(coords))
    {
      ++counter;
    }
  }
  pthread_mutex_lock(&mutex);
  res += counter * 1.0 / dens;
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main()
{
  key_t key = 0;
  int shmid = 0, ch = 0, semid = 0;
  long long clocks = 0;
  pair <double, double>* shared;
  sembuf buf;
  cin >> num_threads >> dens;
  pthread_t* tid = new pthread_t[num_threads];
  ch = pthread_mutex_init(&mutex, NULL);

  if(ch != 0)
  {
    cout << "Problem with mutex init" << endl;
    exit(-1);
  }
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
  (*shared).first = 0;
  (*shared).second = 0;

  for(int i = 0; i < num_threads; ++i)
  {
    ch = pthread_create(tid + i, NULL, integrate, NULL);
    if(ch != 0)
    {
      cout << "Problem with thread creation" << endl;
      exit(-1);
    }
  }

  clocks = clock();
  for(int i = 0; i < num_threads; ++i)
  {
    pthread_join(tid[i], NULL);
  }
  clocks = clock() - clocks;
  cout << clocks * 1.0 / CLOCKS_PER_SEC << endl;

  (*shared).first = res * 3.0;
  (*shared).second = clocks * 1.0 / CLOCKS_PER_SEC;
  buf.sem_op = 1;
  buf.sem_flg = 0;
  buf.sem_num = 0;
  ch = semop(semid, &buf, 1);
  if(ch < 0)
  {
    cout << "Problem with semop" << endl;
    exit(-1);
  }
  ch = shmdt(shared);
  pthread_mutex_destroy(&mutex);
  if(ch < 0)
  {
    cout << "Problem with shdt" << endl;
  }
  return 0;
}
