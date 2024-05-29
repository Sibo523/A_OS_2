#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h> // Include the <unistd.h> header file
#include <string.h>
#include <sys/types.h> // Include the <sys/types.h> header file
#include <sys/wait.h>
 
//we need to know what includesare good before submitting

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Usage: ./%s -e 'ttt 123456789'\n", argv[0]);
        return -1;
    }
    // Parse the arguments
    char *exe = strtok(argv[2], " ");
    char *arg = strtok(NULL, " "); // Use NULL instead of 0 for subsequent tokens

    // Prepare the path to the executable
    char *path = "../Question_1/ttt";
    
    if (!strcmp("ttt",exe)){
        exe = path;
    }
    else{
        printf("Invalid executable\n");
        return -1;
    }

    pid_t pid = fork();
    if(pid < 0){
        perror("fork failed");
        exit(EXIT_FAILURE);
    }
    if (pid == 0)
    {
        fflush(stdin);
        execlp(exe, exe, arg, NULL);
        perror("Execution of path failed");
        exit(EXIT_FAILURE);
    }

    int status;
    waitpid(pid, &status, 0);
    return 0;
}