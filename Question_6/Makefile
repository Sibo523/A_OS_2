
CC = gcc
CFLAGS = -g -Wall -fprofile-arcs -ftest-coverage

all: mync everyone

mync: mync.c
	$(CC) $(CFLAGS) -o mync $<

everyone: 
	$(MAKE) -C ../Question_1
# test:all I use it manually
# 	chmod +x test.sh
# ./test.sh
clean:
	rm -f mync *.o ttt
super_clean:
	$(MAKE) -C ../Question_1 clean