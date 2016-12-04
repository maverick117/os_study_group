// ---------------------------------------------------------
// Rong's Shell
// OS self-learning group
// All done by DataCorrutped (rongyy@shanghaitech.edu.cn)
// Dec 4, 2016
// ---------------------------------------------------------
// C programming is hard, not like C++.
// But this shell is really fun.
// Can't change work directry for now.
// Also a bunch of un-finished things,
// but no bugs. (I hope)
// ---------------------------------------------------------

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define MAXLINE 80
#define MAXHIST 10

char** Read(int* check_wait){
	char* word = malloc(MAXLINE*sizeof(char));
	int word_length = 0;
	char** command_line = malloc(0);
	int word_count = 0;
	char c;
	while (1){
		c = getchar();
		if (c == 255) {exit(0); }
		if (c == '&') {*check_wait = 1; }
		if ((c==' ' ) || (c=='\t') || 
			(c=='\r') || (c=='\n')) {
			if (word_length==0){
				// In case we have double or more
				// space or something.
				if (c=='\n') {break; }
				continue;
			}
			// Refresh.
			word[word_length] = '\0';
			word_length = 0;
			command_line[word_count] = word;
			word_count ++;
			word = malloc(MAXLINE*sizeof(char));
			if (c=='\n') {break;}
		} else {
			// Or simply add.
			word[word_length] = c;
			word_length++;
		}
	}
	return command_line;
}

int main(){
	int hist_count=0;
	char** history[MAXHIST];
	char** command_line;
	int shouldrun = 1;
	int check_wait = 0;
	pid_t pid;

	memset(history ,0 , sizeof(char**)*MAXHIST);

	while (shouldrun){

		printf("Rong's> ");
		fflush(stdout);

		// Input
		check_wait = 0;
		command_line = Read(&check_wait);

		// History
		if (strcmp(command_line[0], "history") == 0){
		// This part is stupid too,
		// I shall seek for some change later.
			int i;
			if (hist_count<10){
				for (i=0; i<hist_count; i++){
					printf("%s\n", history[i][0]);
				}
			} else {
				for (i=0; i<10; i++){
					printf("%s\n", history[hist_count % 10][0]);
					hist_count ++;
				}
				hist_count -= 10;
			}
		} else if (strcmp(command_line[0], "!!") == 0){
			// Thus there will be no !! in history,
			// I simply replace it with the last command,
			// and the last command will be pushed into
			// the history again.
			command_line = history[hist_count % 10 -1];
		}
		history[hist_count % 10] = command_line;
		hist_count ++ ;
	// The problem here is I can't run !3.
	// I think I should put fork part in a function
	// Like jianzhong did.
	// So I can iterate over the last serval commands.

		// Fork
		pid = fork();
		if (pid == 0 ){
			execvp(command_line[0], command_line);
			exit(0);
		} else if (pid > 0){
			wait();
		} else if (pid < 0){
			printf("Error!\n");
			return 1;
		}
	}
	return 0;
}
