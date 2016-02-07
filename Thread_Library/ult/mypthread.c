#include "mypthread.h"
#include <stdlib.h>
#include <stdio.h>

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
                        void *(*start_routine) (void *), void *arg){

	int i;
	char newStack[STACKSIZE];
	
	if(!tableCreated)
	{
		initTable();
		//create main thread
		mypthread_t *mainThread = (mypthread_t *) malloc(sizeof(mypthread_t));
		numThreads++;
		table[0] = mainThread;
		table[0]->paused=0;
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
			numThreads++;
			table[i] = thread;
			table[i]->active = 1;
			table[i]->paused=0;
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
		setcontext(&(curr->context));
	}

	//exits with no parent and with numThreads!=0, must find paused thread
	curr = find(1,1);	
	setcontext(&(curr->context));

}

mypthread_t *find(int paused, int joined){
	int i=0; 
	int foundPaused=0;
	mypthread_t *foundThread;

	for(i = 0;i < MAXTHREADS; i++){
		if (paused==1){
		//looking for a thread, with restriction of it being paused
			if(table[i]->paused) {
				foundThread = table[i]; 
				foundPaused=1;
				return foundThread;
			}
		} else if (joined==0 && paused==0){ 
		//looking for a thread, with restriction of it being paused and not joined
			if(table[i]->paused && !table[i]->joined) {
				foundThread = table[i]; 
				foundPaused=1;
				return foundThread;
			}
		}
	} 

	if(!foundPaused) {
		printf("Unable to find paused thread.");
		exit(EXIT_FAILURE);
	}	
}


int mypthread_yield(void){

	//find a thread to start
	mypthread_t* newThread = find(0,0);
	mypthread_t* oldThread = curr;

	//change thread variables
	curr->paused=1;
	curr->active=0;
	newThread->active=1;
	newThread->paused=0;
	newThread->joined=0;

	curr = newThread;

	//switch threads
	if(setcontext(&(curr->context)) == -1){
		//throw error
	}

	return 0;
}


int mypthread_join(mypthread_t thread, void **retval){

	//what to do with retval?
	int i;
	mypthread_t *currThread = NULL;
	mypthread_t *joiningThread = table[thread.id];
	
	//find current thread. Probably can optimize by keeping track of current active thread
	for(i = 0; i < MAXTHREADS; i++)
	{
		if(table[i]->active)
		{
			currThread = table[i];
			break;
		}
	}
	//Is this error check necessary
	if(currThread == NULL)
	{
		perror("Cannot find active thread");
		return -1;
	}
	//if thread is already joined
	if(joiningThread->joined)
	{
		perror("Cannot join thread");
		return -1;
	}
	//if thread already joined calling thread
	if(currThread->joinedThread!=NULL && currThread->joinedThread == joiningThread /*is this comparison correct */ )
	{
		perror("Cannot join thread");
		return -1;
	}
	
	currThread->joinedThread = joiningThread;
	currThread->joined=1;
	currThread->runnable = 0;
	
	//waiting for thread to finish
	while(joiningThread->active)
	{
		mypthread_yield();
	}
	
	currThread->joinedThread = NULL;
	currThread->runnable = 1;
	currThread->joined = 0;
	
	return 0;
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
