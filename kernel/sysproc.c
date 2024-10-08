#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64
sys_exit(void) {
  int n;
  argint(0, &n);
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void) {
  return myproc()->pid;
}

uint64
sys_fork(void) {
  return fork();
}

uint64
sys_wait(void) {
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void) {
  uint64 addr;
  int    n;

  argint(0, &n);
  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void) {
  int  n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

#ifdef LAB_PGTBL
int sys_pgaccess(void) {
  // lab pgtbl: your code here.
  uint64 vaddr;
  int    num;
  uint64 res_addr;
  argaddr(0, &vaddr);
  argint(1, &num);
  argaddr(2, &res_addr);

  struct proc *p         = myproc();
  pagetable_t  pagetable = p->pagetable;
  uint64       res       = 0;

  for (int i = 0; i < num; i++) {
    pte_t *pte = walk(pagetable, vaddr + PGSIZE * i, 0);
    if (*pte & PTE_A) {
      *pte &= (~PTE_A);
      res |= (1L << i);
    }
  }

  copyout(pagetable, res_addr, (char *)&res, sizeof(uint64));

  return 0;
}
#endif

uint64
sys_kill(void) {
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void) {
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_trace(void) {
  // get the trace mask from proc
  uint mask;
  argint(0, (int *)&mask);
  struct proc *p = myproc();
  p->trace_mask  = mask;
  return 0;
}

uint64
sys_sysinfo(void) {
  struct sysinfo info;
  uint64         addr;
  argaddr(0, &addr);
  info.nproc   = numproc();
  info.freemem = freemem();
  if (copyout(myproc()->pagetable, addr, (char *)&info, sizeof(info)) < 0) {
    return -1;
  }
  return 0;
}
