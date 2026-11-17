#include "types.h"
#include "stat.h"
#include "user.h"

static void
work(int priority)
{
  int i, j;
  volatile int x = 0;

  // Long CPU-bound loop: lasts several timer ticks.
  for(i = 0; i < 20; i++)
    for(j = 0; j < 1000000; j++)
      x += j;
  printf(1, "priority %d finished\n", priority);
  if(x == -1)
    printf(1, "x\n");
}

int
main(void)
{
  int pr[3] = {5, 10, 15};
  int i, pid, old;

  // Parent takes the highest priority so it can create all children
  // and set their priorities before any of them gets the CPU.
  old = set_priority(getpid(), 1);
  printf(1, "parent pid %d set to priority 1 (was %d)\n", getpid(), old);

  for(i = 0; i < 3; i++){
    pid = fork();
    if(pid == 0){
      work(pr[i]);
      exit();
    }
    old = set_priority(pid, pr[i]);
    printf(1, "child pid %d set to priority %d (was %d)\n", pid, pr[i], old);
  }

  for(i = 0; i < 3; i++)
    wait();
  printf(1, "set priority 0 -> %d (want -1)\n", set_priority(getpid(), 0));
  printf(1, "set priority 21 -> %d (want -1)\n", set_priority(getpid(), 21));
  printf(1, "set unknown pid 9999 -> %d (want -1)\n", set_priority(9999, 5));
  printf(1, "Done\n");
  exit();
}
