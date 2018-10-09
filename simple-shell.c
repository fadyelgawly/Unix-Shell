#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#define MAX_LINE		80
#define clear() printf("\033[H\033[J")

int lineToTokens(char line[], char* args[]){
    int argc = 0; char * pch;
    pch = strtok(line," \n\t");
    while (pch != NULL) {
        args[argc++] = pch;
        pch = strtok(NULL, " \n\t");
    }
    if (argc) {
        if (strcmp(args[0], "exit") == 0){
            return 0;
        } else if (strcmp(args[0], "clear") == 0){
            clear(); // prompt is cleared and -1 is returned to main to invoke a continue;
        } else if (strcmp(args[argc-1], "&") == 0) {
            return 1;
        }  else {
            return 2;
        }
    }
    return -1; // argc = 0 or clear was executed
}

int executecommand(pid_t pid, char* args[], int concurrent){
    if (pid < 0) {
        //Error forking
        printf("DEBUG:Fork failed");
        fprintf(stderr, "Fork Failed");
        return -1;
    } else if (pid == 0) {
        //Invoke Child process
        execlp(args[0], *args);
        return 0;
    } else { /* parent process */
        if (concurrent == 0) { /*no &*/
            waitpid(pid, NULL, 0); /*wait for child*/
        }
        return 1;
    }
}

int main(void) {
    char *args[MAX_LINE/2 + 1];	/* command line (of 80) has max of 40 arguments */
    pid_t pid; int should_run = 1, concurrent = 1;
    char line [MAX_LINE];
    while (should_run){
        printf("osh>");
        fflush(stdout);
        if (fgets(line, MAX_LINE, stdin) == NULL) {
            continue;
        }
        switch (lineToTokens(line, args)) {
            case 0:
                return 0;
                break;
            case 1:
                concurrent = 1;
                break;
            case 2:
                concurrent = 0;
                break;
            default:
                break;
        }
        pid = fork();
        switch (executecommand(pid, args, concurrent)) {
            case -1:
                return -1;
                break;
            default:
                break;
        }
        
        
        
    }
    return 0;
}
