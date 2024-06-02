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
#include <sys/un.h>


int main(int argc, char *argv[])
{   
    unsigned int s, s2;
    int len;
    // create the address struct for socket
    struct sockaddr_un remote, local = {
        .sun_family = AF_UNIX
        // .sun_path = "/tmp/mynetcat"
    };
    strcpy(local.sun_path, "../Question_1/ttt");
    unlink(local.sun_path);
    len = strlen(local.sun_path) + sizeof(local.sun_family);
    // open the socket
    s = socket(AF_UNIX, SOCK_STREAM, 0);
    if (s == -1)
    {
        perror("socket");
        return 1;
    }
    // bind the socket to the address
    if(bind(s, (struct sockaddr *)&local, len) == -1){
        perror("bind");
        return 1;
    }

    // listen for incoming connections
    listen(s, 1);

    // accept connection
    len = sizeof(remote);
    // remote will store the valuse of the client
    s2 = accept(s, (struct sockaddr *)&remote, &len);


    return 0;
}