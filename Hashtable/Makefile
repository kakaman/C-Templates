#Hashtable compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c99
LIBS =

all: hashtable

test.o: test.c
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

hashtable.o: hashtable.c hashtable.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

hashtable: test.o hashtable.o
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) -r *.o hashtable