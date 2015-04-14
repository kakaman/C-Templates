#Kruskal MST compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c99
LIBS =

all: mst

mst.o: mst.c
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

heap.o: heap.c heap.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

hashtable.o: hashtable.c hashtable.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

graph.o: graph.c graph.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

dsets.o: dsets.c dsets.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

quicksort.o: quicksort.c quicksort.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

mst: mst.o heap.o graph.o dsets.o quicksort.o hashtable.o
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) -r *.o mst
