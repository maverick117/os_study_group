#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>

#define MAX_LINE 80 /* The maximum length command*/
#define HISTORY_LENGTH 10


typedef struct
{
	char* data[HISTORY_LENGTH];
	int count;
} histlist;

void histlist_init(histlist*);
void histlist_push(histlist*, char*);
int histlist_get(histlist*, int, char*);
void histlist_traverse(histlist*);



void histlist_init(histlist* l){
	for (int i = 0; i < HISTORY_LENGTH; ++i)
		l->data[i] = NULL;
	l->count = 0;
}

void histlist_push(histlist* l, char* command){
	if (l->data[0] != NULL) 
		free(l->data[0]);
	memcpy(&l->data[0], &l->data[1], sizeof(char*)*(HISTORY_LENGTH-1));
	l->data[HISTORY_LENGTH-1] = malloc(sizeof(char*)*(strlen(command)+1));
	strcpy(l->data[HISTORY_LENGTH-1], command);
	if (l->count < 10) l->count++;
}

int histlist_get(histlist* l, int index, char* command){ /* index starts from 1 */
	if (l->count==0){
		fprintf(stderr, "No commands in history.\n");
		command[0] = '\0';
		return 1;
	}
	else if (index < 1 || index > l->count){
		fprintf(stderr, "No such command in history: %d\n", index);
		command[0] = '\0';
		return 1;
	}
	index = HISTORY_LENGTH-index;
	strcpy(command, l->data[index]);
	return 0;
}

void histlist_traverse(histlist* l){
	if (l->count==0){
		printf("No commands in history.\n");
		return;
	}
	for (int i = l->count; i >=1 ; --i){
		char command[MAX_LINE];
		histlist_get(l, i, command);
		printf("%d %s\n", i,command);
	}
}

int read_command(char** args, int* should_wait, char* hist_command, histlist* h){ /* return (# of args) + 1 */
	char command[MAX_LINE];
	int k = -1;

	if (hist_command != NULL){
		strcpy(command, hist_command);
	} else {
		for (int i = 0; i < MAX_LINE/2 + 1; ++i){ 
			args[i] = NULL; /* initialize command line arguments */
		}
		for (int i = 0; i < MAX_LINE; ++i){ /* read from stdin */
			char c = getchar();
			if (c == 0xA) {
				command[i] = '\0';
				break;
			}
			command[i] = c;
		}
	}

	char* p = strtok(command, " ");
	while(p != NULL){
		args[++k]=(char*)malloc(sizeof(char)*(strlen(p)+1));
		strcpy(args[k], p);
		p = strtok(NULL, " ");
	}

	if (k==-1) return k; /* empty input */

	if (args[k][strlen(args[k])-1]=='&'){
		if (strlen(args[k])==1){ /* ls & */
			free(args[k]);
			args[k] = NULL;
		} else { /* ls& */
			args[k][strlen(args[k])-1] = '\0';
		}
		*should_wait = 1;
	}
	return k;
}



int main(int argc, char const *argv[])
{
	char* args[MAX_LINE/2 + 1]; /* command line arguments */
	int should_run = 1;  /* flag to determine exit program */
	histlist history;
	histlist_init(&history);
	while (should_run){
		printf("znsh>");
		fflush(stdout);

		int should_wait = 0;
		int should_exec = 1;
		if (read_command(args, &should_wait, NULL, &history)==-1) continue;

		if (strcmp(args[0], "exit")==0){
			should_run = 0;
			should_exec = 0;
		} else if (strcmp(args[0], "history")==0){
			histlist_traverse(&history);
			should_exec = 0;
		} else if (args[0][0] == '!'){
			char command[MAX_LINE];
			if (args[0][1] == '!'){
				if (histlist_get(&history, 1, command)==1)
					should_exec = 0;
				else
					read_command(args, &should_wait, command, &history);
			} else {
				int isalldigit = 1;
				for (char* ch = &args[0][1]; *ch!='\0'; ++ch)
					if (!isdigit(*ch)){
						isalldigit = 0;
						break;
					}
				if (isalldigit && args[0][1]!='\0')
					if (histlist_get(&history, atoi(&args[0][1]), command)==1)
						should_exec = 0;
					else
					{
						read_command(args, &should_wait, command, &history);
					}
				else {
					fprintf(stderr, "Invalid argument: %s\n", &args[0][1]);
					should_exec = 0;
				}
			}
		}

		char command[80] = "";
		for (int i = 0; i < MAX_LINE/2+1; ++i){
			if(args[i] != NULL){
				strcat(command, args[i]);
				strcat(command, " ");
			}
			else
				break;
		}
		histlist_push(&history, command);

		if (should_exec){
			pid_t pid = fork();
			if (pid < 0){
				fprintf(stderr, "Fail to fork.\n");
			} else if (pid == 0) { /* child process */
				if (execvp(args[0], args)==-1){
					fprintf(stderr, "Command not found: %s\n", args[0]);
					// printf("Command not found: %s\n", args[0]);
					exit(1);
				};
			} else { /* parent process*/
				if (should_wait){
					printf("I have waited...\n");
					int status;
					wait(&status);
					printf("Continue...\n");
				}
			}
		}

		for (int i = 0; i < MAX_LINE/2+1; ++i){
			if(args[i] != NULL){
				free(args[i]);
				args[i] = NULL;
			}else
				break;
		}
	}


	return 0;
}