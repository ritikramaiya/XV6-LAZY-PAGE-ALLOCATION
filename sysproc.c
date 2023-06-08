#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"

extern int free_frame_cnt; // OS project: memory management
int lazypaging; // variable defined for lazy

int sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}
 

int sys_sbrk(void)
{
  int addr; int n; uint sz;

  argint(0, &n);

  addr = myproc()->sz;

  if(lazypaging == 1) {
  	if((myproc()->sz + n) >= 0xfffffff) {
    	cprintf("page Allocation failed !\n");
        return -1;
    }

  	myproc()->oldsz = myproc()->sz;
  	myproc()->sz = myproc()->sz + n;
    if(n < 0) {
    	if((sz = deallocuvm(myproc()->pgdir, myproc()->oldsz, myproc()->sz)) == 0)
    	{
      		cprintf("page Deallocation failed!\n"); // CS550: for mem proj
      		return -1;
    	}
    }
  } else {
  	if(growproc(n) < 0)
    	return -1;
  }
  return addr;
}

int
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

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
int sys_shutdown(void)
{
  /* Either of the following will work. Does not harm to put them together. */
  outw(0xB004, 0x0|0x2000); // working for old qemu
  outw(0x604, 0x0|0x2000); // working for newer qemu

  return 0;
  
}

int sys_print_free_frame_cnt(void)
{
    cprintf("free-frames %d\n", free_frame_cnt);
    return 0;
}

int sys_set_page_allocator(void) {
	int lazyindication;
	argint(0, &lazyindication);
	lazypaging = lazyindication;
	return 0;
}