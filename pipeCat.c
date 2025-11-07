// AUTHOR: framrt
#define _GNU_SOURCE
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

//handling user exception with large file
#define CHAR_BUFFER 1000
#if 1
    #define HANDLE_INT
#endif

#ifdef HANDLE_INT
void handle_int(int sig){
    printf("INTERRUPT WITH CTRL + C\n");
    _exit(1);
}
#endif

// Returns 1 if .txt extension exists, 0 if it was added
//18 -> normal comparison between .txt and the index where tha last dot has been found
int check_and_add_txt_extension(char *filename) {
    char *dotIndex = strrchr(filename, '.');
    if (dotIndex != NULL && strcmp(dotIndex, ".txt") == 0) {
        return 1; // Extension already exists
    }
    strcat(filename, ".txt");
    return 0; // Extension was added
}

////////////CAT WITH FILEINFO/////////////
int main(int argc, char *argv[]){
    if(argc < 2){
        perror("no file inserted\n");
        exit(EXIT_FAILURE);
    }
    struct sigaction saI; 
    printf("My PID is: %d\n", getpid());
    //declaration of a pipe file descriptor
    int pipefd[2];
    char filename[256];
    printf("input file from user: %s" , argv[0]);
    strncpy(filename, argv[1], sizeof(filename) - 5); // Leave room for ".txt" .txt is 4 + /0 character
    filename[sizeof(filename) - 5] = '\0';
    
    int result = check_and_add_txt_extension(filename);
    
    if (result == 1) {
        printf("File already has .txt extension: %s\n", filename);
    } else {
        printf("Added .txt extension: %s\n", filename);
    }   
    
    FILE *f1 = fopen(filename, "r");
    if(f1 == NULL){
        perror("failed to open the file\n");
        exit(EXIT_FAILURE);
    }
    pid_t cpid; 
    if(pipe(pipefd) == -1 ) {
        perror("pipe creation failed");
        exit(EXIT_FAILURE); 
    }
    cpid = fork();
#ifdef HANDLE_INT 
    saI.sa_handler = handle_int;    
    memset(&saI, 0, sizeof(saI)); //setting the saI memory area to 0
    sigaction(SIGINT, &saI, NULL);
#endif
    if(cpid == -1){
        perror("fork failure"); 
        exit(EXIT_FAILURE);
    }//child code start
    if(cpid == 0){ //child can read
        printf("child pid: %d\n", getppid());
        close(pipefd[0]);
        char buf[CHAR_BUFFER]; 
        ssize_t byte_reads; 
        while((byte_reads = fread(buf, 1, sizeof(buf), f1)) > 0 ){
            write(pipefd[1], buf, byte_reads);
        }
        fclose(f1); //closing file
        close(pipefd[1]); // Close write end when done (pipe channel end)
        exit(EXIT_SUCCESS);
    }
    else{
        //parent code
        close(pipefd[1]);
        char buf[CHAR_BUFFER];
        int read_count = 0;
        ssize_t byte_read = 0; 
        int total_bytes = 0; 
        int file_total = 0; 
        printf("parent reciving: \n");
       
        while(1){ //necessary to read until EOF
            byte_read = read(pipefd[0], buf, sizeof(buf));   
            if(byte_read > 0){ 
                // Successfully read data
                read_count++;
                total_bytes += byte_read;
                printf("[Read #%d: %zd bytes] ", read_count, byte_read);//line information
                write(STDOUT_FILENO, buf, byte_read);
                printf("\n");
            }
            else if(byte_read == 0){
                // EOF - child closed the write end
                printf("===========================================\n");
                printf("Parent: Pipe closed by child (EOF reached)\n");
                break;
            }
            else{
                // Error on reading a line
                perror("read error");
                break;
            }
            file_total += total_bytes; 
        }
        printf("\n[TOTAL BYTES FILE: %zu]\n", file_total);
        close(pipefd[0]);
        wait(NULL);
    }


    return EXIT_SUCCESS;
}
