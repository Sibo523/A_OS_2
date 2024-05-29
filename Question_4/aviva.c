#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){
    for(size_t i = 0; i <argc; i++){
        printf("%s ", *(argv+i));
    }
    int opt = getopt(argc, argv, "ttt");
    printf(" %c\n", opt);
    return 0;
}