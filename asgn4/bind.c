//-------------------------------------------------------------------------------------------------
// bind.c
// TODO: Explaination
//-------------------------------------------------------------------------------------------------

#include "bind.h"

#include <assert.h>
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>

/**
 * converts a string into a 16 bit unsigned integer
 * Return 0 on invalid string input
*/
uint16_t strtouint16(char number[]) {
    char *last;
    long num = strtol(number, &last, 10);
    if (num <= 0 || num > UINT16_MAX || *last != '\0') {
        return 0;
    }
    return num;
}
/**
 * Take in a port number and creates a socket connection
 * Or return an error code
*/
int create_listen_socket(uint16_t port) {
    signal(SIGPIPE, SIG_IGN);
    if (port == 0) {
        return -1;
    }
    struct sockaddr_in addr;
    int listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        return -2;
    }
    memset(&addr, 0, sizeof addr);
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htons(INADDR_ANY);
    addr.sin_port = htons(port);
    if (bind(listenfd, (struct sockaddr *) &addr, sizeof addr) < 0) {
        return -3;
    }
    if (listen(listenfd, 500) < 0) {
        return -4;
    }
    return listenfd;
}
