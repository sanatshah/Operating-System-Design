#include "mypthread.h"
#include <stdlib.h>
#include <stdio.h>

int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
                        void *(*start_routine) (void *), void *arg){

	int i;
	char* newStack = calloc(STACKSIZE, 1);
	
	if(!tableCreated)
	{
		initTable();
		//create main thread
		mypthread_t *mainThread = (mypthread_t *) malloc(sizeof(mypthread_t));
		table[0] = mainThread;
		table[0]->id = 0;
		table[0]->joined = 0;
		table[0]->active = 2;
		table[0]->blocked = 0;
		table[0]->joinedThread = NULL;
		if(getcontext(&table[0]->context) == -1)
		{
			perror("getcontext error");
			return -1;	//error
		}
		curr = table[0];
	}
	for(i = 1; i < MAXTHREADS; i++)
	{
		if(table[i] == NULL || !table[i]->active)
		{
			numThreads++;
			table[i] = thread;
			table[i]->active = 1;
			table[i]->joined=0;
			table[i]->id = i;
			table[i]->blocked = 0;
			//table[i]->parent = NULL;
			table[i]->joinedThread = NULL;
			if(getcontext(&table[i]->context) == -1)
			{
				perror("getcontext error");
				return -1;	//error
			}
			table[i]->context.uc_stack.ss_sp = newStack;
			table[i]->context.uc_stack.ss_size = STACKSIZE;
			table[i]->context.uc_link = NULL;

			makecontext(&table[i]->context, (void (*)(void))start_routine, 1,arg);
			return 0;
		}
	}
	return -1;
}

void mypthread_exit(void *retval){

	numThreads--;
	mypthread_t *old = curr;
	int i=0;
	char* tmpHolder;
	//if the number of threads is 0 then exit 
	//printf("NUMTHREADS = %d\n\n\n\n\n",numThreads);
	if (numThreads==0){
			
		for(i = 0; i < MAXTHREADS; i++)
		{
					
		}

	}

	//exits and has a parent because of a join
	if (curr->joined){
		curr->joinedThread->blocked = 0;
	}
	
	curr->active=0;
	curr->joined=0;
	char* newStack = calloc(STACKSIZE, 1);

	char* oldStack;
	oldStack = curr->context.uc_stack.ss_sp;
	curr->context.uc_stack.ss_sp = newStack;
	free(oldStack);

	//exits with no parent and with numThreads!=0, must find paused thread
	curr = find();
	swapcontext(&old->context, &curr->context);
}

mypthread_t *find(){
	int i;
	int pos;
	mypthread_t *foundThread;
	//static int current = 0;
	
/*	for(i = 0;i < MAXTHREADS; i++){*/
/*		if(table[i]!=NULL)*/
/*		{*/
/*			printf("id: %d, active: %d, joined: %d, blocked: %d\n", table[i]->id, table[i]->active,  table[i]->joined, table[i]->blocked);*/
/*		}*/
/*	}*/
	//change this to loop around whole thread table
	for(i = curr->id+1; ; i++){
		pos = i%MAXTHREADS;
		if(table[pos]!=NULL)
		{
		//printf("id: %d\n" ,table[pos]->id);
			if(table[pos]->active && !table[pos]->blocked) {
				foundThread = table[pos];
				return foundThread;
			}
		}
	}
	perror("Unable to find paused thread.");
	exit(EXIT_FAILURE);
}

int mypthread_yield(void){
	mypthread_t* newThread = find();
	mypthread_t* oldThread = curr;

	curr->active=1;
	newThread->active=2;

	curr = newThread;
	swapcontext(&oldThread->context, &curr->context);
	return 0;
}

//what to do with retval?
int mypthread_join(mypthread_t thread, void **retval){
	int i;
	mypthread_t *currThread = curr;
	mypthread_t *joiningThread = table[thread.id];
	mypthread_t* newThread;
	
	if(currThread == joiningThread)
	{
		perror("Thread cannot join itself");
		return -1;
	}
	if(!joiningThread->active)
	{
		return 0;
	}
	//if thread is already joined
	if(joiningThread->joined)
	{
		perror("First, cannot join thread");
		printf("%d",joiningThread->id);
		return -1;
	}
	//if joiningThread already joined currThread
	if(currThread->joined && currThread->joinedThread == joiningThread)
	{
		perror("Cannot join thread");
		return -1;
	}
	
	joiningThread->joinedThread = currThread;
	currThread->blocked = 1;
	joiningThread->joined = 1;
	
	mypthread_yield();
	
/*
	if(swapcontext(&(currThread->context), &(joiningThread->context)) == -1) {
		perror("Can't swith to joined thread");
		return -1;
	}
*/
	return 0;
}

void initTable()
{
	int i;
	
	for(i = 0; i < MAXTHREADS; i++)
	{
		table[i] =NULL;
	}
	tableCreated = 1;
}
