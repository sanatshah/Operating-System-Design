#ifndef H_MYPTHREAD
#define H_MYPTHREAD
#include <ucontext.h>

//Definitions
#define MAXTHREADS	512
#define STACKSIZE	16384

// Types
typedef struct {
	// Define any fields you might need inside here.
} mypthread_attr_t;

typedef struct mypthread_t mypthread_t;
struct 	mypthread_t{
	int id;
	int active;		//0 = not active, 1 = paused, 2 = running
	int blocked;		//blocked means its waiting for a thread to exit
	int joined;		//if someone joins me
	ucontext_t context;
	mypthread_t *joinedThread; 	/*this is the pointer to the thread that joined me*/
};

// Functions
int mypthread_create(mypthread_t *thread, const mypthread_attr_t *attr,
			void *(*start_routine) (void *), void *arg);

void mypthread_exit(void *retval);

int mypthread_yield(void);

int mypthread_join(mypthread_t thread, void **retval);

void initTable();

mypthread_t* find();

//Environment Variables 
int tableCreated;
mypthread_t *table[MAXTHREADS];
static int numThreads = 0;
static mypthread_t* curr;


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

static inline int mypthread_mutex_init(mypthread_mutex_t *mutex,
			const mypthread_mutexattr_t *attr) { return 0; }

static inline int mypthread_mutex_destroy(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_lock(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_trylock(mypthread_mutex_t *mutex) { return 0; }

static inline int mypthread_mutex_unlock(mypthread_mutex_t *mutex) { return 0; }

#endif
