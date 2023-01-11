Eric Xie \
ejxie \
Fall 2022 CSE130 asgn3

# Programming Assignment 3
This assignment aims to create an audit log that orders the requests the server processes them.

## Building 

The httpserver program can be built with the following commands below:
```
$ make 
```

## Running

To run the program after compiling it, you can run the command:
```
$ ./httpserver [-t threads] [-l logfile] <port>
```

## Files:
|File         |Description                                                                       | 
|:-----------:| -------------------------------------------------------------------------------- |
|Httpserver.c |A source file that contains the main logic listens for the client.                |
|bind.c       |A source file that contains the create socket.                                    |
|Handle.c     |A source file that contains the logic to handle connects and directs the request. |
|Parser.c     |A source file that contains the logic that parses the request.                    |
|Method.c     |A source file that contains the logic that acts depending on the method           |
|Audit.c      |A source file that contains the logic to logs the requests processed by server.   |
|Threads.c    |A source file that contains the logic to creating threads.                        |
|queue.c      |A source file that contains the logic for the queue API.                          |
|List.c       |A source file that contains the logic for the Linked List API.                    |
|ReturnCodes.c|A source file that contains the logic to write reponse to client.                 |
|Makefile     |A file that offers the ability to generate executables and clean generated files. |
|README.md    |A file that contains the descriptions of each file and its' role in this project. |

# Design Decisions
Documentation of the choices I made working on this assignment.
## Modules
|Module    |Purpose                                                                                 | 
|:--------:| -------------------------------------------------------------------------------------- |
|unistd.h  |Included for the O\_RDONLY macro that is used to indicate read only for the file descriptor.|
|fcntl.h   |Included for the file descriptor macros for stdin, stdout, and stderr.                  |
|string.h  |Included to help deal with error handling PROT.                                         |
|stdio.h   |Included for the fprintf function to print error messages to standard error.            |
|errno.h   |Included to deal with a variety of exit statuses as well as error statuses.             |
|err.h     |Included to print out the correct error messages.                                       |
|stdbool.h |Included for the boolean (bool) type.                                                   |
|sys/stat.h|Included for the stats struct and to check file metadata (permissions and size).        |
|sys/socket.h |Included for the ability to create sockets.                                          |
|netinet/in.h |Included for the sockaddr_in (uint16_t) type.                                        |
|pthread.h|Included for the ability to multithread and its associated properties                    |
|arpa/inet.h |Included for the uint16_t type.                                                       |
## Data Structure and Algorithms
A queue holds all the incoming requests and dequeues them according to availability and seniority. The queue is a linked list that acts as a bounded buffer. Flock wasn't working for me and pthread RW_Lock was not working across files. So, I created a linked list that holds metadata about the each file and lock/unlock them accordingly.
## Coherency and Atomicity
The audit log's coherency and atomicity is inherited from handle_connection() and the interactions between the queue and locks on the files. All request are enqueued and dequeued in the order it is recieved. The files is placed within a linked list alongside other metadata needed to lock the file to prevent read and write overrides. Additionally, there is a mutex lock when writing to the audit log to ensure atomicity. In the event that two thread wants to write to the same file. The first one will increment the writer count for that file and the other thread(s) will need to wait until the writing thread is done before signaling the file is avaliable. 
## Design 
Given the specifications of the Audit Log, I created a simple function that writes an entry to an outfile given a file path and status code. Placed at the end of the GET/HEAD/PUT request, it will always log the response. There was no need to worry about sigterm, because all memory is allocated dynamically, writes itself to output, and freed. 
## Data Structures and Algorithms
All the relavent information from the client's request is stored in a Request struct. The parsing algorithm used allocates the exact memory need for the method, uri, and prot. When reading the file of uri, it does it in chucks until read returns a 0.
## Multi-Threading
This server is multithreaded. Explaination in Asignment #4
## Errors
Multithreading presents errors that were unusual and that you don't think about normally. Passing references to the thread was an unexpected source of error (only in valgrind). It does not consistantly dereference pointers to all sorts of values ranging from int to structs. I noticed this error when valgrind gave me an error for invalid read-size. The obsecure nature of the error was a real head scratcher. The solution for me the use of global variables. Audit itself was largely error free.