//-------------------------------------------------------------------------------------------------
// Threads.c
//-------------------------------------------------------------------------------------------------
#include "queue.h"
#include "List.h"
#include "Handle.h"
#include <err.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
List list = NULL;
queue_t *queue = NULL;
pthread_t *pthreads = NULL;
int log_fd;
int running_threads;
void *worker_thread() {
    void *e = NULL;
    while (running) {
        if (!queue_pop(queue, &e)) {
            break;
        };
        handle_connection((Request *)e, queue, list, log_fd);
    }
    running_threads--;
	pthread_cond_signal(lock_empty(queue));
    pthread_cond_signal(lock_full(queue));  
    pthread_mutex_unlock(lock_mutex(queue));
    return NULL;
}

int create_threads(int size, pthread_t **t) {
    list = newList();
    for (int i=0; i < size; i++) {
        if (pthread_create(*t + i, NULL, worker_thread, NULL) != 0) {
            warnx("Failed to create a thread.\n");
            return -1;
        }
        running_threads++;
    }
    return 1;
}
