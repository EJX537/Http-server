//-------------------------------------------------------------------------------------------------
// Httpserver.c
//-------------------------------------------------------------------------------------------------
#include "queue.h"
#include "bind.h"
#include "Handle.h"
#include "Audit.h"
#include "Threads.h"
#include <err.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <pthread.h>
#include <sys/socket.h>
#include <assert.h>

struct sigaction action;
void sigterm(int signum) {
    if (signum == SIGTERM || signum == SIGINT) {
        running = 0;
        pthread_mutex_lock(lock_mutex(queue));
        running_threads = thread_count;
        pthread_mutex_unlock(lock_mutex(queue));
        while (running_threads > 0) {
            pthread_mutex_lock(lock_mutex(queue));
	        pthread_cond_signal(lock_empty(queue));
	        pthread_cond_signal(lock_full(queue));
            pthread_mutex_unlock(lock_mutex(queue));
        }
        for (int i=0; i < thread_count; i++) {
            pthread_join(pthreads[i], NULL);
        }
        if (queue) {
            queue_delete(&queue);
        }
        if (list) {
            freeList(&list);
        }
        close(log_fd);
        free(pthreads);
        exit(EXIT_SUCCESS);
    }
}
int main(int argc, char *argv[]) {
    int listenfd, opt;
    log_fd = 0;
    char *end;
    uint16_t port;
    while ((opt = getopt(argc, argv, ":t:l:")) != -1) {
        switch (opt) {
        case 't':
            thread_count = (int)strtol(optarg, &end, 10);
            break;
        case 'l':
            log_fd = open(optarg, O_WRONLY | O_CREAT | O_TRUNC, 0777);
            break;
        case ':':
            if (optopt == 't') thread_count = DEFAULT_THREAD_COUNT;
            if (optopt == 'l') log_fd = 0;
            break;
        }
    }
    printf("threads: %d\n", thread_count);
    printf("open fd: %d\n", log_fd);
    if (argc - optind != 1) {
        errx(EXIT_FAILURE, "correct usage: ./httpserver [-t threads] [-l logfile] <port>\n");
    }
    if (thread_count < 1) {
        errx(EXIT_FAILURE, "thread error: invalid thread count: %d\n", thread_count);
    }
    port = strtouint16(argv[optind]);
    listenfd = create_listen_socket(port);
    /**
     * Listen to the errors produced by creating the socket
     * Then returning the correct error message
    */
    if (listenfd == -1) {
        errx(EXIT_FAILURE, "invalid port number: %s", argv[optind]);
    } else if (listenfd == -2) {
        errx(EXIT_FAILURE, "socket error");
    } else if (listenfd == -3) {
        errx(EXIT_FAILURE, "bind error");
    } else if (listenfd == -4) {
        errx(EXIT_FAILURE, "listen error");
    }
    // Sigterm
    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = sigterm;
    sigaction(SIGTERM, &action, NULL);
    sigaction(SIGINT, &action, NULL);
    // Start queue of size (1024)
    queue = queue_new(1024);
    // Create threads
    pthreads = (pthread_t *) calloc(thread_count, sizeof(pthread_t));
    if (create_threads(thread_count, &pthreads, queue, &log_fd) < 0) return EXIT_FAILURE;
    while (running) {
        int connfd = accept(listenfd, NULL, NULL);
        if (connfd < 0) {
            warn("accept error");
            continue;
        }
        Request *r = newRequest(connfd);
        if (!queue_full(queue)) {
            queue_push(queue, (void *)r);
        }
    }

    return EXIT_SUCCESS;
}
