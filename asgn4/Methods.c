#include "Handle.h"
#include "ReturnCodes.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <pthread.h>

int isURI(char *c);
int file_sizer(char *p);
// int search_list(List L, char *uri);
char *construct_filepath(Request *r);
void write_to_file(Request *r, char *remainder, int rcvd);
char *strip_remainder(char *remainder, int start, int rcvd, int max_length);
int search_list(List L, char *uri);
int Ffile(List L, char *uri, int method);
Ftype *complete_task(List L, char *uri, int method);
Ftype *newFile(char *uri);
void freeFile(Ftype *F);
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

// If the request is GET or get, it reads from the the uri and return the contents of the file
int handle_GET(Request *r, List L) {
    // Checks if the file exist and what permissions it has
    int permissions = isURI(r->uri);
    if (permissions == 201) {
        return 404; // Not found
    }
    if (permissions == 403) {
        return 403; // Forbidden
    }
    if (!(permissions == 1 || permissions == 4)) {
        return 403; // Forbidden
    }
    if (!Ffile(L, r->uri, 'r')) return 500; 
    int fd = open(r->uri + 1, O_RDONLY);
    int size = file_sizer(r->uri);
    char buff[4096] = {0};
    int count;
    int cc = 0;
    response_OK_GET(r->fd, size);
    // Reads the rest of the client input
    while ((count = pread(fd, buff, 4096, cc)) > 0) {
        cc += count;
        write(r->fd, buff, count);
    }
    complete_task(L, r->uri, 'r');
    return 200;
}

// If the request is HEAD or head, it returns the byte size of the file
int handle_HEAD(Request *r, List L) {
    // Checks if the file exist and what permission it has
    int permissions = isURI(r->uri);
    if (permissions == 201) {
        return 404; // Not found
    }
    if (permissions == 403) {
        return 403; // Forbidden
    }
    if (!(permissions == 1 || permissions == 4)) {
        return 403; // Forbidden
    }
    if (!Ffile(L, r->uri, 'r')) return 500; 
    open(r->uri + 1, O_RDONLY);
    int size = file_sizer(r->uri);
    response_OK_GET(r->fd, size);
    complete_task(L, r->uri, 'r');
    return 200;
}

// If the request if PUT or put, it opens the URI filepath or creates a new one and writes to it
// the contents of the body to the size of the content length
int handle_PUT(Request *r, List L, char *buff, int rcvd) {
    // Checks if the file exist and what permissions it has
    int permissions = isURI(r->uri); 
    if (permissions == 403) {
        return 403; // Forbidden
    }
    if (!(permissions == 3 || permissions == 4) && permissions != 201) {
        return 403; // Forbidden
    }
    if (!Ffile(L, r->uri, 'w')) return 500; 
    if (permissions == 201) { // File does not exist
        r->uriFD = open(r->uri + 1, O_WRONLY | O_CREAT, 0644); // Creates new file
        write_to_file(r, buff, rcvd);
        response_CREATE(r->fd);
    } else { // File exist
        r->uriFD = open(r->uri + 1, O_WRONLY | O_TRUNC);
        write_to_file(r, buff, rcvd);
        response_OK(r->fd);
    }
    complete_task(L, r->uri, 'w');
    return permissions == 201 ? 201 : 200;

}
// Writes to URI the contents in client input
void write_to_file(Request *r, char *remainder, int rcvd) {
    char buff[4096] = {0};
    int count;
    // Gets whats after the header(s)
    char *rValue = strip_remainder(remainder, r->requestSize, rcvd, r->length);
    write(r->uriFD, rValue, strlen(rValue));
    // If content length of the input is longer than what is read from the buff
    int i = rcvd;
    if (r->length > rcvd) {
        write(r->uriFD, rValue, strlen(rValue));
        while (i < r->length && (count = read(r->fd, buff, 4096)) > 0) {
            write(r->uriFD, buff, count);
            i += count;
        }
    }
    free(rValue);
    rValue = NULL;
    return;
}

// Takes in the original read buffer and returns the body
char *strip_remainder(char *remainder, int start, int rcvd, int max_length) {
    char *rValue;
    int rSize = rcvd - start;
    max_length = max_length < rSize ? max_length : rSize;
    // Get the body of the request
    rValue = calloc(max_length + 1, sizeof(char));
    for (int i=0; i < max_length; i++) {
        rValue[i] = remainder[start + i];
    }
    return rValue;
}

// Free memory
void free_Request(Request **r) {
    free((*r)->method);
    free((*r)->uri);
    free((*r)->prot);
    free((*r)->Request_id);
    close((*r)->fd);
    free(*r);
}

int file_sizer(char *p) {
    struct stat st;
    stat(p + 1, &st);
    return st.st_size;
}

// Checks the URI and makes sure the there is valid permission
int isURI(char *c) {
    int permission = 0;
    struct stat fileStat;
    int exist = stat(c + 1, &fileStat);
    // Check if file exist
    if (exist < 0) return 201;
    // Is Directory
    if (S_ISDIR(fileStat.st_mode)) return 403;
    // Is Readable
    if (fileStat.st_mode & S_IRUSR)
        permission += 1;
    // Is Writable
    if (fileStat.st_mode & S_IWUSR)
        permission += 3;
    // No permission
    return permission ? permission : 403;
}

int Ffile(List L, char *uri, int method) {
    pthread_mutex_lock(&mutex);
    int exist = search_list(L, uri);
    if (exist < 0) {
        Ftype *newF = newFile(uri);
        newF->using++; 
        switch (method) {
        case 'r':
            newF->reader++;
            break;
        case 'w':
            newF->writer++;
            break;
        }
        append(L, newF);
    } else {
        Ftype *F = get(L);
        F->using++; 
        switch (method) {
        case 'r':
            while(F->writer > 0) pthread_cond_wait(F->writing, &mutex);
            F->reader++;
            break;
        case 'w':
            while(F->reader > 0) pthread_cond_wait(F->reading, &mutex);
            while(F->writer > 0) pthread_cond_wait(F->writing, &mutex);
            F->writer++;
            break;
        }
        pthread_mutex_unlock(&mutex);
        return 1;
    }
    pthread_mutex_unlock(&mutex);
    return -1;
}

Ftype *complete_task(List L, char *uri, int method) {
    pthread_mutex_lock(&mutex);
    int exist = search_list(L, uri);
    if (exist < 0) {
    } else {
        Ftype *F = get(L);
        F->using--; 
        switch (method) {
        case 'r':
            F->reader--;
            if (F->reader == 0) {
                pthread_cond_broadcast(F->reading);
            }
            break;
        case 'w':
            F->writer--;
            if (F->writer == 0) {
                pthread_cond_signal(F->writing);
            }
            break;
        }
        if (F->using == 0) {
            freeFile(F);
            delete(L);
        }
        pthread_mutex_unlock(&mutex);
        return F;
    }
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int search_list(List L, char *uri) {
    if (length(L) == 0) return -1;
    moveFront(L);
    while (Index(L) >= 0) {
        Ftype *F = get(L);
        if (strcmp(F->uri, uri) == 0) return 1;
        moveNext(L);
    }
    return -1;
}

Ftype *newFile(char *uri) {
    Ftype *F = (Ftype *)calloc(1, sizeof(Ftype));
    static pthread_cond_t reading = PTHREAD_COND_INITIALIZER;
    static pthread_cond_t writing = PTHREAD_COND_INITIALIZER;
    F->uri = strdup(uri);
    F->reader = 0;
    F->writer = 0;
    F->using = 0;
    F->reading = &reading;
    F->writing = &writing;
    return F;
}

void freeFile(Ftype *F) {
    if (F) {
        free(F->uri);
        pthread_mutex_destroy(&mutex);
        free(F);
        F = NULL;
    }
    return;
}
