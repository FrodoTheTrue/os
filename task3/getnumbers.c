#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>

#define MAX_NUMBERS 1000000000

int countNumbers = 0;
long long numbersArray[MAX_NUMBERS];

int takeNumbers(int fd) {
	int numberSize = 0;
	int isNegativeNumber = 0;
	int isCreatingNumber = 0;
	long long currentNumber = 0;
	char byte;
	while (read(fd, &byte, 1) > 0) {
		if (byte == '-' && numberSize == 0) {
			isNegativeNumber = 1;
			isCreatingNumber = 1;
		}
		if (byte >= '0' && byte <= '9') {
			if (numberSize == 0) {
				isCreatingNumber = 1;
			}
			if (numberSize >= 20) {
				printf("Too big number\n");
				return 1;
			}
			char digit = byte - '0';
			currentNumber = currentNumber * 10 + digit;
			numberSize++;
		} else {
			if (isCreatingNumber) {
				if (byte != '-') {
					numberSize = 0;
					if (MAX_NUMBERS == countNumbers) {
						printf("Too much numbers\n");
						exit(1);
					} else {
						if (isNegativeNumber) {
							currentNumber = currentNumber * -1;
						}
						// numbersArray = realloc(numbersArray, countNumbers * sizeof(long long int) + 1);
						numbersArray[countNumbers] = currentNumber;
						countNumbers = countNumbers + 1;
					}
					isCreatingNumber = 0;
					isNegativeNumber = 0;
					currentNumber = 0;
				}
			} else {
				isCreatingNumber = 0;
				isNegativeNumber = 0;
				currentNumber = 0;
			}
		}
	}
}


int compareNumbers(const void * n1, const void * n2) 
{
    if (*((long long*)n1) > *((long long*)n2)) return  1;
    if (*((long long*)n1) < *((long long*)n2)) return -1;
    return 0;
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
        printf("Need 2 and more arguments\n");
        return -1;
    }
    // long long int *numbersArray = (long long int*) malloc(0 * sizeof(long long int));
    for (int i=0;i<argc-2;i++) {
    	int fd = open(argv[i+1], O_RDONLY);
		if (fd == -1) {
			printf("Error openning file with numbers\n");
			return -1;
		}
		takeNumbers(fd);
    }
   
	qsort(numbersArray, countNumbers, sizeof(long long), compareNumbers);

	int fd_result = open(argv[argc-1], O_WRONLY);
    if (fd_result == -1) {
    	printf("Error openning result file\n");
		return -1;
    }
    int i;
    for (i = 0; i < countNumbers; i++) {
		int res = dprintf(fd_result, "%lld\n", numbersArray[i]);
		if (res < 0) {
			printf("Error write number to file");
			exit(1);
		}
	}

	close(fd_result);
}