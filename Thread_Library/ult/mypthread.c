#include "mypthread.h"
#include <stdlib.h>

//Thread Variables 

static mypthread_t threadArray[threadCount];
static int numThreads = 1;
static mypthread_t *curr;

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
                        void *(*start_routine) (void *), void *arg){

	int i;
	char newStack[STACKSIZE];
	
	if(!tableCreated)
	{
		initTable();
		//create main thread
		mypthread_t *mainThread = (mypthread_t *) malloc(sizeof(mypthread_t));
		table[0] = mainThread;
		table[0]->id = 0;
		table[0]->active = 1;
		table[0]->executing = 1;
		if(getcontext(&table[0]->context) == -1)
		{
			perror("getcontext error");
			return -1;	//error
		}
	}
	for(i = 1; i < MAXTHREADS; i++)
	{
		if(table[i] == NULL || !table[i]->active)
		{
			table[i] = thread;
			table[i]->active = 1;
			table[i]->id = i;
			if(getcontext(&table[i]->context) == -1)
			{
				perror("getcontext error");
				return -1;	//error
			}
			table[i]->context.uc_stack.ss_sp = newStack;
			table[i]->context.uc_stack.ss_size = sizeof(newStack);
			table[i]->context.uc_link = NULL;
			makecontext(&table[i]->context, (void*)start_routine, 1);
			return 0;	
		}
	}
	return -1;
}

void mypthread_exit(void *retval){

	numThreads--;

	//if the number of threads is 0 then exit 
	if (numThreads==0){
		exit(0);
	}

	//exits and has a parent because of a join 
	if (curr->parent){ 
		curr = curr->parent;
	}

	//exits and is not joined, must find paused thread
	
	mypthread_t *paused;
	paused= findPaused();

}


int mypthread_yield(void){


}


int mypthread_join(mypthread_t thread, void **retval){

}

void initTable()
{
	int i;
	for(i = 0; i < MAXTHREADS; i++)
	{
		table[i] = NULL;
	}
	
	tableCreated = 1;
}
