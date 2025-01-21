#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <limits.h>
#include <sys/stat.h>
#include <time.h>

#define major(x)        ((int32_t)(((u_int32_t)(x) >> 24) & 0xff))
#define minor(x)        ((int32_t)((x) & 0xffffff))

int task1(int argc, const char* argv[]) {
	if(argc != 3) {
		fprintf(stderr, "usage: ./lab1 1 filename\n");
		return 1;
	}

	const char* filename = argv[2];

	int fd = open(filename, O_RDONLY);
	if(fd == -1) {
		fprintf(stderr, "open file \"%s\": %s\n", filename, strerror(errno));
		fprintf(stderr, "open file \"%s\": %s\n", filename, sys_errlist[errno]);
		fprintf(stderr, "open file \"%s\": ", filename); perror(NULL);
		return 1;
	}
	
	printf("file \"%s\" opened\n", filename);

    return 0;
}

int task23(int argc, const char* argv[]) {
	if(argc != 4) {
		fprintf(stderr, "usage: ./lab1 2 filename mode\n");
		return 1;
	}

	const char* filename = argv[2];
	const char* raw_mode = argv[3];

	char *mode_end;
	long mode = strtol(raw_mode, &mode_end, 8);
	if (mode_end == raw_mode || *mode_end != '\0' || ((mode == LONG_MIN || mode == LONG_MAX) && errno == ERANGE)) {
        fprintf(stderr, "parse mode \"%s\": %s\n", raw_mode, mode_end);
		return 1;
	}

	int fd = open(filename, O_CREAT | O_RDONLY, mode);
	if(fd == -1) {
		fprintf(stderr, "open file \"%s\": %s\n", filename, strerror(errno));
		return 1;
	}

	char data[] = "abcd\nedfg\nhijk\0";

	ssize_t n = write(fd, data, strlen(data));
	if(n == -1) {
		fprintf(stderr, "write to file \"%s\": %s\n", filename, strerror(errno));
		close(fd);
	}

	fd = open(filename, O_RDONLY);
	if(fd == -1) {
		fprintf(stderr, "open file \"%s\": %s\n", filename, strerror(errno));
		return 1;
	}

	char buf[256];
	memset(buf, 0, sizeof(buf));

	n = read(fd, buf, sizeof(buf));
	if(n == -1) {
		fprintf(stderr, "read from file \"%s\": %s\n", filename, strerror(errno));
		close(fd);
	}

	printf("buffer: %s\n", buf);

	if(close(fd) == -1) {
		fprintf(stderr, "close file \"%s\": %s\n", filename, strerror(errno));
		return 1;
	}

	return 0;
}

int task456(int argc, const char* argv[]) {
	if(argc != 4) {
		fprintf(stderr, "usage: ./lab1 2 filename mode\n");
		return 1;
	}

	const char* filename = argv[2];
	const char* raw_mode = argv[3];

	char *mode_end;
	long mode = strtol(raw_mode, &mode_end, 8);
	if (mode_end == raw_mode || *mode_end != '\0' || ((mode == LONG_MIN || mode == LONG_MAX) && errno == ERANGE)) {
        fprintf(stderr, "parse mode \"%s\": %s\n", raw_mode, mode_end);
		return 1;
	}

	int fd = open(filename, O_CREAT | O_RDWR, mode);
	if(fd == -1) {
		fprintf(stderr, "open file \"%s\": %s\n", filename, strerror(errno));
		return 1;
	}

	char data[] = "abcd\nedfg\nhijk";

	ssize_t n = write(fd, data, strlen(data));

	char buf[256];
	memset(buf, 0, sizeof(buf));

	off_t offset = lseek(fd, 0, SEEK_CUR);
	printf("offset: %lli\n", offset);
	read(fd, buf, sizeof(buf));
	printf("buffer: %s\n", buf);
	memset(buf, 0, sizeof(buf));

	offset = lseek(fd, 0, SEEK_SET);
	printf("offset: %lli\n", offset);
	read(fd, buf, sizeof(buf));
	printf("buffer: %s\n", buf);
	memset(buf, 0, sizeof(buf));

	offset = lseek(fd, -4, SEEK_END);
	printf("offset: %lli\n", offset);
	read(fd, buf, sizeof(buf));
	printf("buffer: %s\n", buf);
	memset(buf, 0, sizeof(buf));

	char data2[] = "newdata";
	offset = lseek(fd, 5, SEEK_SET);
	printf("offset: %lli\n", offset);
	write(fd, data2, strlen(data2));
	offset = lseek(fd, 0, SEEK_SET);
	printf("offset: %lli\n", offset);
	read(fd, buf, sizeof(buf));
	printf("buffer: %s\n", buf);
	memset(buf, 0, sizeof(buf));

	offset = lseek(fd, 0, SEEK_CUR);
	printf("offset: %lli\n", offset);

	offset = lseek(fd, -10, SEEK_SET);
	fprintf(stderr, "lseek in file \"%s\": %s\n", filename, strerror(errno));

	offset = lseek(fd, 256, SEEK_SET);
	printf("offset: %lli\n", offset);

	write(fd, "Z", 1);

	if(close(fd) == -1) {
		fprintf(stderr, "close file \"%s\": %s\n", filename, strerror(errno));
		return 1;
	}

	return 0;
}

int task7(int argc, const char* argv[]) {
	if(argc != 3) {
		fprintf(stderr, "usage: ./lab1 7 filename\n");
		return 1;
	}

	const char* filename = argv[2];

	struct stat sb;

	if (stat(filename, &sb) != 0) {
		fprintf(stderr, "get stats for file \"%s\": %s\n", filename, strerror(errno));
		return 1;
	}

	printf("ID of containing device:  [%x,%x]\n",
                  major(sb.st_dev),
                  minor(sb.st_dev));

    printf("File type:                ");

    switch (sb.st_mode & S_IFMT) {
    case S_IFBLK:  printf("block device\n");            break;
    case S_IFCHR:  printf("character device\n");        break;
    case S_IFDIR:  printf("directory\n");               break;
    case S_IFIFO:  printf("FIFO/pipe\n");               break;
    case S_IFLNK:  printf("symlink\n");                 break;
    case S_IFREG:  printf("regular file\n");            break;
    case S_IFSOCK: printf("socket\n");                  break;
    default:       printf("unknown?\n");                break;
    }

    printf("I-node number:            %ju\n", (uintmax_t) sb.st_ino);

    printf("Mode:                     %jo (octal)\n",
                  (uintmax_t) sb.st_mode);

    printf("Link count:               %ju\n", (uintmax_t) sb.st_nlink);
    printf("Ownership:                UID=%ju   GID=%ju\n",
                  (uintmax_t) sb.st_uid, (uintmax_t) sb.st_gid);

    printf("Preferred I/O block size: %jd bytes\n",
                  (intmax_t) sb.st_blksize);
    printf("File size:                %jd bytes\n",
                  (intmax_t) sb.st_size);
    printf("Blocks allocated:         %jd\n",
                  (intmax_t) sb.st_blocks);

    printf("Last status change:       %s", ctime(&sb.st_ctime));
    printf("Last file access:         %s", ctime(&sb.st_atime));
    printf("Last file modification:   %s", ctime(&sb.st_mtime));

	return 0;
}

int copy(int fd_src, int fd_dst) {
	char buf[8];
	ssize_t nr, nw;

	for(;;) {
		nr = read(fd_src, buf, sizeof(buf));
		if(nr == -1) {
			fprintf(stderr, "read from file: %s\n", strerror(errno));
			return 1;
		} else if(nr == 0) {
			break;
		}

		nw = write(fd_dst, buf, nr);
		if(nw == -1) {
			fprintf(stderr, "write to file: %s\n", strerror(errno));
			return 1;
		} else if(nw != nr) {
			fprintf(stderr, "incorrect number of written bytes\n");
			return 1;
		}		
	}

	return 0;
}

int task8(int argc, const char* argv[]) {
	if(argc != 4 && argc != 2) {
		fprintf(stderr, "usage: ./lab1 8 [filename_src filename_dst]\n");
		return 1;
	}

	int files_specified = 0;
	const char* filename_src;
	const char* filename_dst;

	if(argc == 4) {
		files_specified = 1;
		filename_src = argv[2];
		filename_dst = argv[3];
	}

	int fd_src = 0;
	int fd_dst = 1;

	if(files_specified) {
		fd_src = open(filename_src, O_RDONLY);
		if(fd_src == -1) {
			fprintf(stderr, "open file \"%s\": %s\n", filename_src, strerror(errno));
			return 1;
		}

		fd_dst = open(filename_dst, O_WRONLY | O_TRUNC | O_CREAT, 0644);
		if(fd_dst == -1) {
			fprintf(stderr, "open file \"%s\": %s\n", filename_dst, strerror(errno));
			return 1;
		}
	}

	copy(fd_src, fd_dst);

	if(close(fd_src) == -1) {
		fprintf(stderr, "close file \"%s\": %s\n", filename_src, strerror(errno));
		return 1;
	}

	if(close(fd_dst) == -1) {
		fprintf(stderr, "close file \"%s\": %s\n", filename_dst, strerror(errno));
		return 1;
	}

	return 0;
}


int task9(int argc, const char* argv[]) {
	if(argc != 4 && argc != 2) {
		fprintf(stderr, "usage: ./lab1 8 [filename_src filename_dst]\n");
		return 1;
	}

	int files_specified = 0;
	const char* filename_src;
	const char* filename_dst;

	if(argc == 4) {
		files_specified = 1;
		filename_src = argv[2];
		filename_dst = argv[3];
	}

	close(0);
	close(1);
	int fd_src, fd_dst;

	if(files_specified) {
		fd_src = open(filename_src, O_RDONLY);
		if(fd_src == -1) {
			fprintf(stderr, "open file \"%s\": %s\n", filename_src, strerror(errno));
			return 1;
		}

		dup2(fd_src, 0);

		fd_dst = open(filename_dst, O_WRONLY | O_TRUNC | O_CREAT, 0644);
		if(fd_dst == -1) {
			fprintf(stderr, "open file \"%s\": %s\n", filename_dst, strerror(errno));
			return 1;
		}

		dup2(fd_dst, 1);
	}

	copy(0, 1);

	if(close(fd_src) == -1) {
		fprintf(stderr, "close file \"%s\": %s\n", filename_src, strerror(errno));
		return 1;
	}

	if(close(fd_dst) == -1) {
		fprintf(stderr, "close file \"%s\": %s\n", filename_dst, strerror(errno));
		return 1;
	}

	return 0;
}

int task10(int argc, const char* argv[]) {
	if(argc != 3) {
		fprintf(stderr, "usage: ./lab1 10 filename\n");
		return 1;
	}

	const char* filename = argv[2];

	int fd = open(filename, O_RDONLY);
	if(fd == -1) {
		fprintf(stderr, "open file \"%s\": %s\n", filename, strerror(errno));
		return 1;
	}

	char buf[8];

	off_t offset = lseek(fd, 0, SEEK_END);
	
	while(offset > sizeof(buf)) {
		offset = lseek(fd, -1 * sizeof(buf), SEEK_CUR);
		int n = read(fd, buf, sizeof(buf));
		for(int i = n - 1; i >= 0; i--) {
			printf("%i\n", buf[i]);
		}
		offset = lseek(fd, -1 * n, SEEK_CUR);
	}

	offset = lseek(fd, -1 * offset, SEEK_CUR);
	int n = read(fd, buf, sizeof(buf));
	for(int i = n - 1; i >= 0; i--) {
		printf("%c", buf[i]);
	}

	return 0;
}

int task11(int argc, const char* argv[]) {
	if(argc != 4) {
		fprintf(stderr, "usage: ./lab1 11 filename1 filename2\n");
		return 1;
	}

	const char* filename1 = argv[2];
	const char* filename2 = argv[3];

	int fd1 = open(filename1, O_RDONLY);
	if(fd1 == -1) {
		fprintf(stderr, "open file \"%s\": %s\n", filename1, strerror(errno));
		return 1;
	}

	int fd2 = open(filename2, O_RDONLY);
	if(fd2 == -1) {
		fprintf(stderr, "open file \"%s\": %s\n", filename2, strerror(errno));
		return 1;
	}

	off_t offset1 = lseek(fd1, 0, SEEK_END);
	off_t offset2 = lseek(fd2, 0, SEEK_END);

	if(offset1 > offset2) {
		printf("%s is bigger, size: %lli\n", filename1, offset1);
	} else if(offset1 < offset2) {
		printf("%s is bigger, size: %lli\n", filename2, offset2);
	} else {
		printf("equal\n");
	}

	close(fd1);
	close(fd2);

	return 0;
}

int main(int argc, const char* argv[]) {
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
			task23(argc, argv);
			break;
		case 4:
		case 5:
		case 6:
			task456(argc, argv);
			break;
		case 7:
			task7(argc, argv);
			break;
		case 8:
			task8(argc, argv);
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
