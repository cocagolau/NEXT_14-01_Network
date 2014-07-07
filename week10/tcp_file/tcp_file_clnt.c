/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define BUF_SIZE 1024
#define FALSE 0
#define TRUE 1


void error_handling (char * msg);

int main(int argc, char**argv) {
	int sockfd;
	struct sockaddr_in serv_addr;

	int file_length, read_size, i=0;
	FILE * fp;
	char buf[BUF_SIZE] = {0,};

	if (argc != 2) {
		printf("usage: tcp_file_clnt <IP address>\n");
		exit(1);
	}
	
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd == -1) {
		error_handling("socket() error");
	}

	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr=inet_addr(argv[1]);
	serv_addr.sin_port=htons(32000);

	// CONNECT
	if (connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) == -1) {
		error_handling("connect() error");
	}

	printf ("in here\n");

	// OPEN FILE
	fp = fopen("pretest.pdf", "rb");
	if (fp == NULL) {
		error_handling("FILE OPEN ERROR");
	}

	// GET FILE LENGTH
	fseek(fp, 0, SEEK_END);
	file_length = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	printf("file size: %dbyte\n", file_length);
	// write(sockfd, &file_length, 4);
	
	// SEND
	printf ("sending...\n");
	while ((read_size = fread(buf, 1, BUF_SIZE, fp)) > 0) {
		printf ("%d: read_size: %d\n", i++, read_size);
		write(sockfd, buf, read_size);
	}
	printf ("success\n");

	fclose (fp);
	close (sockfd);

	return 0;
}

void error_handling (char * msg) {
	fputs(msg, stderr);
	fputc('\n', stderr);
	exit(1);
}




