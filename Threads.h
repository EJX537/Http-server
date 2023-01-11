//-----------------------------------------------------------------------------
// Threads.h
//-----------------------------------------------------------------------------
#ifndef _Threads_H_
#define _Threads_H_
#include "queue.h"
#include "List.h"
#include <pthread.h>
// Export Types ---------------------------------------------------------------
extern List list;
extern int running_threads;
extern int log_fd;
extern queue_t *queue;
extern pthread_t *pthreads;
// Export Fuctions ------------------------------------------------------------

// TODO: Explain ...
int create_threads(int size, pthread_t **t, queue_t *q, int *outfd);
#endif
