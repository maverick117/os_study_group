#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>


#ifndef ARG_MAX_LEN
  #define ARG_MAX_LEN 80
#endif

char** read_command() {
    char* arg = malloc(ARG_MAX_LEN*sizeof(char));
    char** cmd = malloc(0);
    int len = 0;
    int num = 0;
    for (;;) {
        char c = getchar();
        if (c == (char)255) exit(0);
        else if ((c == ' ' || c == '\n') && len != 0) {
            arg[len] = '\0';
            len = 0;
            cmd = realloc(cmd, (num+1)*sizeof(char*));
            cmd[num++] = arg;
            if (c == '\n') break;
            else arg = malloc(ARG_MAX_LEN*sizeof(char));
        }
        else if ((c == '\n' || c == ' ') && len == 0) {
            if (c == '\n') break;
            continue;
        }
        else arg[len++] = c;
    }
    return cmd;
}

int main() {
    int should_run = 1; /* flag to determine when to exit program */
    while (should_run) {
        printf("ccsh> ");
        fflush(stdout);
        
        char** cmd = read_command();
        pid_t pid = fork();
        if (pid < 0) exit(1);
        else if (pid > 0) {
            int status;
            wait(&status);
        }/* Parent part */
        else {
            execvp(cmd[0], cmd);
            exit(0);
        }/* Child part */
        free(cmd);
    }
    return 0;
}