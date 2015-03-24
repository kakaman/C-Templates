#Ctrie compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c99
LIBS =

all: ctrie

ctrie.o: ctrie.c ctrie.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

ctrie: ctrie.o
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) -r *.o ctrie