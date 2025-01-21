#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

int task1(int argc, char* argv[]) {
	if(argc != 4) {
		fprintf(stderr, "usage: ./lab2 1 env_var_name env_var_value\n");
		return 1;
	}

	char* env_var_name = argv[2];
	char* env_var_value = argv[3];

	setenv(env_var_name, env_var_value, 1);

	env_var_value = getenv(env_var_name);

	printf("%s=%s\n", env_var_name, env_var_value);

	return 0;
}

int task2(int argc, char* argv[]) {
	if(argc != 2) {
		fprintf(stderr, "usage: ./lab2 2\n");
		return 1;
	}

	int fork_res = fork();

	if (fork_res) {
		printf("parent\n");
		return 0;
	}
	
	printf("child\n");

	return 0;
}

int task3(int argc, char* argv[]) {
	if(argc != 2) {
		fprintf(stderr, "usage: ./lab2 3\n");
		return 1;
	}

	pid_t pid, ppid;
    gid_t gid;

    pid = getpid();
    ppid = getppid();
    gid = getgid();
    printf("parent: pid=%d; ppid=%d; gid=%d\n", pid, ppid, gid);

    int fork_res = fork();

    if (fork_res) {
        int child_result;
        pid_t child_pid = wait(&child_result);

	    printf("parent: child_pid=%d; child_result=%d\n", child_pid, child_result);

		return 0;
    }

	pid = getpid();
    ppid = getppid();
    gid = getgid();
    printf("child: pid=%d; ppid=%d; gid=%d\n", pid, ppid, gid);

	return 0;
}

int task4(int argc, char* argv[]) {
	if(argc != 2) {
		fprintf(stderr, "usage: ./lab2 4\n");
		return 1;
	}

	pid_t pid, ppid;
    gid_t gid;

    pid = getpid();
    ppid = getppid();
    gid = getgid();
    printf("parent: pid=%d; ppid=%d; gid=%d\n", pid, ppid, gid);

    int fork_res = fork();

    if (fork_res) {
		// parent die immediately
		return 0;
    }

	pid = getpid();
    ppid = getppid();
    gid = getgid();
    printf("child: pid=%d; ppid=%d; gid=%d\n", pid, ppid, gid);

	sleep(60);

	return 0;
}

int task5(int argc, char* argv[]) {
	if(argc != 2) {
		fprintf(stderr, "usage: ./lab2 5\n");
		return 1;
	}

	pid_t pid, ppid;
    gid_t gid;
    
    pid = getpid();
    ppid = getppid();
    gid = getgid();
    printf("parent: pid=%d; ppid=%d; gid=%d\n", pid, ppid, gid);

    int fork_res = fork();

    if (fork_res) {
        getchar();

		return 0;
    }

    pid = getpid();
    ppid = getppid();
    gid = getgid();
    printf("child: pid=%d; ppid=%d; gid=%d\n", pid, ppid, gid);

	return 0;
}

int task6(int argc, char* argv[]) {
	if(argc != 2) {
		fprintf(stderr, "usage: ./lab2 6\n");
		return 1;
	}

	pid_t pid, ppid;
    gid_t gid;
    
    pid = getpid();
    ppid = getppid();
    gid = getgid();
    printf("parent: pid=%d; ppid=%d; gid=%d\n", pid, ppid, gid);

    int fork_res = fork();

    if (fork_res) {
        pause();

		return 0;
    }

    pid = getpid();
    ppid = getppid();
    gid = getgid();
    printf("child: pid=%d; ppid=%d; gid=%d\n", pid, ppid, gid);

	pause();

	return 0;
}

int task7(int argc, char* argv[]) {
	if(argc != 2) {
		fprintf(stderr, "usage: ./lab2 7\n");
		return 1;
	}

	pid_t pid, ppid;
    gid_t gid;

    pid = getpid();
    ppid = getppid();
    gid = getgid();
	printf("parent: pid=%d; ppid=%d; gid=%d\n", pid, ppid, gid);
    
	int fork_res = fork();

	if (fork_res) {
		pause();

		return 0;
	}

    pid = getpid();
    ppid = getppid();
    gid = getgid();
    printf("child: pid=%d; ppid=%d; gid=%d\n", pid, ppid, gid);

    setpgrp();

	pause();

	return 0;
}

int task8(int argc, char* argv[], int taskno) {
	if(argc != 3) {
		fprintf(stderr, "usage: ./lab2 8|9 filename\n");
		return 1;
	}

	char* src_filename = argv[2];

	char* parent_dst_filename = "lab2.parent.txt";
	char* child_dst_filename = "lab2.child.txt";
	char* dst_filename;

	int src_fd;
	int fork_res;

	if (taskno == 8) {
		src_fd = open(src_filename, O_RDONLY);
		fork_res = fork();
	} else if (taskno == 9) {
		fork_res = fork();
		src_fd = open(src_filename, O_RDONLY);
	}  else {
		fprintf(stderr, "invalid taskno\n");
		return 1;
	}

    if (fork_res) {
        dst_filename = parent_dst_filename;
    } else {
        dst_filename = child_dst_filename;
    }

    int dst_fd = open(dst_filename, O_RDWR | O_CREAT | O_TRUNC, 0644);

    char buf[256];
	ssize_t n;

    do {
        n = read(src_fd, buf, sizeof(buf));
        write(dst_fd, buf, n);
        memset(buf, '\0', sizeof(buf));
    } while (n == sizeof(buf));

    if (fork_res) {
        int child_result;
        wait(&child_result);
    }

    // lseek(dst_fd, 0, SEEK_SET);

    // do {
    //     n = read(dst_fd, buf, sizeof(buf));
    //     printf("%s", buf);
    //     memset(buf, '\0', sizeof(buf));
    // } while(n == sizeof(buf));

	if (taskno == 8) {
		if (fork_res) {
			close(src_fd);
		}
		close(dst_fd);
	} else {
		close(src_fd);
		close(dst_fd);
	}

	return 0;
}

int task10(int argc, char* argv[], char* envp[]) {
	if(argc < 3) {
		fprintf(stderr, "usage: ./lab2 10 filename args...\n");
		return 1;
	}

	printf("args:\n");
    
	for (char** i = argv; *i != NULL; i++) {
        printf("%s\n", *i);
    }
		        
	printf("envs:\n");
    
	for (char** i = envp; *i != NULL; i++) {
        printf("%s\n", *i);
    }

	printf("\n");

	char* filename = argv[2];

	if (strcmp(filename, "child") == 0) {
		printf("in child program");
		return 0;
	}

	char** child_argv = argv + 2;
	char** child_envp = envp;

	int fork_res = fork();

    if (fork_res) {
        int child_result;
        wait(&child_result);
		return child_result;
	}

   	return execve(filename, child_argv, child_envp);
}

int task11(int argc, char* argv[]) {
	if(argc != 3) {
		fprintf(stderr, "usage: ./lab2 11 command\n");
		return 1;
	}

	system(argv[2]);

	return 0;
}

int taskx(int argc, char* argv[]) {
	if(argc != 2) {
		fprintf(stderr, "usage: ./lab2 x\n");
		return 1;
	}

	return 0;
}

int main(int argc, char* argv[], char* envp[]) {
    if(argc < 2) {
		fprintf(stderr, "usage: ./lab2 taskno args...\n");
		return 1;
	}

	long taskno = strtol(argv[1], (char**)NULL, 10);

	switch(taskno) {
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
			task7(argc, argv);
			break;
		case 8:
		case 9:
			task8(argc, argv, taskno);
			break;
		case 10:
			task10(argc, argv, envp);
			break;
		case 11:
			task11(argc, argv);
			break;
		default:
			printf("invalid taskno: %ld", taskno);
			return 1;
	}
	
	return 0;
}
