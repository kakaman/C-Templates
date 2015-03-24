#Graph compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c99
LIBS =

all: graph

graph.o: graph.c
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

graph: graph.o
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) -r *.o graph

