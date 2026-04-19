#Heap compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c11
LIBS =

all: heap.o

heap.o: heap.c heap.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

clean:
	$(RM) -r *.o
