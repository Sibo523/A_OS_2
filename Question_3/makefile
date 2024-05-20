CC = gcc 
Flags = -Wall -g -Wextra

all:mynetcat  

mynetcat: mynetcat.c
	$(CC) $(Flags) -o $@ $^

Phony: clean all 
clean:
	rm -f mynetcat