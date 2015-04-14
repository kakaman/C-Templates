#Graph compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c99
LIBS =

all: graph

graph.o: graph.c graph.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

kargergraph.o: kargergraph.c kargergraph.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)
	
graph: graph.o kargergraph.o
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) -r *.o graph

