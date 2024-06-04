CC = gcc 
Flags = -g -Wall -fprofile-arcs -ftest-coverage

all:ttt

ttt: ttt.c
	$(CC) $(Flags) -o $@ $^
test:all
	chmod +x test.sh
	./test.sh
Phony: clean all 
clean:
	rm -f ttt 