#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <signal.h>
#include <sys/msg.h>
#include <time.h>

#define MSG_LEN 32

typedef struct msg_data {
    long type;
    char data[MSG_LEN + 1];
} msg_data;

int task1(int argc, char* argv[]) {
	if (argc != 4) {
		fprintf(stderr, "usage: ./lab5 1 type\n");
		return 1;
	}

	char* pathname = argv[2];
	int type = strtol(argv[3], (char**)NULL, 10);

	key_t key = ftok(pathname, 228);
	int msqid = msgget(key, IPC_CREAT | 0666);
    printf("msqid: %d\n\n", msqid);

    int message_types[] = {1, 2, 3, 4};
    const char* messages[] = {"create", "read", "update", "delete"};

    msg_data msg;

	msg.type = type;
    strcpy(msg.data, messages[type-1]);
    msgsnd(msqid, &msg, sizeof(msg), 0);

    struct msqid_ds ds = {0};
    msgctl(msqid, IPC_STAT, &ds);
    
	struct ipc_perm perm = ds.msg_perm;
    printf("Ownership and permissions\n");
    printf("Key: %d\n", perm._key);
    printf("Effective UID of owner: %d\n", perm.uid);
    printf("Effective GID of owner: %d\n", perm.gid);
    printf("Effective UID of creator: %d\n", perm.cuid);
    printf("Effective GID of creator: %d\n", perm.cgid);
    printf("Permissions: %o\n", perm.mode);
    printf("Sequence number: %u\n", perm._seq);
    printf("Time of last msgsnd: %ld\n", ds.msg_ctime);
    printf("Time of last msgrcv: %ld\n", ds.msg_rtime);
    printf("Time of last change: %ld\n", ds.msg_ctime);
    printf("Current number of bytes in queue: %lu\n", ds.msg_cbytes);
    printf("Current number of messages in queue: %lu\n", ds.msg_qnum);
    printf("Maximum number of bytes allowed in queue: %lu\n", ds.msg_qbytes);
    printf("PID of last msgsnd: %d\n", ds.msg_lspid);
    printf("PID of last msgrcv: %d\n", ds.msg_lrpid);

	return 0;
}

int task234(int argc, char* argv[], int taskno) {
	if (argc != 4) {
		fprintf(stderr, "usage: ./lab5 msqid type\n");
		return 1;
	}

	//char* pathname = argv[2];
	int msqid = strtol(argv[2], (char**)NULL, 10);
	int type = strtol(argv[3], (char**)NULL, 10);

	//key_t key = ftok(pathname, 228);
	//int msqid = msgget(key, IPC_CREAT | 0666);
    printf("msqid: %d\n\n", msqid);

    int message_types[] = {1, 2, 3, 4};
    const char* messages[] = {"create", "read", "update", "delete"};

    msg_data msg;
	memset(msg.data, MSG_LEN + 1, sizeof(char));

	int flags = 0;
	if (taskno == 3) {
		flags = IPC_NOWAIT;
	} else if (taskno == 4) {
		flags = IPC_NOWAIT | MSG_NOERROR;
	}

	int sz;
	if (taskno == 4) {
		sz = 2;
	} else {
		sz = sizeof(msg);
	}

    msgrcv(msqid, &msg, sz, message_types[type-1], flags);
    printf("%s\n", msg.data);
	
	return 0;
}

int task5(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage: ./lab5 5 msqid\n");
		return 1;
	}

	int msqid = strtol(argv[2], (char**)NULL, 10);

    printf("msqid: %d\n\n", msqid);

	int res = msgctl(msqid, IPC_RMID, 0);
	if (res) {
        perror("remove message queue");
        return 1;
    }

	return 0;
}

typedef struct request {
    long type;
    int client_msqid;
} request;

typedef struct response {
    long type;
    char data[MSG_LEN + 1];
} response;

void sighandler(int signum) {
    switch (signum) {
        case SIGINT:
            printf("SIGINT handled\n");
            break;
		case SIGTERM:
            printf("SIGTERM handled\n");
            break;
        default:
            signal(signum, SIG_DFL);
    }

	exit(signum);
}

int task678(int argc, char* argv[], int taskno) {
	if (argc != 4 && argc != 5) {
		fprintf(stderr, "usage: ./lab5 6 mode pathname [server_msqid]\n");
		return 1;
	}

	if (taskno == 8) {
		struct sigaction sa;
    	sa.sa_handler = sighandler;
    	sigemptyset(&sa.sa_mask);
	    sigaction(SIGINT, &sa, NULL);
	}

	char* mode = argv[2];
	char* pathname = argv[3];

	int server_msg_type = 2;
	int client_msg_type = 1;

	if (strcmp(mode, "server") == 0) {
		char* server_msg = "200 OK";

		key_t key = ftok(pathname, 228);
		int msqid = msgget(key, IPC_CREAT | 0666);
		printf("server msqid: %d\n", msqid);

		request req;
		response resp;

		while (1) {
			msgrcv(msqid, &req, sizeof(req), server_msg_type, 0);
			int client_msqid = req.client_msqid;

			printf("server: request from client: %d\n", client_msqid);

			resp.type = client_msg_type;
			strcpy(resp.data, server_msg);
			msgsnd(client_msqid, &resp, sizeof(resp), 0);
		}

		msgctl(msqid, IPC_RMID, 0);

		return 0;
	}

	key_t key = ftok(pathname, 228);
	int msqid = msgget(key, IPC_CREAT | 0666);
    printf("client msqid: %d\n\n", msqid);

	int server_msqid = strtol(argv[4], (char**)NULL, 10);

	request req;
	response resp;

	while (1) {
		req.type = server_msg_type;
    	req.client_msqid = msqid;
    	msgsnd(server_msqid, &req, sizeof(req), 0);

		memset(resp.data, MSG_LEN + 1, sizeof(char));
		msgrcv(msqid, &resp, sizeof(resp), client_msg_type, 0);
		printf("client: server response: %s\n", resp.data);

		sleep(1);
	}

	msgctl(msqid, IPC_RMID, 0);

	return 0;
}

int task9(int argc, char* argv[]) {
	if (argc != 5) {
		fprintf(stderr, "usage: ./lab5 9 mode server_pathname client_pathname\n");
		return 1;
	}

	char* mode = argv[2];
	char* server_pathname = argv[3];
	char* client_pathname = argv[4];

	int server_msg_type = 2;

	key_t server_key = ftok(server_pathname, 228);
	int server_msqid = msgget(server_key, IPC_CREAT | 0666);
	printf("server msqid: %d\n", server_msqid);

	key_t client_key = ftok(client_pathname, 1337);
	int client_msqid = msgget(client_key, IPC_CREAT | 0666);
	printf("client msqid: %d\n", client_msqid);

	if (strcmp(mode, "server") == 0) {
		char* server_msg = "200 OK";

		response resp;
		request req;

		while (1) {
			msgrcv(server_msqid, &req, sizeof(req), 0, 0);

			printf("server: request from client: %ld\n", req.type);

			char msg[] = "200 OK";

			resp.type = req.type;
			strcpy(resp.data, msg);
			msgsnd(client_msqid, &resp, sizeof(resp), 0);
		}

		msgctl(server_msqid, IPC_RMID, 0);

		return 0;
	}

	srand(time(NULL));
	int client_msg_type = 1000 + rand() % 1000;

	request req;
	response resp;

	while (1) {
		req.type = client_msg_type;
		req.client_msqid = client_msqid;
    	msgsnd(server_msqid, &req, sizeof(req), 0);

		memset(resp.data, MSG_LEN + 1, sizeof(char));
		msgrcv(client_msqid, &resp, sizeof(resp), client_msg_type, 0);
		printf("client: server response: %s\n", resp.data);

		sleep(1);
	}

	msgctl(client_msqid, IPC_RMID, 0);

	return 0;
}

int task10(int argc, char* argv[]) {
	if (argc != 4) {
		fprintf(stderr, "usage: ./lab5 9 mode pathname\n");
		return 1;
	}

	char* mode = argv[2];
	char* pathname = argv[3];

	int server_msg_type = 2;

	key_t key = ftok(pathname, 228);
	int msqid = msgget(key, IPC_CREAT | 0666);
	printf("msqid: %d\n", msqid);

	if (strcmp(mode, "server") == 0) {
		char server_msg[] = "200 OK";

		response resp;
		request req;

		while (1) {
			msgrcv(msqid, &req, sizeof(req), server_msg_type, 0);

			printf("server: request from client: %d\n", req.client_msqid);

			char msg[] = "200 OK";

			resp.type = req.client_msqid + 1000;
			strcpy(resp.data, server_msg);
			msgsnd(msqid, &resp, sizeof(resp), 0);
		}

		msgctl(msqid, IPC_RMID, 0);

		return 0;
	}

	srand(time(NULL));
	int client_msg_type = 1000 + rand() % 1000;
	int resp_msg_type = 1000 + client_msg_type;

	request req;
	response resp;

	while (1) {
		req.type = server_msg_type;
		req.client_msqid = client_msg_type;
    	msgsnd(msqid, &req, sizeof(req), 0);

		memset(resp.data, MSG_LEN + 1, sizeof(char));
		msgrcv(msqid, &resp, sizeof(resp), resp_msg_type, 0);
		printf("client: server response: %s\n", resp.data);

		sleep(1);
	}

	return 0;
}

int task11(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage: ./lab5 11 pathname\n");
		return 1;
	}

	char* pathname = argv[2];

	key_t key = ftok(pathname, 228);
	int msqid = msgget(key, IPC_CREAT | 0666);
	printf("msqid: %d\n", msqid);

	msg_data msg;

    if (fork()) {
        while (1) {
            msg.type = 1;
            printf("tik\n");
            sleep(1);
            msgsnd(msqid, &msg, sizeof(msg), 0);
            sleep(1);
            msgrcv(msqid, &msg, sizeof(msg), 2, 0);
        }
    } else {
        while (1) {
            msgrcv(msqid, &msg, sizeof(msg), 1, 0);
            printf("tak\n");
            msg.type = 2;
            msgsnd(msqid, &msg, sizeof(msg), 0);
        }
    }

	return 0;
}

int taskx(int argc, char* argv[]) {
	if (argc != 2) {
		fprintf(stderr, "usage: ./lab5 x\n");
		return 1;
	}

	return 0;
}

int main(int argc, char* argv[]) {
    if(argc < 2) {
		fprintf(stderr, "usage: ./lab1 taskno args...\n");
		return 1;
	}

	long taskno = strtol(argv[1], (char**)NULL, 10);

	switch(taskno) {
		case 1:
			task1(argc, argv);
			break;
		case 2:
		case 3:
		case 4:
			task234(argc, argv, taskno);
			break;
		case 5:
			task5(argc, argv);
			break;
		case 6:
		case 7:
		case 8:
			task678(argc, argv, taskno);
			break;
		case 9:
			task9(argc, argv);
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