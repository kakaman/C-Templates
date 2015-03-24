#Graph compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c99
LIBS =

all: priorityqueue

priorityqueue.o: priorityqueue.c
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

priorityqueue: priorityqueue.o
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) -r *.o priorityqueue

