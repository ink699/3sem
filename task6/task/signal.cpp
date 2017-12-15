/*
Написать 2 программы, передающие файл между собой, с использованием только сигналов как методов синхронизации и передачи данных. Пользоваться pipe, msg*, shm*, sem* и иными средствами синхронизации нельзя!

Требования:
＊ Допустимо написание 2 программ, печатающих свои pid'ы и ожидающих ввода pid'а парной программы.
・ В противном случае предполагается запуск одной программы, которая при помощи вызова fork() создает дочернюю программу. Родитель занимается записью в новый файл данных, читаемых ребенком.
・ При получении сигнала SIGTERM необходимо написать о его получении перед закрытием программы.
・ При получении сигнала SIGCHLD необходимо написать о его получении перед закрытием программы.
Варианты реализации передачи (выберите любой):
1) Побитовая передача через SIGUSR1, SIGUSR2
2) Не-побитовая передача на основе real-time сигналов
*/

#include <signal.h>
#include <sys/types.h>
#include <cmath>
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
#include <algorithm>
using namespace std;

int counter_rc = 0, counter_snd = 8, par_pid, child_pid;
vector <int> ths(8);
char ths_symb, smb_to_snd;
ifstream in;
ofstream out;

void to_bits(char c)
{
  for(int i = 0; i < 8; ++i)
  {
    ths[i] = c % 2;
    c /= 2;
  }
  for(int i = 0; i < 4; ++i)
  {
    swap(ths[i], ths[7 - i]);
  }
}

void usr1_handl(int nsig) //one
{
  if(getpid() == par_pid) //parent
  {
    ths_symb += pow(2, 7 - counter_rc);
    ++counter_rc;
    if(counter_rc == 8)
    {
      counter_rc = 0;
      out << ths_symb;
      ths_symb = 0;
    }
    kill(child_pid, SIGUSR1);
  }
  else //child
  {
    if(counter_snd == 8)
    {
      smb_to_snd = in.get();
      to_bits(smb_to_snd);
      counter_snd = 0;
      if(in.eof())
        exit(0);
    }
    if(ths[counter_snd])
    {
      kill(par_pid, SIGUSR1);
      ++counter_snd;
    }
    else
    {
      kill(par_pid, SIGUSR2);
      ++counter_snd;
    }
  }
}

void usr2_handl(int nsig) //zero, only in parent
{
  ++counter_rc;
  if(counter_rc == 8)
  {
    counter_rc = 0;
    out << ths_symb;
    ths_symb = 0;
  }
  kill(child_pid, SIGUSR1);
}

void chld_handl(int nsig)
{
  cout << "SIGCHLD recieved" << endl;
  out << endl;
  exit(0);
}

void term_handl(int nsig)
{
  cout << "SIGTERM recieved" << endl;
  exit(0);
}

int main()
{
  struct sigaction act_usr1, act_usr2, act_term, act_chld;
  act_usr1.sa_handler = usr1_handl;
  act_usr2.sa_handler = usr2_handl;
  act_term.sa_handler = term_handl;
  act_chld.sa_handler = chld_handl;
  sigaction(SIGUSR1, &act_usr1, NULL);
  sigaction(SIGUSR2, &act_usr2, NULL);
  sigaction(SIGCHLD, &act_chld, NULL);
  sigaction(SIGTERM, &act_term, NULL);

  string input, output;
  cin >> input >> output;
  in.open(input);
  out.open(output);

  par_pid = getpid();
  child_pid = fork();
  if(child_pid != 0) //parent
  {
    kill(child_pid, SIGUSR1);
    while(1);
  }
  else
  {
    child_pid = getpid();
    while(1);
  }
}
