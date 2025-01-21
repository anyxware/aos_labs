#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_LEN 32

typedef struct msg_data {
    long type;
    char data[MSG_LEN + 1];
} msg_data;

int task1(int argc, char* argv[]) {
	if (argc != 3) {
		fprintf(stderr, "usage: ./lab5 1 pathname\n");
		return 1;
	}

	char* pathname = argv[2];

	key_t key = ftok(pathname, 228);
	int msqid = msgget(key, IPC_CREAT | 0666);
    printf("msqid: %d\n", msqid);

    msg_data msg;

    int message_types[] = {0, 1, 2, 3, 4};
    const char* messages[] = {"unknown", "create", "read", "update", "delete"};

    for (int i = 0; i < 5; i++) {
        msg.type = message_types[i];
        strcpy(msg.data, messages[i]);
        msgsnd(msqid, &msg, sizeof(msg), 0);
    }

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
    printf("Sequence number: %u\n\n", perm._seq);
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
		default:
			printf("invalid taskno: %ld", taskno);
			return 1;
	}
	
	return 0;
}