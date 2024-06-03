CC = gcc 
FLAGS = -Wall -O2 -fprofile-arcs -ftest-coverage
all:mync 

mync: mynetcat.c
	$(CC) $(Flags) -o $@ $^

Phony: clean all 
clean:
	rm -f mync .gcda .gcno .gcov .info
