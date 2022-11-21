//-------------------------------------------------------------------------------------------------
// Audit.c
//-------------------------------------------------------------------------------------------------
#include "Audit.h"

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>

char *format_log(Request *r, int status_code);
// Takes in a request and file path and writes to it
int audit_log(Request *r, int fd, int status_code) {
    char *return_log = format_log(r, status_code);
    if (write(fd < 0 ? STDOUT_FILENO : fd, return_log, strlen(return_log)) < 0) return -1;
    free(return_log);
    return 1;
}
// Return in format: 
// <Oper>,<URI>,<Status-Code>,<RequestID header value>\n
char *format_log(Request *r, int status_code) {
    int counter = 0;
    int request_Length = r->Request_id == NULL ? 1 : strlen(r->Request_id);
    int string_length = strlen(r->method) + 20 + 3 + 3 + request_Length;
    char *return_string = calloc(string_length + 2, sizeof(char));
    char status[4];
    for (unsigned long i = 0; i < strlen(r->method); i++) {
        return_string[counter++] = r->method[i];
    }
    return_string[counter++] = ',';
    for (unsigned long i = 0; i < strlen(r->uri); i++) {
        return_string[counter++] = r->uri[i];
    }
    return_string[counter++] = ',';
    sprintf(status, "%d", status_code);
    for (unsigned long i = 0; i < 3; i++) {
        return_string[counter++] = status[i];
    }
    return_string[counter++] = ',';
    if (r->Request_id == NULL) {
        return_string[counter++] = '0';
    } else {
        for (unsigned long i = 0; i < strlen(r->Request_id); i++) {
            return_string[counter++] = r->Request_id[i];
        }
    }
    return_string[counter] = '\n';
    return_string[++counter] = '\0';
    return return_string;
}
