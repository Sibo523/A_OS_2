CC = gcc 
Flags = -Wall -g -Wextra

all:mync

mync: mync.c
	$(CC) $(Flags) -o $@ $^

Phony: clean all 
clean:
	rm -f mync 