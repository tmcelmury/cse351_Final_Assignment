#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>

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
  char stack1[1024], stack2[1024];

  getcontext(&buf1);
  getcontext(&buf2);
  getcontext(&GlobalBuffer);

  // set "insert" thread's context below
  
  // set "delete" thread's context below
  printf("Main: Jumping to insert\n");
  // the line below causes infinite loop.
  setcontext(&GlobalBuffer); 

  // swap context of main and insert
  
  printf("Main: Jumping to delete\n");
  // swap context of insert and delete  

  printf("Main thread --- all done\n");
  exit(0);
}


