#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

int read_cmd(char*** cmd) { 
    char* arg = NULL;
    int num = 0;
    int len = 0;
    for (;;) {
        char c = getchar();
        if (c == (char)255) exit(0);
        else if ((c == '\n' || c == ' ') && len != 0) {
            arg = realloc(arg, sizeof(char)*(len+1));
            arg[len] = '\0';
            *cmd = realloc(*cmd, sizeof(char*)*(++num));
            *cmd[num-1] = arg;
            if (c == '\n') break;
            arg = NULL;
            len = 0;
        }
        else if ((c == '\n' || c == ' ') && len == 0) {
            free(arg);
            if (c == '\n') break;
            continue;
        }
        else {
            arg = realloc(arg, sizeof(char)*(len+1));
            arg[len++] = c;
        }
    }
    return num;
}

void free_cmd_mem(char*** cmd, int seg_count) {
    for (int i=0; i<seg_count; i++) {
        free(*cmd[i]);
    }
    free(*cmd);
    *cmd = NULL;
}

int main() {
    int should_run = 1; /* flag to determine when to exit program */
    while (should_run) {
        printf("ccsh> ");
        fflush(stdout);
        
        char** cmd = NULL;
        int seg_count = read_cmd(&cmd);
        printf("%s\n", cmd[0]);
        
        pid_t pid = fork();
        if (pid < 0) exit(1);
        else if (pid > 0) {
            int status;
            waitpid(pid, &status, 0);
        }/* Parent part */
        else {
            fflush(stdout);
            execvp(cmd[0], cmd);
            exit(0);
        }/* Child part */
        free_cmd_mem(&cmd, seg_count);
    }
    return 0;
}