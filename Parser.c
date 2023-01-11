//-------------------------------------------------------------------------------------------------
// Parser.c
//-------------------------------------------------------------------------------------------------
#include <stdbool.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

#include "Handle.h"

int valid_method(Request **r, char *token);
int parse_header(Request *r, int start, char *token);
int check_header(Request *r, char *header);

// Takes in a request and parses it into usable data
int parse_request(Request **r, char *token) {
    // Check request line
    int next = valid_method(r, token);
    // Error
    if (next == 400) return 400; // Bad Prot
    if (next == 501) return 501; // Bad Method

    // Length after the request line
    (*r)->requestSize += next;

    // Check header
    int error = parse_header(*r, next - 2, token);
    // Error
    if (error == 400) return 400; // Header Error
    return 200;
}

// Makes sure the request status line is correct
int valid_method(Request **r, char *token) {
    int i = 0, lengthMETH = 0, lengthPROT = 0, lengthURI = 0; 
    // Gets the length of the line
    while (true) {
        if (!(token[i] == '\r' && token[i + 1] == '\n')) {
            i++;
        } else {
            break;
        }
    }
    // Get the length of method
    for (int j=0; j < i; j++) {
        if (token[j] == ' ') break;
        lengthMETH++;
    }
    (*r)->method = calloc(lengthMETH + 1, sizeof(char));
    // Gets the method
    for (int j=0; j < lengthMETH; j++) {
        if (token[j] != ' ') (*r)->method[j] = token[j];
    }
    // Get the Length of HTTP
    for (int j=i; j > 0; j--) {
        if (token[j] == ' ') break;
        lengthPROT++;
    }
    lengthPROT--;
    (*r)->prot = calloc(lengthPROT + 1, sizeof(char));
    // Get the HTTP version
    for (int j=0; j < lengthPROT; j++) {
        (*r)->prot[j] = token[i - lengthPROT + j];
    }

    // Check if Prot is the right version
    if (strcmp((*r)->prot, "HTTP/1.1") != 0) return 400;
    // Gets the URI
    (*r)->uri = calloc(i - 1 - strlen((*r)->prot) - strlen((*r)->method), sizeof(char));
    for (int j=(int)strlen((*r)->method) + 1; j < i - 9; j++) {
        (*r)->uri[lengthURI++] = token[j];
    }

    // Check if method is implemented
    if (strcmp((*r)->method, "GET") == 0 || strcmp((*r)->method, "HEAD") == 0 || strcmp((*r)->method, "PUT") == 0 ||
        strcmp((*r)->method, "get") == 0 || strcmp((*r)->method, "head") == 0 || strcmp((*r)->method, "put") == 0) {
     } else {
        // If method is invalid return -501
        free((*r)->prot);
        (*r)->prot = NULL;
        free((*r)->method);
        (*r)->method = NULL;
        free((*r)->method);
        (*r)->method = NULL;
        return 501;
    }

    // Return length of the request line plus the length of the terminators
    return i + 2;
}

// Takes in all the headers and parses them
int parse_header(Request *r, int start, char *token) {
    char line[2048] = {0};
    int v = 0;
    int error;
    bool exist= false;
    // Checks until the end of the buffer or reaches the terminators
    while (!(token[start] == '\r' && token[start + 1] == '\n' && token[start + 2] == '\r'&&
        token[start + 3] == '\n')) {
            if (start > 2045) return 400;
            // Checks each line of the request header
            if (!(token[start] == '\r' && token[start + 1] == '\n')) {
                line[v++] = token[start];
                // Indicates that there exist another line
                exist = true;
            } else {
                // Checks the line
                error = check_header(r, line);
                if (error == 400) return 400;
                v = 0;
                // Reset the buffer
                memset(line, 0, sizeof(line));
                exist = false;
                start++;
            }
            start++;
        }
    // Checks the final header line
    if (exist) {
        error = check_header(r, line);
        if (error == 400) return 400;
    }
    return 200;
}

// Makes sure that the header is formatted correctly
int check_header(Request *r, char *header) {
    char content_length[] = "Content-Length: ";
    char Request_Id[] = "Request-Id: ";
    char *value;
    int v = 0;
    bool isColin = false;
    // Ignore empty header
    if (strlen(header) == 0) return 200;
    // Correctly formatted header
    for (unsigned long i=0; i < strlen(header); i++) {
        if (header[i] == ' ' && isColin == false) {
            if (header[i - 1] == ':') {
                isColin = true;
            } else {
                return 400;
            }
        }
    }
    if (!isColin) return 400;
    // If the header is content-length grab it and place it into request->length
    if (strstr(header, content_length) != NULL) {
        value = calloc(strlen(header) - 15, sizeof(char));
        for (unsigned long i=16; i < strlen(header); i++) {
            value[v++] = header[i];
        }
        r->length = strtol(value, NULL, 10);
        free(value);
    } else if (strstr(header, Request_Id) != NULL) {
        r->Request_id = calloc(strlen(header) - 11, sizeof(char));
        for (unsigned long i=12; i < strlen(header); i++) {
            r->Request_id[v++] = header[i];
        }
    }
    r->requestSize += strlen(header) + 2;
    return 200;
}
