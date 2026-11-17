#include "types.h"
#include "stat.h"
#include "user.h"

int
main(void)
{
  char *p;
  int pid;

  // The first sbrk() in a xv6 program returns a page-aligned address.
  p = sbrk(4096);
  if(p == (char*)-1){
    printf(1, "prottest: sbrk failed\n");
    exit();
  }
  p[0] = 'A';
  printf(1, "stored A at %x\n", (uint)p);

  // (i) protect the page, then show it can still be read
  if(mprotect(p, 1) < 0){
    printf(1, "prottest: mprotect failed\n");
    exit();
  }
  printf(1, "after mprotect, read: %c\n", p[0]);

  // (ii) child tries to write to the read-only page and must be killed
  pid = fork();
  if(pid == 0){
    printf(1, "child writing to protected page...\n");
    p[0] = 'B';
    printf(1, "child survived write (BUG)\n");
    exit();
  }
  wait();
  printf(1, "parent still sees: %c\n", p[0]);

  // (iii) unprotect, write must succeed now
  if(munprotect(p, 1) < 0){
    printf(1, "prottest: munprotect failed\n");
    exit();
  }
  p[0] = 'C';
  printf(1, "after munprotect, write ok: %c\n", p[0]);

  // (iv) error cases must all return -1
  printf(1, "unaligned addr -> %d (want -1)\n", mprotect(p + 1, 1));
  printf(1, "len = 0 -> %d (want -1)\n", mprotect(p, 0));
  printf(1, "range beyond memory -> %d (want -1)\n", mprotect(p, 1000000));

  // extra checks: negative length, and a two-page range
  printf(1, "negative len -> %d (want -1)\n", mprotect(p, -3));
  {
    char *q = sbrk(2*4096);
    q[0] = 'x';
    q[4096] = 'y';
    printf(1, "two-page mprotect -> %d (want 0)\n", mprotect(q, 2));
    printf(1, "two-page read ok: %c%c\n", q[0], q[4096]);
    printf(1, "two-page munprotect -> %d (want 0)\n", munprotect(q, 2));
    q[0] = 'z';
    q[4096] = 'w';
    printf(1, "two-page write ok: %c%c\n", q[0], q[4096]);
  }

  printf(1, "prottest done\n");
  exit();
}
