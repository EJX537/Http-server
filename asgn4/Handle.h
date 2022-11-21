//-----------------------------------------------------------------------------
// Handle.h
//-----------------------------------------------------------------------------
#ifndef _HANDLE_H_
#define _HANDLE_H_

#include "queue.h"
#include "List.h"
#include <signal.h>
#define DEFAULT_THREAD_COUNT 4
// Export Types ---------------------------------------------------------------
// All the component parts of the request sent by the client
typedef struct Request Request;
struct Request {
    char *method;
    char *uri;
    char *prot;
    char *Request_id;

    int requestSize; // Remainder
    int length; // content-length
    int uriFD; // Files path
    int fd; // Client
};
extern int thread_count;
extern volatile sig_atomic_t running;

// Export Fuctions ------------------------------------------------------------
// Reads the request and parses the information then assigning to different response functions
void handle_connection(Request *r, queue_t *q, List L, int outfd);
// TODO: Explain ...
Request *newRequest(int fd);
#endif
