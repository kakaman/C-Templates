#Dictionary compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c99
LIBS =

all: dictionary

test.o: test.c
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

dictionary.o: dictionary.c dictionary.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

dictionary: dictionary.o dictionary.o
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) -r *.o dictionary