#include <pthread.h>
#include <iostream>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <math.h>
#include <time.h>
#include <fstream>
#include <stdlib.h>

using namespace std;

double res = 0;
int num_threads = 0, dens = 0, num_points = 0;
double gl_time = 0;

pthread_mutex_t mutex;

void* integrate(void* arg)
{
  int tm = clock();
  double l_tm = 0;
  double x, y;
  int counter = 0;
  unsigned int seed;
  for(int i = 0; i < dens; ++i)
  {
    tm = clock();
    seed = clock();
    x = rand_r(&seed) * 1.0 / RAND_MAX;
    y = rand_r(&seed) * 1.0 / RAND_MAX;
    if(y <= x*x)
    {
      ++counter;
    }
    l_tm += (clock() - tm) * 1.0 / CLOCKS_PER_SEC;
  }
  pthread_mutex_lock(&mutex);
  if(gl_time < l_tm)
  gl_time = l_tm;
  res += counter * 1.0 / num_points;
  pthread_mutex_unlock(&mutex);
  return NULL;
}

int main()
{
  ofstream out("stat.txt");
  int ch = 0;
  long long clocks = 0;
  sembuf buf;
  cin >> num_points;

  ch = pthread_mutex_init(&mutex, NULL);

  if(ch != 0)
  {
    cout << "Problem with mutex init" << endl;
    exit(-1);
  }

  pair <double, double>* shared = new pair <double, double>;
  int tm = clock();
  for(num_threads = 1; num_threads < 21; ++num_threads)
  {
    gl_time = 0;
    dens = num_points / num_threads;
    pthread_t* tid = new pthread_t[num_threads];

    (*shared).first = 0;
    (*shared).second = 0;
    res = 0;


    for(int i = 0; i < num_threads; ++i)
    {
      ch = pthread_create(tid + i, NULL, integrate, NULL);
      if(ch != 0)
      {
        cout << "Problem with thread creation" << endl;
        exit(-1);
      }
    }
    for(int i = 0; i < num_threads; ++i)
    {
      pthread_join(tid[i], NULL);
    }

    (*shared).first = res * 1.0;
    (*shared).second = clocks * 1.0 / CLOCKS_PER_SEC;

    out << (*shared).first << " " << (*shared).second << endl;
    delete[] tid;
    cout << gl_time << endl;
  }
  cout << (clock() - tm) * 1.0 / CLOCKS_PER_SEC << endl;
  pthread_mutex_destroy(&mutex);
  return 0;
}
