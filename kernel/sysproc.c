#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"


uint64
sys_exit(void)
{
  int n;
  if(argint(0, &n) < 0)
    return -1;
  exit(n);
  return 0;  // not reached
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
  if(argaddr(0, &p) < 0)
    return -1;
  return wait(p);
}

uint64
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
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
// lab 2.1
uint64
sys_trace(void)
{
  int n;
  if(argint(0,&n)<0){//从用户空间获取第一个参数n，如果获取失败则返回-1
    return -1;
  }
  myproc()->mask=n;//myproc返回指向当前进程结构体的指针
  return 0;
}
//lab 2.2
uint64
sys_sysinfo(void)
{
  uint64 addr;
  //从用户空间获取第一个参数的地址
  if(argaddr(0, &addr)<0){
    return -1;
  }
  //获取系统信息
  struct sysinfo info;
  info.freemem = getfreemem();
  info.nproc = getnproc();
  //将内核空间数据复制到用户空间
  if(copyout(myproc()->pagetable, addr, 
            (char *)&info, sizeof(info)) < 0)
    return -1;

  return 0;
}