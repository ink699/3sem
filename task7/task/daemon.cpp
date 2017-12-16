/*
Разработайте программу-демон, которая будет следить за содержимым/состоянием некоторой директории на файловой системе.
Варианты программы (выберите любой):

1) subrevision-daemon: демон, отслеживающий рекурсивно изменения любых файлов и директорий внутри нужной директории, и записывающий их в некоторый лог-файл. Например, если между двумя итерациями считывания содержимого данные в файле 1.txt изменились, вывести разность между ними в git-like виде (+ данные /-данные) или в diff-like (> ревизия n-1 / < ревизия n). Хранить и выводить можно только поледнюю ревизию.

2) inotify-daemon: демон, отслеживающий обращения к содержимому нужной директории через интерфейс inotify-подсистемы ядра Linux: получите inotify-файловый дескриптор и отслеживайте системные вызовы. Демон должен выводить данные в лог в формате время: вызов(некоторая информация: параметры, данные и т.д)

Обязательное условие: демон (на то он и демон) должен быть отвязан от любого терминала, устойчив к посылке сигналов, кроме явных SIGKILL и SIGSTOP, сохранять свой PID, не реагировать на нажатия комбинаций клавиш.
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
#include <ctime>
#include <sys/inotify.h>
#include <poll.h>
using namespace std;

ofstream dlog("log.txt");
void handle_events(int ifd)
{
    char buf[4096] __attribute__ ((aligned(__alignof__(struct inotify_event))));
    struct inotify_event *event;
    int i;
    ssize_t len;
    char *ptr;
    time_t sec;
    tm* lt;
    bool a_ch = true;

    for (;;)
    {
        len = read(ifd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN) {
            cout << "Error";
            exit(-1);
        }

         if (len <= 0)
             break;

         for (ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len)
         {
             event = (struct inotify_event *) ptr;
             if (event->mask & IN_ACCESS)
             {
                a_ch = false;
             }
             if (event->mask & IN_ATTRIB)
                dlog << "IN_ATTRIB: ";
             if (event->mask & IN_CLOSE_WRITE)
                dlog << "IN_CLOSE_WRITE: ";
             if (event->mask & IN_CLOSE_NOWRITE)
                dlog << "IN_CLOSE_NOWRITE: ";
             if (event->mask & IN_CREATE)
                dlog << "IN_CREATE: ";
             if (event->mask & IN_DELETE)
                dlog << "IN_DELETE: ";
             if (event->mask & IN_DELETE_SELF)
                dlog << "IN_DELETE_SELF: ";
             if (event->mask & IN_MODIFY)
                dlog << "IN_MODIFY: ";
             if (event->mask & IN_MOVE_SELF)
                dlog << "IN_MOVE_SELF: ";
             if (event->mask & IN_MOVED_FROM)
                dlog << "IN_MOVED_FROM: ";
             if (event->mask & IN_MOVED_TO)
                dlog << "IN_MOVED_TO: ";
             if (event->mask & IN_OPEN)
                dlog << "IN_OPEN: ";

             if (event->len && a_ch)
                 dlog << event->name;

            if(a_ch)
            {
             if (event->mask & IN_ISDIR)
                 dlog << " [directory]";
             else
                 dlog << " [file]";
            }

             sec = time(NULL);
             lt = localtime(&sec);
             if(a_ch)
              dlog << ", time: " << asctime(lt) << endl;
             a_ch = true;
         }
     }
 }

int main()
{
  string dir;
  cin >> dir;
  int pid = fork();
  if(pid != 0)
    exit(0);
  setsid();
  sigset_t set;
  sigfillset(&set);
  sigprocmask(SIG_SETMASK, &set, NULL);
  struct pollfd pfd[1];
  int poll_res = 0;

  dlog << "My pid is " << getpid() << endl << endl;
  int ifd = inotify_init();
  pfd[0].fd = ifd;
  pfd[0].events = POLLIN;
  int wd = inotify_add_watch(ifd, dir.c_str(), IN_ALL_EVENTS);

  while(1)
  {
    poll_res = poll(pfd, 1, -1);
    if (poll_res > 0)
      handle_events(ifd);
  }
}
