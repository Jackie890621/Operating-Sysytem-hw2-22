#define _POSIX_SOURCE
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <signal.h>
#include<sys/ptrace.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/reg.h>

int main(int argc,char **argv)
{
	pid_t child_pid;
	int p_status;
	long syscall;
	FILE *file1;
	FILE *file2;
	char input[20];
	char output[20];
	char temp;

	child_pid = fork();

	if(child_pid == 0) // child process
	{
		ptrace(PTRACE_TRACEME, child_pid, NULL, NULL);
		kill(getpid(), SIGSTOP);
	
		// Implement your file copy code
		while (1) {
			printf("Please input the source file name:\n");
			scanf("%s", input);
			file1 = fopen(input, "r");
			if (file1 == NULL) {
				printf("Fail to open file %s\n", input);
				continue;
			}

			printf("Please input the destination file name:\n");
			scanf("%s", output);
			file2 = fopen(output, "w");
			while ((temp = fgetc(file1)) != EOF) {
				fputc(temp, file2);
			}
			printf("%s has generated\n", output);
			fclose(file1);
			fclose(file2);
			break;
		}
		exit(0);
	}
	else if(child_pid > 0) // parent process
		while(waitpid(child_pid, &p_status, 0))
			if(WIFEXITED(p_status))
				break;
			else
			{
				syscall = ptrace(PTRACE_PEEKUSER, child_pid, sizeof(long)*ORIG_RAX, NULL);
				printf("system call: %ld\n", syscall);
				ptrace(PTRACE_SYSCALL, child_pid, NULL, NULL);
			}
	else
		printf("fork error\n");

	return 0;
}
