
CC = gcc
CFLAGS = -Wall -Wextra

all: mync

mync: mynetcat.c
	$(CC) $(CFLAGS) -o mync mynetcat.c

clean:
	rm -f mync *.o