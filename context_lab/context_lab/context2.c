#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <string.h>


void proc1(char*);
void proc2(char*);
ucontext_t GlobalContext;

int main(void)
{
	int retval;
	int count = 0;
	char message[] = "Original Message";
	getcontext(&GlobalContext);
	fprintf (stderr,"%d. message = %s\n", count, message);
	if (count >= 4)
		return;
	// The following line is quite important.
	// Notice that count retains its current value
	// after we set the context back to GlobalContext.
	count++;
	if (count % 2 == 0)
		proc1(message);
	else
		proc2(message);

}
void proc1(char* message)
{
	strcpy (message, "Newer Message");
	setcontext(&GlobalContext);
}	
void proc2(char* message)
{
	strcpy (message, "Newest Message");
	setcontext(&GlobalContext);
}	

		 

