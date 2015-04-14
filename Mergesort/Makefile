#Mergecount compiler

CC = gcc
INC =
FLAGS = -D_GNU_SOURCE -g -W -Wall -std=c99
LIBS =

all: merge_count

merge_count.o: mergecount.c
	$(CC) -c $(FLAGS) $(INC) $< -o $@ $(LIBS)

merge_count: merge_count.o
	$(CC) $(FLAGS) $^ -o $@ $(LIBS)

clean:
	$(RM) -r *.o merge_count
