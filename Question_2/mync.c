#include <stdlib.h>

#include <unistd.h> // Include the <unistd.h> header file

int main(int argc, char *argv[]){
    if (argc != 4) {
        printf("Usage: ./%s -e 'ttt 123456789'\n", argv[0]);
        return -1;
    }
    int inputPipe[2], filterPipe[2], intermediatePipe[2], transformPipe[2];

    // Setup the first pipe
    if (pipe(inputPipe) == -1) {
        perror("Failed to create inputPipe");

        exit(EXIT_FAILURE);
    }

    // Create the first child process to read the phonebook
    if (fork() == 0) {
        // Redirect stdout to inputPipe's write-end
        close(inputPipe[0]); // Close unused read-end

        dup2(inputPipe[1], STDOUT_FILENO);
        close(inputPipe[1]); // Close the write-end after duplication

        // Execute 'cat' to read the phonebook
        execlp("cat", "cat", "phonebook.txt", NULL);
        perror("Execution of 'cat' failed");
        exit(EXIT_FAILURE);
    }
    return 0;
}