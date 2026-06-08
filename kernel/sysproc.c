#include "types.h"
#include "riscv.h"
#include "param.h"
#include "defs.h"
#include "date.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

#define MAX_CHECK_PAGE_NUM 32
uint64
sys_exit(void)
{
  int n;
  if (argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  if (argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if (argint(0, &n) < 0)
    return -1;

  addr = myproc()->sz;
  if (growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if (argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n)
  {
    if (myproc()->killed)
    {
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

#ifdef LAB_PGTBL
int sys_pgaccess(void)
{
  // lab pgtbl: your code here.
  // 获取args
  uint64 userpage_vmaddr;
  int check_page_num;
  uint64 cache_useraddr;
  if (argaddr(0, &userpage_vmaddr) < 0)
    return -1;
  if (argint(1, &check_page_num) < 0)
    return -1;
  if (argaddr(2, &cache_useraddr) < 0)
    return -1;

  if (userpage_vmaddr >= MAXVA)
    panic("pgaccess");

  if (check_page_num > MAX_CHECK_PAGE_NUM)
    panic("pgaccess : too much page num");

  struct proc *p = myproc();
  pagetable_t pagetable = p->pagetable;

  uint32 res =0;

  for (int i = 0; i < check_page_num; i++)
  {
    //获取pte
    pte_t* pte = walk(pagetable, userpage_vmaddr, 0);
    //判断是否访问
    if(*pte & PTE_A ){
      //存储结果
      res |= (1U<<i);
      //清除pte_a
      *pte &= ~PTE_A;
    }
    //检查下一个页面
    userpage_vmaddr += PGSIZE;
    if (userpage_vmaddr >= MAXVA) break;
  }
  //结果复制到用户态
  copyout(pagetable,cache_useraddr,(char*)(&res),sizeof(res));

  return 0;
}
#endif

uint64
sys_kill(void)
{
  int pid;

  if (argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
