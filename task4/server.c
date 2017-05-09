#include <stdio.h>
#include <sys/types.h>
#include <sys/file.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <pthread.h>

#define LIFE_HEIGHT 10
#define LIFE_WIDTH 10
#define PORT_NUMBER 1337

char life[LIFE_HEIGHT][LIFE_WIDTH];
char oldLife [LIFE_HEIGHT][LIFE_WIDTH];

int checkDistX(int x) {
    if (x >= 0 && x < LIFE_WIDTH) {
        return 1;
    }
    return 0;
}

int checkDistY(int y) {
    if (y >= 0 && y < LIFE_HEIGHT) {
        return 1;
    }
    return 0;
}

void makeStep() {
    int i, j, x, y, adjusted_i, adjusted_j, count;
    for (i = 0; i < LIFE_HEIGHT; i++) {
        for (j = 0; j < LIFE_WIDTH; j++) {
            oldLife[i][j] = life[i][j];
        }
    }
    for (i = 0; i < LIFE_HEIGHT; i++) {
        for (j = 0; j < LIFE_WIDTH; j++) {
            count = 0;
            for(x = -1; x < 2; x++) {
                for (y = -1; y < 2; y++) {
                    if (x == 0 && y == 0)
                        continue;
                    adjusted_i = i + x;
                    adjusted_j = j + y;
                    if (checkDistY(adjusted_i) == 0) {
                            continue;
                    }
                    if (checkDistY(adjusted_j) == 0) {
                            continue;
                    }
                    count += oldLife[adjusted_i][adjusted_j];
                }
            }
            switch(count) {
                case 3:
                    life[i][j] = 1;
                    break;
                case 2:
                    break;
                default:
                    life[i][j] = 0;
            }   
        }
    }
}

void checkLive() {
    pthread_t step_t;
    while (1) {
        pthread_create(&step_t, NULL, &makeStep, NULL);
        sleep(1);
        if (pthread_kill(step_t, NULL) == 0) {
            printf("Error: not generate field in 1 sec\n");
        }
        while (pthread_kill(step_t, NULL) == 0) {
            ;
        }
    }
}
 
int main () {
	FILE *fd = fopen("field.txt", "r");
	if (fd == NULL) {
        printf("Error openning file");
        exit(1);
    };
	int symb;
    for (int i = 0; i < LIFE_HEIGHT; i++) {
        for (int j = 0; j < LIFE_WIDTH; j++) {
            symb = fgetc(fd);
            life[i][j] = symb - '0';
        };
        symb = fgetc(fd);
    };
    fclose(fd);
    pthread_t life_t;
    pthread_create(&life_t, NULL, &checkLive, NULL);
    int sock, newsockfd, clilen;
    struct sockaddr_in serv_addr, cli_addr;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Error: cannot open socket");
        exit(1);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(PORT_NUMBER);
    if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        perror("Error: cannot bind socket");
        exit(1);
    }
    listen(sock, 5);
    clilen = sizeof(cli_addr);
    while(1) {
        newsockfd = accept(sock, (struct sockaddr *)&cli_addr, &clilen);
        switch (fork()) { 
        case -1:
            printf("Error: cannot fork()\n");
            exit(1);
        case 0:
            close(sock);
            write(newsockfd, life, 100);
            close(newsockfd);
            exit(0);
        default:
            close(newsockfd);
        }
    }   
}