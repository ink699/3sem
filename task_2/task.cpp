#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fcntl.h>
#include <string>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sstream>
#include <cmath>
using namespace std;

string conv_lg(string s)
{
  if (s == "")
    return "0";
  int lg = s.size();
  int sz = log10(lg);
  ++sz;
  stringstream ss;
  ss << sz;
  ss << lg;
  string res;
  ss >> res;
  ss.clear();
  return res;
}

int to_int(char* c)
{
  string s(c);
  stringstream ss;
  int res = 0;
  ss << s;
  ss >> res;
  ss.clear();
  return res;
}

class dpipe//Class for string exchange
{
private:
  int txd[2];//parent - r, child - w
  int rxd[2];//parent - w, child - r
  int ppid = 0;
public:

  dpipe()
  {
    ppid = getpid();
  }

  void create()//should be applied before fork()
  {
    if(pipe(txd) < 0)
    {
      cout << "Impposible to create a pipe" << endl;
      exit(-1);
    }
    if(pipe(rxd) < 0)
    {
      cout << "Impposible to create a pipe" << endl;
      exit(-1);
    }
  }

  void close_d() //if something goes wrong
  {
    if(getpid() == ppid)
    {
      close(txd[1]);
      close(rxd[0]);
    }
    else
    {
      close(txd[0]);
      close(rxd[1]);
    }
  }

  void init()//should be applied after fork()
  {
    if(getpid() == ppid)
    {
      close(txd[1]);
      close(rxd[0]);
    }
    else
    {
      close(txd[0]);
      close(rxd[1]);
    }
  }

  string child_from_parent()//should be used only in the child
  {
    if (getpid() != ppid)
    {
      char lg_sz[1];
      int size = read(rxd[0], lg_sz, 1);
      if(size < 0)
      {
        cout << "Problem with reading from parent" << endl;
        close_d();
        exit(-1);
      }
      int l_sz = lg_sz[0] - '0';
      if(l_sz == 0)
      {
        return "";
      }
      char* sz = new char[l_sz];
      size = read(rxd[0], sz, l_sz);
      if(size < 0)
      {
        cout << "Problem with reading from parent" << endl;
        close_d();
        exit(-1);
      }
      int str_sz = to_int(sz);
      char* str = new char[str_sz + 1];
      size = read(rxd[0], str, str_sz + 1);
      if(size < 0)
      {
        cout << "Problem with reading from parent" << endl;
        close_d();
        exit(-1);
      }
      string res(str);
      res.resize(size - 1);
      delete[] sz;
      delete[] str;
      return res;
    }
    cout << "Sike, that's the wrong process!" << endl;
    close_d();
    exit(-1);
  }

  string parent_from_child()//should be used only in the parent
  {
    if (getpid() == ppid)
    {
      char lg_sz[1];
      int size = read(txd[0], lg_sz, 1);
      if(size < 0)
      {
        cout << "Problem with reading from child" << endl;
        close_d();
        exit(-1);
      }
      int l_sz = lg_sz[0] - '0';
      if(l_sz == 0)
      {
        return "";
      }
      char* sz = new char[l_sz];
      size = read(txd[0], sz, l_sz);
      if(size < 0)
      {
        cout << "Problem with reading from child" << endl;
        close_d();
        exit(-1);
      }
      int str_sz = to_int(sz);
      char* str = new char[str_sz + 1];
      size = read(txd[0], str, str_sz + 1);
      if(size < 0)
      {
        cout << "Problem with reading from child" << endl;
        close_d();
        exit(-1);
      }
      string res(str);
      res.resize(size - 1);
      delete[] sz;
      delete[] str;
      return res;
    }
    cout << "Sike, that's the wrong process!" << endl;
    close_d();
    exit(-1);
  }

  void parent_write(string s)//should be used only in the parent
  {
    if(getpid() == ppid)
    {
      if(s == "")
      {
        int size = write(rxd[1], "0", 1);
        if(size < 0)
        {
          cout << "Problem with writing in parent";
          close_d();
          exit(-1);
        }
        return;
      }
      s = conv_lg(s) + s;//We need this to transmit the string length from the parent to the child
      char* c = new char[s.size() + 1];
      copy(s.begin(), s.end(), c);
      c[s.size()] = '\n';
      int size = write(rxd[1], c, s.size() + 1);
      if(size < 0)
      {
        cout << "Problem with writing in parent";
        close_d();
        exit(-1);
      }
      delete[] c;
      return;
    }
    cout << "Sike, that's the wrong process!" << endl;
    close_d();
    exit(-1);
  }

  void child_write(string s)//should be used only in the child
  {
    if(getpid() != ppid)
    {
      if(s == "")
      {
        int size = write(txd[1], "0", 1);
        if(size < 0)
        {
          cout << "Problem with writing in child";
          close_d();
          exit(-1);
        }
        return;
      }
      s = conv_lg(s) + s;//We need this to transmit the string length from the child to the parent
      char* c = new char[s.size() + 1];
      copy(s.begin(), s.end(), c);
      c[s.size()] = '\n';
      int size = write(txd[1], c, s.size() + 1);;
      if(size < 0)
      {
        cout << "Problem with writing in child";
        close_d();
        exit(-1);
      }
      delete[] c;
      return;
    }
    cout << "Sike, that's the wrong process!" << endl;
    close_d();
    exit(-1);
  }

};

int main()
{
  string r = "asd";
  dpipe dp;
  int pid = 0;
  dp.create();
  pid = fork();
  string s = "";
  if(pid < 0)
  {
    dp.close_d();
    exit(-1);
  }
  dp.init();
  for(;;)
  {
    if(pid)
    {
      getline(cin, s);
      dp.parent_write(s);
      s = dp.parent_from_child();
      cout << s << endl;
    }
    else
    {
      s = dp.child_from_parent();
      dp.child_write(s);
    }
  }
  return 0;
}
