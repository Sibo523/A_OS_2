#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>
#define index 3
#include <sys/wait.h>
#include <stdbool.h>
void runprogram(pid_t pid,char mode,int client_sock,int server_sock,char *exe, char *arg)
{
    if (pid == 0) {
        switch (mode) {
            case 'i':
                dup2(client_sock, 0);
                break;
            case 'o':
                dup2(client_sock, 1);
                break;
            case 'b':
                dup2(client_sock, 0);
                dup2(client_sock, 1);
                break;
        }
        execlp(exe, exe, "123456789", NULL);
        perror("Execution of exe failed");
        exit(EXIT_FAILURE);
    }
    
    int status;
    waitpid(pid, &status, 0);
    close(client_sock);
    close(server_sock);
}

// mync -e “ttt 123456789” -i TCPS4050 -o TCPClocalhost,4455
#define BUFFER_SIZE 1024
void talking(int client_sock,pid_t pid,char* buffer){
    if (pid == 0) { // Child process: read from server
        while (1) {
            ssize_t bytes_read = read(client_sock, buffer, BUFFER_SIZE - 1);
            if (bytes_read <= 0) {
                break; // Exit loop on read error or server disconnect
            }
            buffer[bytes_read] = '\0';
            printf("Received from server: %s\n", buffer);
        }
        close(client_sock);
        exit(0);
    } else { // Parent process: write to server
        while (1) {
            printf("Enter message (-1 to quit): ");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

            if (strcmp(buffer, "-1") == 0) {
                break; // Exit loop if user enters -1
            }

            write(client_sock, buffer, strlen(buffer));
        }
        close(client_sock);
      

        kill(pid, SIGKILL); // Kill child process reading from server
    }
}
int create_client(int port, char mode, const char *server_ip)
{
    int client_sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    memcpy(buffer, "aviva", 1024);
    printf("%d",port);
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (client_sock < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    if (connect(client_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect failed");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    talking(client_sock,pid,buffer);
    return 0;
}
int create_server(char mode, int port, const char *exe, const char *arg,int flag)
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port); // Convert port to network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_sock, 5) < 0)
    {
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    if (strcmp(exe,"ttt") != 0){
        puts(exe);
        perror("exe is not ttt sadge");
        exit(EXIT_FAILURE);
    }
    exe = "../Question_1/ttt";
    socklen_t client_addr_size = sizeof(client_addr);
    client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_addr_size);
    if (client_sock < 0)
    {
        perror("accept failed");
        exit(EXIT_FAILURE);
    }
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if(flag){
        talking(client_sock,pid,buffer);
    }
    else{
        runprogram(pid,mode,client_sock,server_sock,exe,arg);
    }
    return 0;
}

int main(int argc, char *argv[]) {
    bool isServer = false;
    int flag = 1;
    char *execArgs = NULL;
    char mode = 0;
    int type ;
    int port = 0;
    char *exe ;
    char *arg ;

    int opt;
    while ((opt = getopt(argc, argv, "e:iob")) != -1) {
        switch (opt) {
            case 'e':
             // optarg now points to the argument of -e
                puts("has e");
                flag = 0;
                break;
            case 'i':
                mode = opt;
                puts("has i");
                break;
            case 'o':
                mode = opt;
                puts("has o");
                break;
            case 'b':
                mode = opt;
                puts("has b");
                break;
            default:
                fprintf(stderr, "Usage: %s [-e execArgs] -{i,o,b} TCPS4050/TCPC4050\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
    char *l;
    for (int i = 0; i < argc; i++) {
        if (strstr(argv[i], "ttt")) {
            exe = strtok(argv[i]," ");
            arg = strtok(NULL," ");
        }
        if(strstr(argv[i],"TCPS")){
            puts("hello man");
            isServer = true;
            l= strtok(argv[i],"S");
            port = atoi(strtok(NULL,"S"));
            
        }
        if(strstr(argv[i],"TCPC")){
            puts("hello man 2");
            isServer = false;
            l= strtok(argv[i],"C");
            port = atoi(strtok(NULL,"C"));
            port = atoi(strtok(NULL,"C"));

        }
    }
    if (isServer) {
        create_server(mode, port, exe, arg, flag);
    } else {
        create_client(port, mode, "127.0.0.1");
    }

    return 0;
}