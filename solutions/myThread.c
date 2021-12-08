/* This is the skeleton program for you to use to create your own
 * threading library based on setcontext, makecontext, swapcontext,
 * getcontext routines. This work must be done on CSE or CSCE. */ 

/* Enter the names of all team members here:
 * Member 1: Trent McElmury
 * Member 2: Kevin Wertz	*/
#include "myThread.h"
#include <time.h>
#include <signal.h>

#define INTERVAL 2000
#define BOUND 100000 
#define DELAY 5000
#define STACKSIZE 8192 
#define THREADS 12 
#define PRINT 10000

#define POSIX 1
#define MYSEM 2
#define NONE 0
/* This program can operate in three modes:
 * with POSIX semaphore (define MUTEX POSIX) 
 * with your own semaphore (define MUTEX MYSEM)
 * or without any semaphore (don't define MUTEX at all)*/
//#define MUTEX POSIX 
//#define MUTEX MYSEM 
#define MUTEX NONE 
#define DEBUG 1

ucontext_t context[THREADS], myCleanup, myMain;
int status[THREADS];
struct itimerval clocktimer;
int totalThreads = 0;
int currentThread = 0;
volatile int sharedCounter = 0;
#if MUTEX == POSIX 
sem_t mutex;
#elif MUTEX == MYSEM 
mysem_t mutex;
#endif

int main( void )
{
#if MUTEX==POSIX
	sem_init(&mutex, 0, 1);
#endif
	getcontext(&myMain);
	char ** myStack[THREADS];
	char myCleanupStack[STACKSIZE];
	int j;
	/* initialize timer to send signal every 2 ms */
	clocktimer.it_value.tv_sec = 0;
	clocktimer.it_value.tv_usec = INTERVAL;
	clocktimer.it_interval.tv_sec = 0;
	clocktimer.it_interval.tv_usec = INTERVAL;
	setitimer (ITIMER_REAL, &clocktimer, 0);
	signal (SIGALRM, signalHandler);

	// set up cleanup context 
	getcontext(&myCleanup);
	myCleanup.uc_stack.ss_sp = &myCleanupStack;
	myCleanup.uc_stack.ss_size = STACKSIZE;
	myCleanup.uc_link = &myMain;
	makecontext(&myCleanup, cleanup, 0);

	/* Set up contexts for the user threads that will run task1 and task2.
	 * Assign even number threads to task1 and odd number threads to task2. 
	 */
	for (j = 0; j < THREADS; j++) {

		// set up your context for each thread 
		// Pass the current value of j as the thread id
		getcontext(&context[j]);
		myStack[j] = malloc(STACKSIZE);
		context[j].uc_link = &myCleanup;
		context[j].uc_stack.ss_sp = myStack[j];
		context[j].uc_stack.ss_size = STACKSIZE;

		if (j % 2 == 0){
#if DEBUG == 1
			printf("Creating task1 thread[%d].\n", j);
#endif
			makecontext(&context[j], task1, 1, j);	
		}
		else {
#if DEBUG == 1
			printf("Creating task2 thread[%d].\n", j);
#endif
			makecontext(&context[j], task2, 1, j);
		}
		// Status array for each thread, 1 means ready to execute,
		// 2 means currently executing, 0 means it has finished execution. 
		status[j] = 1;

		// Keep track of the number of task1 and task2 threads
		// When totalThreads is equal to 0, all tasks have finished, 
		// return to the main thread.
		totalThreads++; 
	}

#if DEBUG == 1
	printf("Running threads.\n");
	for (int j = 0; j < THREADS; j++) {	// For debugging purposes
		printf("Address: %x		Size(B): %i\n", context[j].uc_stack.ss_sp, context[j].uc_stack.ss_size);
	}
#endif
	/* You need to switch from the main thread to the first thread. Use the
	 * global variable currentThread to keep track of the currently running thread. */

	// start running your threads here.
	currentThread = 0;

	swapcontext(&myMain, &context[0]);

	/* At this point, threads have all finished. 
	   free the malloc'd stack space for each thread */

	for(j = 0; j < THREADS; j++)
	{	
		free(myStack[j]);
	}
	printf("==========================\n");
	printf("sharedCounter = %d\n", sharedCounter);
	printf("==========================\n");
	printf("Total Threads left: %d\n",totalThreads);
	printf("Status of threads at finish: ");
	for(int i=0;i<THREADS;i++)
	{
		printf(" %i,",status[i]);
	}
	printf("\n");
#if DEBUG == 1
	printf("Program terminates successfully.\n");
	printf("Note that it is OK for the execution orders\n");
	printf("to differ from one run to the next!\n");
#endif
}

void signalHandler( int signal ) {
	/* This method switches from one thread to the next when a timer
	 * signal arrives. It needs to pick the next runnable thread to
	 * execute and then switch the context from the current thread
	 * (indicated by currentThread) to the next thread 
	 *
	 * Hint: it should never pick a thread that already completed its
	 * task so you may need to consult the status array. Otherwise, you
	 * may get segmentation faults. */
	if(status[currentThread]==2)  //only if thread was running set to waiting to run	
	{										
		status[currentThread] = 1;
	}
	int lastThread = currentThread;
	int j=0;
	for(int i=currentThread;j<THREADS;j++)		//Go through all threads to find one with a context of 1
	{											// once found, break from loop and swap to it
		if(currentThread>=THREADS - 1)			// If none found, do not swap contexts
		{
			currentThread=0;
		}
		else
		{
			currentThread++;
		}
		if(status[currentThread]==1)
		{
			break;
		}
	}

	if(j>=THREADS)	//If this is the case all thread states were examined and none were in the waiting to run state
	{				//Do not schedule a new thread as the cleanup is currently being run
		return;
	}
	else			//The next thread that could be run was found, swap to its context
	{
		status[currentThread]=2;
		swapcontext(&context[lastThread], &context[currentThread]);
		return;
	}
}

void cleanup() {
	/* When a thread (task1 or task2) finishes, it should return to
	 * this method. It should then update the status array so that this
	 * particular thread is shown as finished and should not be
	 * scheduled again. You should also decrease the number of threads
	 * (totalThreads--) each time a thread finishes. When totalThreads
	 * is equal to 0, this function can return to the main thread. */
	totalThreads--;
	status[currentThread] = 0;
	while (totalThreads != 0){}; //Wait until quantum is over, or if last executing thread, return to mainContext and finish program
	return; 					 //cleanup returns to mainContext
}

void task1( int tid )	// Do not modify
{
	int i, count = 0;
	while (count < BOUND) {
		for (i = 0; i < DELAY; i++);
#if MUTEX == POSIX
		sem_wait(&mutex);
		sharedCounter = sharedCounter + 1;
		sem_post(&mutex);
#elif MUTEX == MYSEM
		mysem_wait(&mutex);
		sharedCounter = sharedCounter + 1;
		mysem_post(&mutex);
#elif MUTEX == NONE
		sharedCounter = sharedCounter + 1;
#endif
		count++;
#if DEBUG == 1
		if (count%PRINT == 0)
			printf ("task1 [tid = %d]: sharedCounter = %d.\n", tid, sharedCounter);
#endif
	}		
}

void task2( int tid )	// Do not modify
{
	int i, count = 0; 
	while (count < BOUND) {
		for (i = 0; i < DELAY; i++);
#if MUTEX == POSIX
		sem_wait(&mutex);
		sharedCounter = sharedCounter - 1;
		sem_post(&mutex);
#elif MUTEX == MYSEM
		mysem_wait(&mutex);
		sharedCounter = sharedCounter - 1;
		mysem_post(&mutex);
#elif MUTEX == NONE
		sharedCounter = sharedCounter - 1;
#endif
		count++;
#if DEBUG == 1 
		if (count%PRINT == 0)
			printf ("task2 [tid = %d]: sharedCounter = %d.\n", tid, sharedCounter);
#endif
	}		
}
