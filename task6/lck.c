#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <stdbool.h>

char lock[105];

int main(int argc, char * argv[]) {
	if (argc != 4) {
		printf("Error: wrong arguments\n");
	}
	sprintf(lock, "%s.lck", argv[1]);
	while (access(lock, 0) != -1) {
		;
	}
	FILE * lock_fp = fopen(lock, "w");
	fprintf(lock_fp, "%d:r", getpid());
	fclose(lock_fp);
	FILE * fp;
	fp = fopen(argv[1], "r");
	if (fp == NULL) {
		printf("Error openning file");
		exit(1);
	}
	char * line = NULL;
	size_t len = 0;
	ssize_t read;
	char * token;	
	const char delims[3] = " \n";
	char output [2000];
	int pass_changed = 0;
	while ((read = getline(&line, &len, fp)) != -1) {
		char login [100];
		char pass [100];

		token = strtok(line, delims);
		strcpy(login, token);
		
		token = strtok(NULL, delims);
		strcpy(pass, token);

		if (strcmp(login, argv[2]) == 0) {
			sprintf(output, "%s%s %s\n", output, login, argv[3]);
			pass_changed = 1;
		} else {
			sprintf(output, "%s%s %s\n", output, login, pass);
		}
	}
	if(!pass_changed) {
		sprintf(output, "%s%s %s\n", output, argv[2], argv[3]);
		pass_changed = 1;
	}
	fclose(fp);
	lock_fp = fopen(lock, "w");
	fprintf(lock_fp, "%d:w", getpid());
	fclose(lock_fp);
	fp = fopen(argv[1], "w");
	fprintf(fp, "%s", output);
	fclose(fp);
	remove(lock);
}