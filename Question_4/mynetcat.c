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
#include <getopt.h> 
#include <errno.h>

void runprogram(pid_t pid,char mode,int client_sock,int server_sock,const char *exe, char *arg)
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
        execlp(exe, exe, arg, NULL);
        perror("Execution of exe failed");
        exit(EXIT_FAILURE);
    }
    
    int status;
    waitpid(pid, &status, 0);
    if(client_sock == server_sock){
        close(client_sock);
    }
    else{
        close(server_sock);
        close(client_sock);
    }
}

// // mync -e “ttt 123456789” -i TCPS4050 -o TCPClocalhost,4455
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
            printf("Enter message (-1 to quit): \n");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character

            if (strcmp(buffer, "-1") == 0) {
                break; // Exit loop if user enters -1
            }

            if(write(client_sock, buffer, strlen(buffer)) <= 0){
                break;
            }
        }
        close(client_sock);
      

        kill(pid, SIGKILL); // Kill child process reading from server
    }
}
void talking_udp(int client_sock,pid_t pid,char* buffer, struct sockaddr_in server_addr){
    if (pid == 0) { // Child process: read from server
        while (1) {
            ssize_t bytes_read = recvfrom(client_sock, buffer, BUFFER_SIZE,0,NULL, 0);
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
            printf("Enter message (-1 to quit): \n");
            fgets(buffer, sizeof(buffer), stdin);
            buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline character
            if (strcmp(buffer, "-1") == 0) {
                break; // Exit loop if user enters -1
            }

            if(sendto(client_sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) <= 0){
                break;
            }
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
    printf("%d\n",port);
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

    int optval = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

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

int create_udp_server(char mode, int port, const char *exe, const char *arg, int run, int timeout)
{
    int server_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    server_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (server_sock < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int optval = 1;
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port); // Convert port to network byte order
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (strcmp(exe,"ttt") != 0){
        puts(exe);
        perror("exe is not ttt sadge");
        exit(EXIT_FAILURE);
    }
    exe = "../Question_1/ttt";
    socklen_t client_addr_size = sizeof(client_addr);
    memset(&client_addr,0 , client_addr_size);
    size_t bytes = recvfrom(server_sock, buffer, BUFFER_SIZE,0,(struct sockaddr *)&client_addr, &client_addr_size);
    printf("Received from client: %s\n", buffer);
    
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if(timeout > 0){
        alarm(timeout);
    }
    if(run){
        talking_udp(server_sock,pid,buffer, client_addr);
    }
    else{  
        runprogram(pid, mode, server_sock, server_sock, exe, arg);
    }
    
    return 0;
}

int create_udp_client(int port, char mode, const char *server_ip, int timeout)
{
    int client_sock;
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    client_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (client_sock < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(server_ip);

    sendto(client_sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if(timeout > 0){
        alarm(timeout);
    }
    
    memset(buffer, 0, BUFFER_SIZE);
    talking_udp(client_sock,pid,buffer, server_addr);
    return 0;
}

int main(int argc, char *argv[]) {
    bool isServer = false;
    int flag = 1;
    // char *execArgs = NULL;
    char mode = 0;
    // int type ;
    int port = 0;
    char *exe ;
    char *arg ;

    int timeout = -1;
    int opt;
    while ((opt = getopt(argc, argv, "e:iobt:")) != -1) {
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
            case 't':
                puts("has t");
                timeout = atoi(optarg);
                if(timeout <= 0){
                    fprintf(stderr, "Timeout must be a positive integer\n");
                    exit(EXIT_FAILURE);
                }
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
            create_server(mode, port, exe, arg, flag);
        }
        if(strstr(argv[i],"TCPC")){
            puts("hello man 2");
            isServer = false;
            l= strtok(argv[i],"C");
            port = atoi(strtok(NULL,"C"));
            port = atoi(strtok(NULL,"C"));
            create_client(port, mode, "127.0.0.1");
        }
        if(strstr(argv[i],"UDPS")){
            puts("hello man 3");
            l = strtok(argv[i],"S");
            port = atoi(strtok(NULL,"S"));
            create_udp_server(mode, port, exe, arg, flag, timeout);
        }
        if(strstr(argv[i],"UDPC")){
            puts("hello man 4");
            l = strtok(argv[i],"C");
            port = atoi(strtok(NULL,"C"));
            create_udp_client(port,mode, "127.0.0.1", timeout);
        }
    }
    return 0;
}