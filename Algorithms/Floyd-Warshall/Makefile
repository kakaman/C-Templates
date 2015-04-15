#Floyd-Warshall Algorithm compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c99
LIBS =

all: floyd-warshall

floyd-warshall.o: floyd-warshall.c
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)
	
heap.o: heap.c heap.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

graph.o: graph.c graph.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

dsets.o: dsets.c dsets.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

quicksort.o: quicksort.c quicksort.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

floyd-warshall: graph.o floyd-warshall.o quicksort.o dsets.o heap.o quicksort.o
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) -r *.o floyd-warshall