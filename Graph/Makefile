#Graph compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c99
LIBS =

all: graph.o dsets.o quicksort.o

graph.o: graph.c graph.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

dsets.o: dsets.c dsets.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

quicksort.o: quicksort.c quicksort.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

clean:
	$(RM) -r *.o
