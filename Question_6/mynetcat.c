#include <sys/un.h>
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
    printf("exe:%s\n", exe);
    if(chmod(exe, 0755) != 0){
        perror("chmod failed");
        exit(EXIT_FAILURE);
    }
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

void talking_udsd(int client_sock,pid_t pid,char* buffer, struct sockaddr_un server_addr)
{
    if (pid == 0) { // Child process: read from server
        while (1) {
            ssize_t bytes_read = recvfrom(client_sock, buffer, BUFFER_SIZE,0,NULL, 0);
            if (bytes_read <= 0) {
                perror("read failed");
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
            printf("%s\n",server_addr.sun_path);
            if(sendto(client_sock, buffer, strlen(buffer), 0, (struct sockaddr *)&server_addr, sizeof(server_addr)) <= 0){
                perror("sendto failed");
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
int create_server(char mode, int port, const char *exe, char *arg,int flag)
{
    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    char buffer[BUFFER_SIZE];

    if (strcmp(exe,"ttt") != 0){
        puts(exe);
        perror("exe is not ttt sadge");
        exit(EXIT_FAILURE);
    }
    exe = "../Question_1/ttt";

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

    if (strcmp(exe,"ttt") != 0){
        puts(exe);
        perror("exe is not ttt sadge");
        exit(EXIT_FAILURE);
    }
    exe = "../Question_1/ttt";

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

int create_uds_datagram_server(char mode, char *exe, const char* path, const char *arg, int run, int timeout)
{
    int server_sock;
    struct sockaddr_un server_addr, client_addr;
    char buffer[BUFFER_SIZE];


    if (strcmp(exe,"ttt") != 0){
        puts(exe);
        perror("exe is not ttt sadge");
        exit(EXIT_FAILURE);
    }

    server_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (server_sock < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    int optval = 1;

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, path);
    unlink(server_addr.sun_path);
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
   
    exe = "../Question_1/ttt";
    socklen_t client_addr_len = sizeof(client_addr);
    size_t bytes = recvfrom(server_sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&client_addr, &client_addr_len);
    printf("Received from client: %s\n", buffer);
    
    printf("lo nahon %s\n", client_addr.sun_path);

    pid_t pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if(timeout > 0){
        alarm(timeout);
    }
    if(run){
        talking_udsd(server_sock,pid,buffer, client_addr);
    }
    else{  
        runprogram(pid, mode, server_sock, server_sock, exe, arg);
    }
    
    return 0;
}

int create_uds_datagram_client(char mode, const char *path, int timeout)
{
    int client_sock;
    struct sockaddr_un server_addr;
    char buffer[BUFFER_SIZE];
    strcpy(buffer, "barasi");
    client_sock = socket(AF_UNIX, SOCK_DGRAM, 0);
    if (client_sock < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sun_family = AF_UNIX;
    strcpy(server_addr.sun_path, path);

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
    talking_udsd(client_sock,pid,buffer, server_addr);
    return 0;
}

int create_uds_stream_server(char mode, char *exe, const char *arg, int flag, const char *path)
{   
    struct sockaddr_un server, client;
    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));
    server.sun_family = AF_UNIX;
    client.sun_family = AF_UNIX;
    strcpy(server.sun_path, path);
    server.sun_path[strlen(path)] = '\0';
    int server_sock, client_sock;
    char buffer[BUFFER_SIZE];
    server_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_sock < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    unlink(server.sun_path);
    int len = strlen(server.sun_path) + sizeof(server.sun_family);
    printf("%s\n",server.sun_path);
    if (bind(server_sock, (struct sockaddr *)&server, len) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if(listen(server_sock, 1) < 0){
        perror("listen failed");
        exit(EXIT_FAILURE);
    }
    socklen_t client_addr_size = sizeof(client);
    client_sock = accept(server_sock, (struct sockaddr *)&client, &client_addr_size);
    if(client_sock < 0)
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

int create_uds_stream_client(char mode, const char *path)
{

    int client_sock;
    struct sockaddr_un server_addr = {
        .sun_family = AF_UNIX
    };
    char buffer[BUFFER_SIZE];
    client_sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sock < 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    strcpy(server_addr.sun_path, path);
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

// ./mync -e "ttt 123456789" -i UDSSD

int main(int argc, char *argv[]) {
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
    // printf("%d\n",argc);
    char *l;
    for (int i = 0; i < argc; i++) {
        if (strstr(argv[i], "ttt")) {
            if(i < argc -1)
            {
                exe = strtok(argv[i]," ");
                arg = strtok(NULL," ");
            }
            
            // printf("%s bla \n",exe);
            
        }
        if(strstr(argv[i],"TCPS")){
            puts("hello man");
            l= strtok(argv[i],"S");
            port = atoi(strtok(NULL,"S"));
            create_server(mode, port, exe, arg, flag);
        }
        if(strstr(argv[i],"TCPC")){
            puts("hello man 2");
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
        if(strstr(argv[i],"UDSSD")){
            puts("hello man 5");
            char* path = strtok(argv[i],"D");
            path += 5;
            create_uds_datagram_server(mode, exe, path, arg, flag, timeout);
        }
        if(strstr(argv[i],"UDSCD")){
            puts("hello man 6");
            char* path = strtok(argv[i],"D");
            path += 5;
            create_uds_datagram_client(mode, path, timeout);
        }
        if(strstr(argv[i],"UDSSS")){
            puts("hello man 7");
            char *path = strtok(argv[i],"SSS");
            path += 5;
            printf("%s\n",exe);
            printf("%s\n",path);
            create_uds_stream_server(mode, path, arg, flag, path);
        }
        if(strstr(argv[i],"UDSCS")){
            puts("hello man 8");
            char *path = strtok(argv[i],"SCS");
            path += 5;
            printf("%s\n",path);
            create_uds_stream_client(mode, path);
        }
    }
    return 0;
}

// int main(int argc, char *argv[])
// {   
//     unsigned int s, s2;
//     int len;
//     // create the address struct for socket
//     struct sockaddr_un remote, local = {
//         .sun_family = AF_UNIX,
//         // .sun_path = "/tmp/mynetcat"
//     };
//     strcpy(local.sun_path, "../Question_1/ttt");
//     unlink(local.sun_path);
//     len = strlen(local.sun_path) + sizeof(local.sun_family);
//     // open the socket
//     s = socket(AF_UNIX, SOCK_STREAM, 0);
//     if (s == -1)
//     {
//         perror("socket");
//         return 1;
//     }
//     // bind the socket to the address
//     if(bind(s, (struct sockaddr *)&local, len) == -1){
//         perror("bind");
//         return 1;
//     }

//     // listen for incoming connections
//     listen(s, 1);

//     // accept connection
//     len = sizeof(remote);
//     // remote will store the valuse of the client
//     s2 = accept(s, (struct sockaddr *)&remote, &len);


//     return 0;
// }