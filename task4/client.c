#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <netinet/in.h>
#include <string.h>

#define LIFE_WIDTH 10
#define LIFE_HEIGHT 10
#define PORT_NUMBER 1337

char field[LIFE_HEIGHT][LIFE_WIDTH];

void printBoard() {
	int i,j;
	for (i = 0; i < LIFE_HEIGHT; i++) {
		for (j = 0; j < LIFE_WIDTH; j++) {
			printf("%c", field[i][j] + '0');
		}
		printf("\n");
	}
}

int main(int argc, char *argv[]) {
   struct sockaddr_in serv_addr;
   struct hostent *server;
   int sock = socket(AF_INET, SOCK_STREAM, 0);
   if (sock < 0) {
      printf("Error while openning socket\n");
      exit(1);
   }
   server = gethostbyname("localhost");
   if (server == NULL) {
      printf("Error: host unreachable\n");
      exit(1);
   }
   bzero((char *) &serv_addr, sizeof(serv_addr));
   serv_addr.sin_family = AF_INET;
   bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   serv_addr.sin_port = htons(PORT_NUMBER);
   if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
      printf("Error while connecting to socket\n");
      exit(1);
   }
   bzero(field, 100);
   if (read(sock, field, 100) < 0) {
      printf("Error reading from socket\n");
      exit(1);
   }
   printBoard();
   close(sock);
   return 0;
}