#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include <sys/mman.h>
#include <vector>
#include <fcntl.h>
using namespace std;

vector <int> cr_files()
{
  char s[2];
  vector <int> res;
  int desc;
  for(int i = 10; i <= 25; ++i)
  {
    s[0] = (i / 10) + '0';
    s[1] = (i % 10) + '0';
    desc = open(s, O_RDWR | O_CREAT, 0666);
    if(desc == -1)
      exit(-1);
    res.push_back(desc);
    ftruncate(desc, 1 << i);
  }
  return res;
}

int main(int argc, const char* argv[])
{
  vector <int> exp = cr_files();
  int res_ds = open("res", O_RDWR | O_CREAT, 0666);
  ofstream res_out("res");
  char *start, *beg_res;
  for(int i = 10; i <= 25; ++i)
  {
    ftruncate(res_ds, 0);
    res_out << "This should disappear This should disappear This should disappear This should disappear This should disappear" << endl;;
    start = (char*)mmap(NULL, 1 << i, PROT_WRITE | PROT_READ, MAP_SHARED, exp[i - 10], 0);
    if (start == NULL)
    {
      cout << "Problem with mmap";
      exit(-1);
    }
    ftruncate(res_ds, 1 << i);
    beg_res = (char*)mmap(NULL, 1 << i, PROT_WRITE | PROT_READ, MAP_SHARED, res_ds, 0);
    if (start == NULL)
    {
      cout << "Problem with mmap";
      exit(-1);
    }
    memcpy(beg_res, start, 1 << i);
    munmap(start, 1 << i);
    munmap(beg_res, 1 << i);
  }
  for(int i = 10; i <= 25; ++i)
  {
    close(exp[i - 10]);
  }
  close(res_ds);
}
