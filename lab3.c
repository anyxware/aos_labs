#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

void sighandler(int signum) {
    switch (signum) {
        case SIGINT:
            printf("SIGINT handled, enter something: ");
			getchar();
            break;
		case SIGUSR1:
            printf("SIGUSR1 handled, enter something: ");
            getchar();
			break;
		case SIGUSR2:
            printf("SIGUSR2 handled, enter something: ");
            getchar();
			break;
		case SIGALRM:
            printf("SIGALRM handled, enter something: ");
            getchar();
			break;
        default:
            signal(signum, SIG_DFL);
    }
}

int task1(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab3 1\n");
		return 1;
	}

	signal(SIGINT, sighandler);

    while (1) {
        sleep(1);
	}

	return 0;
}

int task2(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab3 2\n");
		return 1;
	}

	struct sigaction sa;
    sa.sa_handler = sighandler;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGINT, &sa, NULL);
    
	while (1) {
		sleep(1);
	}

	return 0;
}

int task3(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab3 3\n");
		return 1;
	}

	signal(SIGCHLD, SIG_IGN);

	int childpid = fork();

	if (childpid) {
		getchar();

		return 0;
	}

    printf("child pid: %d\n", getpid());

	return 0;
}

int task4(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab3 4\n");
		return 1;
	}

	int childpid = fork();

	if (childpid) {
		int code;
        wait(&code);
        printf("child status: %d\n", code);

		return 0;
	}

	for (int i = 0; i < 1000; i++) {
        printf("iteration: %d\n", i);
        for (int j = 0; j < 1000; j++) {
            for (int k = 0; k < 1000; k++) {}
		}
	}

	return 0;
}

void child56789(long taskno) {
	if (taskno == 7 || taskno == 8 || taskno == 9) {
		struct sigaction sa;
		sa.sa_handler = sighandler;
		sigemptyset(&sa.sa_mask);

		if (taskno == 8) {
			sigaction(SIGUSR1, &sa, NULL);
			sigaction(SIGUSR2, &sa, NULL);
		}

		if (taskno == 9) {
			sigaction(SIGALRM, &sa, NULL);
			alarm(1);
			printf("waiting for alarm\n");
		}

		pause();

		return;
	}

	for (int i = 0; i < 1000; i++) {
        printf("iteration: %d\n", i);
        for (int j = 0; j < 1000; j++) {
            for (int k = 0; k < 1000; k++) {}
		}
	}
}

int task56789(int argc, char* argv[], long taskno) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab3 5\n");
		return 1;
	}

	int childpid = fork();

	if (childpid) {
		int sig;
		if (taskno == 5) {
			sig = SIGUSR1;
		} else {
			sig = SIGUSR2;
		}

		if (taskno != 9) {
			getchar();
			kill(childpid, sig);
		}

		int code;
        wait(&code);
        printf("child status: %d\n", code);

		return 0;
	} 
	
	child56789(taskno);		

	return 0;
}

void sighandler2(int signum) {
    switch (signum) {
        case SIGINT:
            printf("SIGINT handled, enter something: ");
			getchar();
            break;
		case SIGTERM:
            printf("SIGTERM handled, enter something: ");
			getchar();
            break;
    }
}

int task10(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab3 10\n");
		return 1;
	}

	struct sigaction sa;
	sa.sa_handler = sighandler2;
	sigemptyset(&sa.sa_mask);

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	sigset_t sigset;
    sigaddset(&sigset, SIGTERM);
    sigprocmask(SIG_BLOCK, &sigset, NULL);

	pause();

	sigprocmask(SIG_UNBLOCK, &sigset, NULL);
    
	return 0;
}

void handler3(int signum) {}

int task11(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab3 11\n");
		return 1;
	}

	sigset_t sigset, emptysigset;
    sigaddset(&sigset, SIGUSR1);
	sigemptyset(&emptysigset);

	struct sigaction sa;
	sa.sa_handler = handler3;
	sigemptyset(&sa.sa_mask);

	int parentpid = getpid();
	int childpid = fork();

	sigaction(SIGUSR1, &sa, NULL);

	int currentpid = getpid(), anotherpid;
	char* message;

	if (childpid) {
		anotherpid = childpid;
		message = "tik";
		printf("parent: %d\n", currentpid);
		printf("child: %d\n", childpid);
	} else {
		anotherpid = parentpid;
		message = "tak";
		sigsuspend(&emptysigset);
	}

	while (1) {
		sigprocmask(SIG_BLOCK, &sigset, NULL);

		printf("%d: %s\n", currentpid, message);
		sleep(1);

		kill(anotherpid, SIGUSR1);
        
		sigsuspend(&emptysigset);
	}

	return 0;
}

int taskx(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab3 x\n");
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
		case 6:
		case 7:
		case 8:
		case 9:
			task56789(argc, argv, taskno);
			break;
		case 10:
			task10(argc, argv);
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
