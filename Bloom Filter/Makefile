#Bloom filter compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c99
LIBS =

all: bloom

test.o: test.c
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

bloomfilter.o: bloomfilter.c bloomfilter.h
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

bloom: test.o bloomfilter.o
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) -r *.o bloom

