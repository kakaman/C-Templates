#Bellman-Ford Algorithm compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c99
LIBS =

all: bellman-ford

bellman-ford.o: bellman-ford.c
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)
	
heap.o: heap.c heap.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

graph.o: graph.c graph.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

dsets.o: dsets.c dsets.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

quicksort.o: quicksort.c quicksort.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

bellman-ford: graph.o bellman-ford.o quicksort.o dsets.o heap.o quicksort.o
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) -r *.o bellman-ford