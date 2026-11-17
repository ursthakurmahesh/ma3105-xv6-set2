Name: Mahesh Thakur
Roll Number: 2401MC58
Set: Set 2
Lab: Operating System Lab Examination (MA3105), 17 November 2026
Base source: unmodified xv6-public (x86) - the same copy provided in the exam.

Build and run (each question folder has a complete, independent xv6 tree):
  cd Q1/xv6 && make qemu-nox CPUS=1   (then run: prottest)
  cd Q2/xv6 && make qemu-nox CPUS=1   (then run: schedtest)

Question 1: Page Protection with mprotect() and munprotect()
- Added a helper setpgwrite() in vm.c (declared in defs.h) that sets or
  clears PTE_W on the page-table entries of a page range; it lives in vm.c
  because walkpgdir() is static there, and it fails if a page is not present.
- Implemented sys_mprotect()/sys_munprotect() in sysproc.c: they reject an
  unaligned addr, len <= 0, and any range ending above myproc()->sz, then
  call the helper and flush the TLB with lcr3(V2P(myproc()->pgdir)).
- Wired the two calls through syscall.h, syscall.c, user.h and usys.S, and
  added prottest.c (registered in the Makefile UPROGS list).
- prottest.c follows the required sequence: read after mprotect, a child
  killed on write, parent still sees A, write of C after munprotect, and the
  three -1 error cases. See Q1/Q1_output.txt.

Question 2: Priority-Based CPU Scheduler
- Added an int priority field to struct proc (proc.h); allocproc() in proc.c
  initializes every new process, including fork() children, to priority 10.
- Added set_priority(int pid, int priority) in proc.c: it searches the
  process table under ptable.lock, stores the new priority and returns the
  old one; sys_set_priority() in sysproc.c rejects priorities outside 1-20
  and returns -1 when no such process exists.
- Rewrote scheduler() in proc.c: under ptable.lock it scans the table and
  picks the RUNNABLE process with the smallest priority number, ties broken
  by table order, then switches to it exactly as the original code did.
- schedtest.c sets the parent to priority 1, creates three children at
  priorities 5, 10 and 15, and each child runs a CPU-bound loop; they finish
  in priority order and the parent prints Done. It also checks that priority
  0, priority 21 and an unknown pid each return -1. See Q2/Q2_output.txt;
  the cleaned source is Q2/2401MC58_Set2_Q2.tar.gz.

How to build and test on your system (Linux, or Windows with WSL):
Prerequisites: gcc and make (GCC 11 and 12 both work; the trees include the
small fix GCC 12 needs), and QEMU (qemu-system-i386; on Ubuntu:
sudo apt install qemu-system-x86).

Question 1:
  1. Unzip and enter the Q1 tree:
       unzip 2401MC58_MaheshThakur.zip
       cd 2401MC58_MaheshThakur/Q1/xv6
  2. Build and boot: make qemu-nox CPUS=1
  3. At the xv6 $ prompt, run separately:
       echo 2401MC58
       prottest
     Expected: "after mprotect, read: A"; the child is killed on write
     ("trap 14 err 7 ... --kill proc" - this intentional fault is the pass);
     "parent still sees: A"; "after munprotect, write ok: C"; unaligned
     address, len = 0, range beyond memory and negative len each return -1;
     two-page mprotect and munprotect return 0 (reads xy, then writes zw).
  4. Quit QEMU: press Ctrl-a, release, then press x.

Question 2:
  1. From Q1/xv6: cd ../../Q2/xv6
  2. Build and boot: make qemu-nox CPUS=1
     (one CPU is required; the finish order is not defined on several)
  3. At the xv6 $ prompt: echo 2401MC58, then schedtest
     Expected: "priority 5 finished", then 10, then 15 (highest priority
     first); priority 0, priority 21 and unknown pid each return -1;
     then "Done".

Note: in the Q2 tree the stock usertests case "preempt" does not finish,
because the required rule gives equal priorities to the first process in
table order, so its forever-spinning test child never lets the parent run
again. schedtest is the assignment's own test and works. In Q1 the full
usertests suite passes; in Q2 every usertests test before preempt passes.
