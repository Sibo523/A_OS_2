
CC = gcc
CFLAGS = -Wall -Wextra

all: mync ttt

ttt: main.c
	$(CC) $(Flags) -o $@ $^

mync: mynetcat.c
	$(CC) $(CFLAGS) -o mync mynetcat.c

clean:
	rm -f mync *.o ttt