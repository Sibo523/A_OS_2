CC = gcc 
FLAGS = -Wall -O2 -fprofile-arcs -ftest-coverage -g

all:mync 

mync: mynetcat.c
	$(CC) $(Flags) -o $@ $^
cov:
	gcov mynetcat.c

Phony: clean all 
clean:
	rm -f mync