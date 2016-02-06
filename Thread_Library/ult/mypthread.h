#ifndef H_MYPTHREAD
#define H_MYPTHREAD

#include <ucontext.h>

//Definitions
#define MAXTHREADS	512
#define STACKSIZE	16384
#define threadCount	16384

// Types
typedef struct {
	// Define any fields you might need inside here.
} mypthread_attr_t;

typedef struct {
	int active;
	int executing;
	int joined;
	int id;
	ucontext_t context;
	mypthread_t parent;
} mypthread_t;

// Functions
int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
			void *(*start_routine) (void *), void *arg);

void mypthread_exit(void *retval);

int mypthread_yield(void);

int mypthread_join(mypthread_t thread, void **retval);

void initTable();

//Environment Variables 
int tableCreated;
mypthread_t *table[MAXTHREADS];

/* Don't touch anything after this line.
 *
 * This is included just to make the mtsort.c program compatible
 * with both your ULT implementation as well as the system pthreads
 * implementation. The key idea is that mutexes are essentially
 * useless in a cooperative implementation, but are necessary in
 * a preemptive implementation.
 */

typedef int mypthread_mutex_t;
typedef int mypthread_mutexattr_t;

inline int mypthread_mutex_init(mypthread_mutex_t *mutex,
			const mypthread_mutexattr_t *attr) { return 0; }

inline int mypthread_mutex_destroy(mypthread_mutex_t *mutex) { return 0; }

inline int mypthread_mutex_lock(mypthread_mutex_t *mutex) { return 0; }

inline int mypthread_mutex_trylock(mypthread_mutex_t *mutex) { return 0; }

inline int mypthread_mutex_unlock(mypthread_mutex_t *mutex) { return 0; }

#endif
