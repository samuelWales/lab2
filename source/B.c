#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_ARGUMENTS 256
#define MAX_COMMANDS_AMOUNT 16

pid_t tp;

void termination_func(int sig) {
	printf("B terminated by A\n");
	kill(0, SIGTERM);
	exit(0);
}

int main(int argc, char* argv[]) {
	tp = getpid();
	char* cmd[MAX_COMMANDS_AMOUNT][MAX_ARGUMENTS];
	char input_buf[256];
	int cmd_counter = 0;
	signal(SIGTERM, termination_func);
	printf("\nType a command:\n\n");
	fgets(input_buf, sizeof(input_buf), stdin);	
	
	if (argc > 1) {
	        int parent = atoi(argv[1]);
		
	        if (parent <= 0) {
		        printf("Atoi error\n");
		} else {
			kill(parent, SIGUSR1);
		}
       	}

	cmd[cmd_counter][0] = strtok(input_buf, "|");
	while (cmd[cmd_counter][0] != NULL) {
		cmd_counter++;
		cmd[cmd_counter][0] = strtok(NULL, "|");
	}

	for (int i = 0; i < cmd_counter; i++) {
		char* arg = strtok(cmd[i][0], " \n");
		int arg_counter = 0;
		while (arg != NULL) {
			cmd[i][arg_counter] = arg;
			arg_counter++;
			arg = strtok(NULL, " \n");
		}
	}
	
	int fieldes[cmd_counter-1][2];
	for (int  i = 0; i < cmd_counter - 1; i++) {
		pipe(fieldes[i]);
	}
	
	for (int i = 0; i < cmd_counter; i++) {
		pid_t child_pid = fork();
		if (child_pid < 0) printf("Error while forking\n");
		else if (child_pid == 0) {
			if (i==0) dup2(fieldes[i][1], STDOUT_FILENO);
			else if (i == cmd_counter - 1) dup2(fieldes[i - 1][0], STDIN_FILENO);
			else {
				dup2(fieldes[i - 1][0], STDIN_FILENO);
				dup2(fieldes[i][1], STDOUT_FILENO);
			}
			for (int j = 0; j < cmd_counter-1; j++) {
				close(fieldes[j][0]);
				close(fieldes[j][1]);
			}
			execvp(cmd[i][0], cmd[i]);
			
		}
		
	}

	for (int i = 0; i < cmd_counter - 1; i++) {
		close(fieldes[i][0]);
		close(fieldes[i][1]);

	}
	wait(NULL);
	kill(getppid(), SIGUSR2);
	return 0;
}