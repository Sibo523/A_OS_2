CC = gcc 
Flags =  -Wall -O2 -fprofile-arcs -ftest-coverage

all:ttt

ttt: main.c
	$(CC) $(Flags) -o $@ $^

Phony: clean all 
clean:
	rm -f ttt *.gcda *.gcno *.gcov *.info