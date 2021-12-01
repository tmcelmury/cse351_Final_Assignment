#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>

#define STACKSIZE 2048

ucontext_t GlobalBuffer; 

int i;

void insert (void)
{
  printf("Insert an item\n");
  i++;
  printf("Done inserting! i = %d\n", i);
  return;
}
void delete (void)
{
  printf("Delete an item\n");
  i--;
  printf("Done deleting! i = %d\n", i);
  return;
}

int
main(int argc, char **argv)
{
  ucontext_t buf1, buf2;
  char stack1[STACKSIZE], stack2[STACKSIZE];


  getcontext(&GlobalBuffer);
  getcontext(&buf1);
  getcontext(&buf2);
  // set a thread's context
  buf1.uc_stack.ss_sp = stack1;
  buf1.uc_stack.ss_size = STACKSIZE;
  buf1.uc_link = &GlobalBuffer;
  makecontext(&buf1, insert, 0);
  // set another thread's context
  buf2.uc_stack.ss_sp = stack2;
  buf2.uc_stack.ss_size = STACKSIZE;
  buf2.uc_link = &GlobalBuffer;
  makecontext(&buf2, delete, 0);
  printf("Main: Jumping to insert\n");
  swapcontext(&GlobalBuffer,&buf1);
  printf("Main: Jumping to delete\n");
  swapcontext(&GlobalBuffer,&buf2);
  printf("Main thread --- all done\n");
  exit(0);
}


