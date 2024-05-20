CC = gcc 
Flags = -Wall -g -Wextra

all:file1  

file1: main.c
	$(CC) $(Flags) -o $@ $^

Phony: clean all 
clean:
	rm -f file1 