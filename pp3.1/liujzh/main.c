#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#ifndef MAXLINE
#define MAXLINE 80
#endif

#ifndef HISTLEN
#define HISTLEN 5
#endif

#ifndef DIRLEN
#define DIRLEN 512
#endif

int signal_interrupt;

void interrupt_handler(int signum){
  signal_interrupt = 1;
}

char getInput(char ** args, int* waitFlag){ /* Function to read input */
  int i = -1, j = 0;
  int blank =1;
  char c;
  *waitFlag = 0;
  memset(args,0,sizeof(char *)*(MAXLINE/2+1));
  while((c = getchar()) != '\n' && c != EOF){
    if(signal_interrupt){
      signal_interrupt = 0;
      
    }
    if(c == '&'){         /* Put child process in background and continue */
      *waitFlag = 1;
      continue;
    }
    if (c == ' ' || c == '\t' || c == '\n'){
      blank = 1;          /* White spaces */
      continue;
    }
    else{
      if (blank == 1){    /* Command Characters */
        blank = 0;
        i++;
        j = 0;
        args[i] = (char*) malloc(sizeof(char)*MAXLINE);
        memset(args[i],0,sizeof(char)*MAXLINE);
      }
      args[i][j++] = c;
    }
  }
  if(c == '\n')
    return (char)0;
  else
    return c;
}

int commitFork(char ** args, int waitFlag, int *status){
  pid_t pid = fork();           /* fork child */
  if(pid < 0){            /* If forking failed then exit */
    printf("Forking failed. Exiting...\n");
    exit(127);
  }
  else if(pid == 0){      /* If the current process is the Child Process */
    exit(execvp(args[0],args));
  }
  else{
    if(!waitFlag){        /* Parent process */
      waitpid(pid,status,0);
    }
    else{
      printf("%s PID: %d\n",args[0],pid); 
    }
  }
  if (*status == 65280){
    printf("osh: Command \'%s\' not found.\n",args[0]);
    *status = 127;
  }
  return 0;
}

void waitOthers(){
  int wstatus = 0;          /* Status flag for waiting processes */
  pid_t pid;
  while(1){               /* Wait to see if any background processes have exited */
    pid = waitpid(-1,&wstatus,WNOHANG);
    if(pid == 0 || pid == -1) break;
    else{ 
      if (wstatus != 0) printf("%d ",wstatus);
      wstatus = 0;
      printf("PID: %d finished.\n",pid);
    }
  }
}

void updateHistory(char** args, char *** cmdhist, int comcnt){
  int i;
  int index = comcnt%HISTLEN;
  if(cmdhist[index] == NULL){
    cmdhist[index] = malloc(sizeof(char*)*(MAXLINE/2+1));
    for(i = 0; args[i] != NULL; i++)
      cmdhist[index][i] = args[i];
  }
  else{
    i = 0;
    while(cmdhist[index][i] != NULL)
      free(cmdhist[index][i++]);
    memset(cmdhist[index],0,sizeof(char*)*(MAXLINE/2+1));
    for(i = 0; args[i] != NULL; i++)
      cmdhist[index][i] = args[i];
  }
}

void printHistory(char *** cmdhist, int comcnt){
  int i = 0, j = 0;
  for(i = comcnt - 4; i <= comcnt; i++){
    if(i <= 0) continue;
    printf("%d  ",i);
    j = 0;
    while(cmdhist[i%HISTLEN][j] != NULL)
      printf("%s ",cmdhist[i%HISTLEN][j++]);
    printf("\n");
  }
}

void clearargs(char ** args){
  int i;
  for(i = 0; args[i] != NULL; i++)
    free(args[i]);
}


int main(){
  char *args[MAXLINE/2+1];  /* Command argument list */
  char **cmdhist[HISTLEN];  /* Command history buffer */
  int should_run = 1;       /* Flag for continuing */
  int i = 0, j = 0;         /* Loop indicators */
  char c;                   /* char for holding input character */
  int waitFlag = 0;         /* Flag indicating whether to wait for child process */
  int status = 0;           /* Status flag for process */
  int comcnt = 0;
  int valid = 0;
  char currentdir[DIRLEN];
  memset(cmdhist,0,sizeof(char**)*HISTLEN); /* Initialize all command history lines to zero */
  signal_interrupt = 0;
  if(signal(SIGINT,interrupt_handler) == SIG_ERR){
    fprintf(stderr,"Signal Handler Setup Error. Exiting.\n");
    exit(1);
  }
  while(should_run){
    
    getcwd(currentdir,DIRLEN);

    if(status == 0)         /* No error */
      printf("osh: %s > ",currentdir);
    else{                   /* Print out error code */
      printf("%d osh: %s >",status,currentdir);
      status = 0;
    }
    fflush(stdout);         /* Flush stdout pipe */

    c = getInput(args,&waitFlag); /* Parse input */

    if (c == 0 && args[0] == NULL) {
      waitOthers();
      continue; /* No commands typed */ 
    }
    if (c == EOF) {
      exit(1);  /* If end of file reached then return */
    }
    if (strcmp(args[0],"history") == 0 && args[1] == NULL){
      printHistory(cmdhist,comcnt);
      clearargs(args);
      waitOthers();
      continue;
    }
    else if(strcmp(args[0],"cd") == 0){
      if(args[1] != NULL && args[2] == NULL){
        if(chdir(args[1]) == -1)
          printf("osh: change directory to %s failed.\n",args[1]); 
      }
      else{
        printf("Invalid argument with command \'cd\'.\n");
      }
      waitOthers();
      updateHistory(args,cmdhist,comcnt);
      continue;
    }
    else if(strcmp(args[0], "!!") == 0 && args[1] == NULL){
      commitFork(cmdhist[comcnt%HISTLEN],waitFlag,&status);
      clearargs(args);
      waitOthers();
      continue;
    }
    else if(args[0][0] == '!' && args[1] == NULL){
      i = 1;
      j = 0;
      valid = 1;
      while(args[0][i] != '\0'){
        if(args[0][i] < 48 || args[0][i] > 57){
          printf("Error with non-numerical character \'%c\'\n",args[0][i]);
          valid = 0;
        }
        j = j*10+args[0][i]-48;
        i++;
      }
      if(valid){
        if(j >0 && j >= comcnt - 4 && j <= comcnt){
          commitFork(cmdhist[j%HISTLEN],0,&status);
        }
        else{
          printf("Not in valid region: %d\n",j);
        }
      }
      clearargs(args);
      waitOthers();
      continue;
    }
    else if(strcmp(args[0], "exit") == 0){
      clearargs(args);
      exit(0);
    }

    comcnt++; /* Valid command, count += 1 */
    commitFork(args,waitFlag,&status); /* Commit the fork() */
    waitOthers(); /* Wait for other background processes to exit */
    updateHistory(args,cmdhist,comcnt);
  }  
  return 0;
}
