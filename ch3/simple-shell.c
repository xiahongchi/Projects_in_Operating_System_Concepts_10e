/**
 * Simple shell interface program.
 *
 * Operating System Concepts - Tenth Edition
 * Copyright John Wiley & Sons - 2018
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_LINE 80 /* 80 chars per line, per command */
#define MAX_FNAME 40
#define DEF_MODE S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH

int parseline(char buf[], char *args[]);
int redirect(char *args[], char file[]);
int testpipe(char *args[], char ***p_end);

int main(void)
{
	char *args[MAX_LINE / 2 + 1]; /* command line (of 80) has max of 40 arguments */
	int should_run = 1;

	char history_buf[MAX_LINE];
	history_buf[0] = '\0';

	while (should_run)
	{
		char buf[MAX_LINE], buf_bk[MAX_LINE];
		int bg;
		int rd;
		int pp;
		char file[MAX_FNAME];
		char **p_end;
		pid_t pid;

		printf("osh>");
		fflush(stdout);
		fflush(stdin);
		args[0] = NULL;

		/**
		 * After reading user input, the steps are:
		 * (1) fork a child process
		 * (2) the child process will invoke execvp()
		 * (3) if command included &, parent will invoke wait()
		 */
		if (fgets(buf, MAX_LINE, stdin) == 0)
		{
			should_run = 0;
			continue;
		}
		strcpy(buf_bk, buf);
		bg = parseline(buf, args);
		if (strcmp(args[0], "exit") == 0)
		{
			should_run = 0;
			continue;
		}
		if (strcmp(args[0], "!!") == 0)
		{
			if (history_buf[0] == '\0')
			{
				printf("No commands in history\n");
				continue;
			}
			else
			{
				strcpy(buf, history_buf);
				bg = parseline(buf, args);
			}
		}
		else
		{
			strcpy(history_buf, buf_bk);
		}
		rd = redirect(args, file);
		pp = testpipe(args, &p_end);
		pid = fork();
		if (pid < 0)
		{
			printf("osh: fork error\n");
			exit(1);
		}
		else if (pid == 0)
		{
			if (pp == 1)
			{
				int p[2];
				if (pipe(p) < 0)
				{
					printf("osh: pipe error\n");
					exit(1);
				}
				pid = fork();
				if (pid < 0)
				{
					printf("osh: fork error\n");
					exit(1);
				}
				else if (pid == 0)
				{
					// child write
					if (p[1] != STDOUT_FILENO)
					{
						if (dup2(p[1], STDOUT_FILENO) < 0)
						{
							printf("osh: dup2 error\n");
						}
						if (close(p[1] < 0))
						{
							printf("osh: close error\n");
						}
					}
					if (close(p[0]) < 0)
					{
						printf("osh: dup2 error\n");
					}
					execvp(args[0], args);
					printf("osh: execvp error\n");
				}
				else
				{
					// parent read
					if (p[0] != STDIN_FILENO)
					{
						if (dup2(p[0], STDIN_FILENO) < 0)
						{
							printf("osh: dup2 error\n");
						}
						if (close(p[0]) < 0)
						{
							printf("osh: close error\n");
						}
					}
					if (close(p[1]) < 0)
					{
						printf("osh: dup2 error\n");
					}
					execvp(p_end[0], p_end);
					printf("osh: execvp error\n");
				}
			}
			else
			{
				if (rd == 0)
				{
					int fd;
					if ((fd = open(file, O_RDONLY, 0)) < 0)
					{
						printf("osh: cannot open file %s\n", file);
						exit(1);
					}
					if (dup2(fd, STDIN_FILENO) < 0)
					{
						printf("osh: dup2 error\n");
						exit(1);
					}
				}
				else if (rd == 1)
				{
					int fd;
					if ((fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, DEF_MODE)) < 0)
					{
						printf("osh: cannot open file %s\n", file);
						exit(1);
					}
					if (dup2(fd, STDOUT_FILENO) < 0)
					{
						printf("osh: dup2 error\n");
						exit(1);
					}
				}
				execvp(args[0], args);
				printf("osh: execvp error\n");
				exit(1);
			}
		}
		else
		{
			if (bg == 0)
			{
				if (waitpid(pid, NULL, 0) < 0)
				{
					printf("osh: waitpid error\n");
					exit(1);
				}
			}
		}
	}

	return 0;
}

int parseline(char buf[], char *args[])
{
	int argc = 0;
	char *sp, *p = buf;
	while (*p == ' ')
		p++;
	// suppose there is only one space between args
	while (sp = strchr(p, ' '))
	{
		*sp = '\0';
		args[argc++] = p;
		p = sp + 1;
	}
	sp = strchr(p, '\n');
	*sp = '\0';
	args[argc] = p;
	if (strcmp(args[argc], "&") == 0)
	{
		args[argc] = NULL;
		return 1;
	}
	else
	{
		args[argc + 1] = NULL;
		return 0;
	}
}

int redirect(char *args[], char file[])
{
	int k = 0;
	while (args[k] && strcmp(args[k], "<") != 0 && strcmp(args[k], ">") != 0)
	{
		k++;
	}
	if (args[k])
	{
		if (strcmp(args[k], "<") == 0)
		{
			strcpy(file, args[k + 1]);
			args[k] = NULL;
			return 0;
		}
		else
		{
			strcpy(file, args[k + 1]);
			args[k] = NULL;
			return 1;
		}
	}
	return 2;
}

int testpipe(char *args[], char ***p_end)
{
	int k = 0;
	while (args[k] && strcmp(args[k], "|"))
		k++;
	if (args[k])
	{
		args[k] = NULL;
		*p_end = args + k + 1;
		return 1;
	}
	else
	{
		return 0;
	}
}