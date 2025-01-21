#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

int task1(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab4 1\n");
		return 1;
	}

	//       <---[0]child_pipe[1]--- 
	// parent						 child
	//       ---[1]parent_pipe[0]--> 
	int parent_pipe[2], child_pipe[2];

    pipe(parent_pipe);
    pipe(child_pipe);

	int childpid = fork();
	
	int readfd, writefd;
	char* message;

    if (childpid) {
 		close(parent_pipe[0]);
        close(child_pipe[1]);

		readfd = child_pipe[0];
		writefd = parent_pipe[1];

        message = "tik";
        
		write(writefd, message, strlen(message));
    } else {
        close(parent_pipe[1]);
        close(child_pipe[0]);

		readfd = parent_pipe[0];
		writefd = child_pipe[1];

        message = "tak";
    }

	while (1) {
		char buf[256] = {0};
        read(readfd, buf, sizeof(buf));
		printf("%s\n", buf);

		sleep(1);

		write(writefd, message, strlen(message));
	
		sleep(1);
	}

	close(readfd);
    close(writefd);

    return 0;
}

int task2(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab4 2\n");
		return 1;
	}
	
	int pipefds[2];

    pipe(pipefds);

	int childpid = fork();
	
	int pipefd, writefd;
	char* message;

    if (childpid) {
 		close(pipefds[0]);

		pipefd = pipefds[1];

		while (1) {
			char buf[256] = {0};
			
			int n = read(STDIN_FILENO, buf, sizeof(buf));
			if (n == 0) {
				break;
			}

			write(pipefd, buf, n);
		}
    } else {
        close(pipefds[1]);

		pipefd = pipefds[0];

		fcntl(pipefd, F_SETFL, O_NONBLOCK);

		int stop_iterating = 0;
		int empty_pipe_count = 0;

        while (1) {
			char buf[256] = {0};
			
			int n = read(pipefd, buf, sizeof(buf));
			switch (n) {
				case 0:
					stop_iterating = 1;
				case -1:
					empty_pipe_count++;
					sleep(1);

					if (empty_pipe_count >= 10) {
						stop_iterating = 1;
					}

					break;
				default:
					empty_pipe_count = 0;
			}
			
			if (stop_iterating) {
				printf("child done\n");
				break;
			}

			if (empty_pipe_count) {
				//printf("empty pipe\n");
				continue;
			}

			write(STDOUT_FILENO, buf, n);
		}
    }

	close(pipefd);

	return 0;
}

int task3(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab4 3\n");
		return 1;
	}

	int parent_pipe[2], child_pipe[2];

    pipe(parent_pipe);
    pipe(child_pipe);

	int childpid = fork();
	
	int readfd, writefd;

    if (childpid) {
 		close(parent_pipe[0]);
        close(child_pipe[1]);

		readfd = child_pipe[0];
		writefd = parent_pipe[1];
        
		char buf[256] = {0};
		int n = read(STDIN_FILENO, buf, sizeof(buf));
		write(writefd, buf, n);
    } else {
        close(parent_pipe[1]);
        close(child_pipe[0]);

		readfd = parent_pipe[0];
		writefd = child_pipe[1];
    }

	while (1) {
		char buf[256] = {0};
		int n = read(readfd, buf, sizeof(buf));
		if (n == 0) {
			break;
		}

		write(STDOUT_FILENO, buf, n);

		n = read(STDIN_FILENO, buf, sizeof(buf));
		if (n == 0) {
			break;
		}

		write(writefd, buf, n);
	}

	close(readfd);
    close(writefd);

	return 0;
}

int task4(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab4 4\n");
		return 1;
	}

	int p[2];

    pipe(p);

	int childpid = fork();
	
	int pipefd;

    if (childpid) {
 		close(p[0]);

		pipefd = p[1];
        
		while (1) {
			char buf[256] = {0};
			int n = read(STDIN_FILENO, buf, sizeof(buf));
			if (n == 0) {
				break;
			}
			write(pipefd, buf, n);
		}
    } else {
        close(p[1]);

		pipefd = p[0];

		dup2(pipefd, STDIN_FILENO);

		char* filepath = "/usr/bin/awk";
		char* argv[] = { filepath, "{ print $1 }", NULL };

		execve(filepath, argv, NULL);
	}

	close(pipefd);

	return 0;
}

int task5(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab4 5\n");
		return 1;
	}

	int p[2];

    pipe(p);

	int childpid = fork();
	
	int pipefd;

    if (childpid) {
 		close(p[0]);

		pipefd = p[1];
        
		dup2(pipefd, STDOUT_FILENO);

		char* filepath = "/usr/bin/who";
		char* argv[] = { filepath, NULL };

		execve(filepath, argv, NULL);
    } else {
        close(p[1]);

		pipefd = p[0];

		dup2(pipefd, STDIN_FILENO);

		char* filepath = "/usr/bin/wc";
		char* argv[] = { filepath, "-l", NULL };

		execve(filepath, argv, NULL);
	}

	return 0;
}

int task6(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab4 6\n");
		return 1;
	}
	
	char* fifopath = "lab4.pipe";

	mkfifo(fifopath, 0777);

	int childpid = fork();

	if (childpid) {
		int fd = open(fifopath, O_WRONLY);
		
		char message[] = "lolaboba";
		write(fd, message, sizeof(message));
		
		close(fd);

		return 0;
	}	

	int fd = open(fifopath, O_RDONLY | O_NDELAY);
    
	char buf[256] = {0};
	while (1) {
		int n = read(fd, buf, sizeof(buf));
		if (n == 0) {
			break;
		}
	}

	printf("%s\n", buf);
    
	close(fd);

	return 0;
}

int task789(int argc, char* argv[], long taskno) {
	if (argc != 3) {
		fprintf(stderr, "usage: ./lab4 7 filepath\n");
		return 1;
	}

	char* filepath = argv[2];

	char digits[] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    
	int childpid = fork();

	if (childpid) {
		int fd = open(filepath, O_WRONLY | O_CREAT | O_TRUNC, 0666);
        
		struct flock lock = {0};

		if (taskno == 8 || taskno == 9) {
        	lock.l_type = F_WRLCK;
        	fcntl(fd, F_SETLK, &lock);
		}

		for (int i = 0; i < 200; i++) {
			write(fd, &digits[i % 10], 1);
		}

		if (taskno == 8 || taskno == 9) {
			lock.l_type = F_UNLCK; 
   			fcntl(fd, F_SETLK, &lock);
		}

		close(fd);

		return 0;
    } else {
        int fd = open(filepath, O_RDONLY);

		struct flock lock = {0};

		if (taskno == 8 || taskno == 9) {
			lock.l_type = F_RDLCK;
			fcntl(fd, F_SETLKW, &lock); 
		}

		int counter = 0;

		char buf[1];
        while(1) {
			int n = read(fd, buf, 1);
			if (n <= 0) {
				break;
			}

			counter += n;
		}

		if (taskno == 8 || taskno == 9) {
			lock.l_type = F_UNLCK; 
   			fcntl(fd, F_SETLK, &lock);
		}

		close(fd);

		printf("%d\n", counter);

		return 0;
    }
}

int task1011(int argc, char* argv[], long taskno) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab4 10\n");
		return 1;
	}

	int parentpid = getpid();
	int childpid = fork();

	if (childpid) {
		printf("parent:	%d\n", parentpid);
		printf("child:	%d\n", childpid);
	}

	int pid = getpid();

	struct flock lock = {0};
	lock.l_pid = pid;

	while (1) {
		if (taskno == 11) {
        	lock.l_type = F_WRLCK;
			while (1) {	
				int stdin_lock = fcntl(STDIN_FILENO, F_SETLK, &lock);
				int stdout_lock = fcntl(STDOUT_FILENO, F_SETLK, &lock);
				if (stdin_lock == 0 && stdout_lock == 0) {
					printf("%d: lock\n", pid);
					break;
				}
			}
		}

		char buf[4096] = {0};
		int n = read(STDIN_FILENO, buf, sizeof(buf));
		printf("%d: %s", pid, buf);
	
		if (taskno == 11) {
			lock.l_type = F_UNLCK;
			while (1) {	
				int stdin_unlock = fcntl(STDIN_FILENO, F_SETLK, &lock);
				int stdout_unlock = fcntl(STDOUT_FILENO, F_SETLK, &lock);
				if (stdin_unlock == 0 && stdout_unlock == 0) {
					printf("%d: unlock\n", pid);
					break;
				}
			}
		}
	}

	return 0;
}

int taskx(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab4 x\n");
		return 1;
	}

	return 0;
}

int main(int argc, char* argv[], char* envp[]) {
    if (argc < 2) {
		fprintf(stderr, "usage: ./lab3 taskno args...\n");
		return 1;
	}

	long taskno = strtol(argv[1], (char**)NULL, 10);

	switch (taskno) {
		case 1:
			task1(argc, argv);
			break;
		case 2:
			task2(argc, argv);
			break;
		case 3:
			task3(argc, argv);
			break;
		case 4:
			task4(argc, argv);
			break;
		case 5:
			task5(argc, argv);
			break;
		case 6:
			task6(argc, argv);
			break;
		case 7:
		case 8:
		case 9:
			task789(argc, argv, taskno);
			break;
		case 10:
		case 11:
			task1011(argc, argv, taskno);
			break;
		default:
			printf("invalid taskno: %ld", taskno);
			return 1;
	}
	
	return 0;
}
