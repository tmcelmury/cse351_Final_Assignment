#include <stdlib.h>
#include <stdio.h>
#include <ucontext.h>
#include <string.h>

int main(void)
{
	ucontext_t context0;
	int counter = 0;
	long long int testval;
	char message[64];
	// try swapping these two lines
	testval = 100;
	getcontext(&context0);
	// What is the purpose of the line above?
	strcpy (message, "Original Message");
	fprintf (stderr,"1. message = %s, testval = %lld\n", message, testval);
	strcpy (message, "New Message");
	fprintf (stderr,"2. message = %s, testval = %lld\n", message, testval);
	testval = testval * 2;
	fprintf (stderr,"3. message = %s, testval = %lld\n", message, testval);
	strcpy (message, "Newest Message");
	testval = testval * 4;
	fprintf (stderr,"4. message = %s, testval = %lld\n", message, testval);
	while (testval > 0)
	{
		setcontext(&context0);
		// What is the purpose of the line above?
	}

}	


		 

