//-------------------------------------------------------------------------------------------------
// Handle.c
//-------------------------------------------------------------------------------------------------
#include "ReturnCodes.h"
#include "Parser.h"
#include "Methods.h"
#include "Handle.h"
#include "List.h"
#include "Audit.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <regex.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <signal.h>


int thread_count = DEFAULT_THREAD_COUNT;
volatile sig_atomic_t running = 1;
int total_threads;

int which_request(char *method);

Request *newRequest(int fd) {
    Request *request = calloc(1, sizeof(Request));
    request->method = request->uri = request->prot = request->Request_id = NULL;
    request->requestSize = request->length = request->uriFD = 0;
    request->fd = fd;
    return request;
}

// Reads the request and parses the information then assigning to different response functions
void handle_connection(Request *request, queue_t *q, List L, int outfd) {
    printf("recieve request\n");
    char buff[2048] = {0};
    int rcvd, method, methodResult = 500, requestLine;
    int readed = 0;
    bool complete = false;
    // Read the request
    rcvd = read(request->fd, buff, 2048);
    // Less than the initial request line
    if (rcvd < 14) {
        response_BAD_REQUEST(request->fd);
        free_Request(&request);
        return;
    }
    // Read until header is completed
    while (!complete && readed < 2048) {
        for (int i = 0; i < rcvd; i++) {
            if (buff[readed - 3] == '\r' && buff[readed - 2] == '\n' && buff[readed - 1] == '\r' && buff[readed] == '\n') {
                complete = true;
                break;
            }
            readed++;
        }
        if (!complete) {
            rcvd = read(request->fd, buff, 2048);
        }
    }
    // Too Long
    if (!complete) {
        response_BAD_REQUEST(request->fd);
        free_Request(&request);
        return;    
    };
    requestLine = parse_request(&request, buff);
    printf("rr: %d\n", requestLine);
    // Check the request line and make sure it is valid
    switch (requestLine) {
    case 400: // Invalid Prot
        response_BAD_REQUEST(request->fd);
        free_Request(&request);
        return;
    case 501: // Invalid Method (Not Implemented)
        response_NOT_IMPLENTED(request->fd);
        break;
    case 200: // Continue
        method = which_request(request->method);
        switch (method) {
        case 1: // GET
            methodResult = handle_GET(request, L);
            if (methodResult == 403) response_FORBIDDEN(request->fd);
            else if (methodResult == 404) response_NOT_FOUND(request->fd);
            break;
        case 2: // PUT
            request->requestSize += 2;
            methodResult = handle_PUT(request, L, buff, rcvd);
            if (methodResult == 403) response_FORBIDDEN(request->fd);
            else if (methodResult == 404) response_NOT_FOUND(request->fd);
            break;
        case 3: // HEAD
            methodResult = handle_HEAD(request, L);
            if (methodResult == 403) response_FORBIDDEN(request->fd);
            else if (methodResult == 404) response_NOT_FOUND(request->fd);
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
    pthread_mutex_lock(lock_mutex(q));
    int audit = audit_log(request, outfd, methodResult);
    if (!audit) response_INTERNAL_SERVER_ERROR(request->fd);
    pthread_mutex_unlock(lock_mutex(q));
    free_Request(&request);
    return;
}

// Gets return the case number for the correct method
int which_request(char *method) {
    if (strcmp(method, "GET") == 0 || strcmp(method, "get") == 0) return 1;
    if (strcmp(method, "PUT") == 0 || strcmp(method, "put") == 0) return 2;
    if (strcmp(method, "HEAD") == 0 || strcmp(method, "head") == 0) return 3;
    return -1;
}
