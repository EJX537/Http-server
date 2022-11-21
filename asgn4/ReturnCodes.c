//-------------------------------------------------------------------------------------------------
// ReturnCodes.c
//-------------------------------------------------------------------------------------------------
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "ReturnCodes.h"

// Response for valid GET
void response_OK_GET(int fd, int size) {
    char response[] = "HTTP/1.1 200 OK\r\nContent-Length: ";
    char length[12] = {0};
    char closing[] = "\r\n\r\n";
    sprintf(length, "%d", size);
    write(fd, response, strlen(response));
    write(fd, length, strlen(length));
    write(fd, closing, strlen(closing));
}
// Response for valid
void response_OK(int fd) {
    char response[] = "HTTP/1.1 200 OK\r\nContent-Length: 3\r\n\r\nOK\n";
    write(fd, response, strlen(response));
}
// Response for CREATE file
void response_CREATE(int fd) {
    char response[] = "HTTP/1.1 201 Created\r\nContent-Length: 8\r\n\r\nCreated\n";
    write(fd, response, strlen(response));
}
// Response for Failed Request
void response_BAD_REQUEST(int fd) {
    char response[] = "HTTP/1.1 400 Bad Request\r\nContent-Length: 12\r\n\r\nBad Request\n";
    write(fd, response, strlen(response));
}
// Response for no permission or directory
void response_FORBIDDEN(int fd) {
    char response[] = "HTTP/1.1 403 Bad Request\r\nContent-Length: 10\r\n\r\nForbidden\n";
    write(fd, response, strlen(response));
}
// Response for file not found
void response_NOT_FOUND(int fd) {
    char response[] = "HTTP/1.1 404 Not Found\r\nContent-Length: 10\r\n\r\nNot Found\n";
    write(fd, response, strlen(response));
}
void response_INTERNAL_SERVER_ERROR(int fd) {
    char response[] = "HTTP/1.1 500 Internal Server Error\r\nContent-Length: 23\r\n\r\nInternal Server Error\n";
    write(fd, response, strlen(response));
}
// Response for invalid method
void response_NOT_IMPLENTED(int fd) {
    char response[] = "HTTP/1.1 501 Not Implemented\r\nContent-Length: 16\r\n\r\nNot Implemented\n";
    write(fd, response, strlen(response));
}
