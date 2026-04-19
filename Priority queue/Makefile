#Graph compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c11
LIBS =

all: priorityqueue.o

priorityqueue.o: priorityqueue.c priorityqueue.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

clean:
	$(RM) -r *.o

