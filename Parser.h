//-----------------------------------------------------------------------------
// Parser.h
//-----------------------------------------------------------------------------
#ifndef _PARSER_H_
#define _PARSER_H_
#include "Handle.h"
// Export Types ---------------------------------------------------------------
// Takes in a request and parses it into usable data
int parse_request(Request **r, char *token);

#endif
