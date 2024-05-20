CC = gcc 
Flags = -Wall -g -Wextra

all:mync 

mync: mynetcat.c
	$(CC) $(Flags) -o $@ $^

Phony: clean all 
clean:
	rm -f mync