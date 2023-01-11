//-----------------------------------------------------------------------------
// Methods.h
//-----------------------------------------------------------------------------
#ifndef _METHODS_H_
#define _METHODS_H_

#include "Handle.h"
#include "List.h"

// Export Fuctions ------------------------------------------------------------
// If the request is GET or get, it reads from the the uri and return the contents of the file
int handle_GET(Request *r, List L);

// If the request is HEAD or head, it returns the byte size of the file
int handle_HEAD(Request *r, List L);

// If the request if PUT or put, it opens the URI filepath or creates a new one and writes to it
// the contents of the body to the size of the content length
int handle_PUT(Request *r, List L, char *buff, int rcvd);

void free_Request(Request **r);

#endif
