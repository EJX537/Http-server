//-------------------------------------------------------------------------------------------------
// ReturnCodes.c
// Handles all error checking
//-------------------------------------------------------------------------------------------------
// Export Fuctions -------------------------------------------------------------
// 200
void response_OK(int fd);
// 200
void response_OK_GET(int fd, int size);
// 201
void response_CREATE(int fd);
// 400
void response_BAD_REQUEST(int fd);
// 403
void response_FORBIDDEN(int fd);
// 404
void response_NOT_FOUND(int fd);
// 500
void response_INTERNAL_SERVER_ERROR(int fd);
// 501
void response_NOT_IMPLENTED(int fd);
