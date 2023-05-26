#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <wait.h>

void timer(int sig) {
	printf("\nTimer went off\n");
	pid_t pid = getpid();
	kill(pid + 1, SIGTERM);
}

volatile sig_atomic_t timer_set = 1;

void handler(int sig) {
	printf("\nProgramme A received signal from B\n");
	timer_set = 0;
}

void handler2(int sig) {
	sleep(1);
	printf("Programme B finished before timer went off\n");
	printf("Programme A finished with success\n");
	exit(0);
}

int main (int argc, char* argv[]) {
	int status = 0;
	pid_t child_pid, tp;
	child_pid = fork();
	if (child_pid < 0) {
		printf("Error in creating a child\n");
		return 1;
	}
	else if (child_pid == 0) {
		char buf[10];
		tp = getppid();
		sprintf(buf, "%d", tp);
		char *cmd[] = {"./B", buf, NULL};
		execvp(cmd[0], cmd);
	} else {
		signal(SIGUSR1, handler);
		signal(SIGALRM, timer);
		signal(SIGUSR2, handler2);
		pause();
		if (timer_set == 0) {
			printf("Timer for 10 s set\n\n");
			alarm(10);
		}
		sleep(120);
		kill(child_pid, SIGTERM);
		wait(&status);
		 
	}
	printf("Programme A ended\n");
	return 0;
}