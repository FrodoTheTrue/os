#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

typedef int bool;
#define true 1
#define false 0

void generateSparse(int fd) {
	char byte;
	bool isNonZero;
	isNonZero = false;
	while(read(0, &byte, 1)) {
		if (byte == 0) {
			if (lseek(fd, 1, SEEK_CUR) <= 0) {
				printf("Error printing to file");
				exit(1);
			}
			continue;
		}
		isNonZero = true;
		if (write(fd, &byte, 1) <= 0) {
			printf("Error printing to file");
			exit(1);
		};
	}
	if (isNonZero == true) {
		write(fd, 0, 1);
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2)
    {
        printf("Error with arguments\n");
        return -1;
    }
	int fd = open(argv[1], O_WRONLY | O_TRUNC, S_IRWXU);
	if (fd == -1) {
		printf("Error openning file\n");
		return -1;
	}
	generateSparse(fd);
	close(fd);
}