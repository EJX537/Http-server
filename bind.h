//-----------------------------------------------------------------------------
// bind.h
//-----------------------------------------------------------------------------
#include <stdint.h>

// Export Fuctions -------------------------------------------------------------
int create_listen_socket(uint16_t port); 
/**
 * returns a fd as a positive integer if successful
 * Parses port number and binds and listens on it.
 * returns -1 if passed an invalid port number
 * returns -2 if opening the socket failed
 * returns -3 if binding the socket failed
 * returns -4 if listening failed
*/ 

uint16_t strtouint16(char number[]);
/**
 * converts a string into a 16 bit unsigned integer
 * Return 0 on invalid string input
*/
