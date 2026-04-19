#Disjoint Sets compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c11
LIBS =

all: dsets.o

dsets.o: dsets.c dsets.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

clean:
	$(RM) -r *.o
