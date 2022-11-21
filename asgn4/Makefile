#-------------------------------------------------------------------------------------
# Makefile for CSE 130 Programming Assignment 3
# Eric Xie
# ejxie
# Fall 2022
#
# #
# # make                   makes bind
# # make clean			   removes all binaries
#-------------------------------------------------------------------------------------
CC = clang
CFLAGS = -Wall -Werror -Wextra -pedantic
LFLAGS = -pthread
OBJS = Httpserver.o bind.o Handle.o ReturnCodes.o Parser.o Methods.o Audit.o queue.o Threads.o List.o
.PHONY: clean all debug val client

all: httpserver

httpserver: $(OBJS)
	$(CC) $(LFLAGS) -o $@ $(OBJS) 

%.o: %.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f *.o httpserver 

debug: CFLAGS += -g
debug: clean all
