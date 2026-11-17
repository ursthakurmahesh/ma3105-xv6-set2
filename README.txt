Name: Mahesh Thakur
Roll Number: 2401MC58
Set: Set 2
Lab: Operating System Lab Examination (MA3105), 17 November 2026

Base: the unmodified xv6-public (x86) copy given in the exam. Each question
folder has its own complete xv6 tree so the two build and run independently.

Question 1 - mprotect() and munprotect()
setpgwrite() in vm.c (declared in defs.h) sets or clears PTE_W over a page
range; it lives in vm.c because walkpgdir() is static there, and fails on a
page that is not present. sys_mprotect()/sys_munprotect() in sysproc.c
reject an unaligned addr, len <= 0 and ranges past myproc()->sz, then call
the helper and reload cr3 to flush the TLB; syscall.h/c, user.h and usys.S
are only wiring, and prottest.c runs the required sequence plus two extras.

Question 2 - priority based scheduler
proc.h adds a priority field to struct proc and allocproc() defaults it to
10 so fork children start equal. set_priority() in proc.c stores the new
value under ptable.lock and returns the old one; out-of-range values and
unknown pids get -1. scheduler() now picks the RUNNABLE process with the
smallest priority number (ties in table order) and switches as before.
schedtest.c runs spinning children at 5, 10 and 15 that finish in order,
and checks 0, 21 and a bad pid each return -1.

Building and testing it yourself (Linux, or WSL on Windows):
You need gcc, make and qemu-system-i386 (on Ubuntu: sudo apt install
qemu-system-x86). GCC 11 and 12 both work; the trees already carry the
small fix GCC 12 needs.

Q1:
  unzip 2401MC58_MaheshThakur.zip
  cd 2401MC58_MaheshThakur/Q1/xv6
  make qemu-nox CPUS=1
  then at the xv6 $ prompt, one at a time:
      echo 2401MC58
      prottest
  You should get "after mprotect, read: A", then the child killed on its
  write (the "trap 14 err 7 ... --kill proc" line is the pass, not a
  crash), "parent still sees: A", "after munprotect, write ok: C", the bad
  calls returning -1 each, and the two-page case working (reads xy, writes
  zw). Ctrl-a then x quits QEMU.

Q2:
  cd ../../Q2/xv6
  make qemu-nox CPUS=1
  echo 2401MC58, then schedtest
  You should see priority 5, 10 and 15 finish in that order, 0, 21 and an
  unknown pid return -1, and finally "Done". CPUS=1 matters here - on
  several CPUs the finish order is not defined.

Evidence: Q1/Q1_output.txt and Q2/Q2_output.txt are saved console logs of
the runs above (roll number echoed first), and Q1/Q1_screenshot.jpg and
Q2/Q2_screenshot.jpg are screenshots of the same tests running on my own
machine. The cleaned sources are Q1/2401MC58_Set2_Q1.tar.gz and
Q2/2401MC58_Set2_Q2.tar.gz.

One honest note: in the Q2 tree the stock usertests case "preempt" never
finishes, because the required tie-break rule lets an equal-priority child
that spins forever starve its parent. schedtest is the assignment's own
test and passes. In Q1 the full usertests suite passes, and in Q2 every
test before preempt passes.
