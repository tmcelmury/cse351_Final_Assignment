#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <string.h>

void reg_load(ucontext_t *);
void print_context(ucontext_t *);

int main(void)
{
	ucontext_t context0, context1;
	int i = 0, val = 0;
	getcontext(&context0);
	getcontext(&context1);
	reg_load(&context1);
	print_context(&context0);
	print_context(&context1);
	//swapcontext(&context1,&context0);
	swapcontext(&context0,&context1);
	print_context(&context1);

}	

void reg_load(ucontext_t * context)
{
	int i;
	context->uc_link = context;
	for (i = 0; i < 23; i++){
		context->uc_mcontext.gregs[i] = i;
	}
}

void print_context(ucontext_t * context)
{
	int i, val;
	for (i = 0; i < 23; i++){
		val = context->uc_mcontext.gregs[i];
		printf("reg[%d] = %p\n", i, val);
	}
}
