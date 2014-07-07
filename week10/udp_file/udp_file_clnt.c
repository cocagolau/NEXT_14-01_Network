/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define BUF_SIZE 1024

struct packet {
	int num;
	char content[BUF_SIZE];
};

int main(int argc, char**argv) {
	int sockfd;
	int n = 0;
	struct sockaddr_in servaddr,cliaddr;
	struct timeval tv;
	int file_length;
	char buf[BUF_SIZE];
	int response = 0;
	int lost_slice_num;
	struct packet p;
	FILE * f;

	if (argc != 2) {
		printf("usage: udp_file_clnt <IP address>\n");
		exit(1);
	}
	

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
		perror("Error");
	}

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=inet_addr(argv[1]);
	servaddr.sin_port=htons(32000);

	// OPEN FILE
	f = fopen("pretest.pdf", "rb");
	if (f == NULL) {
		printf("FILE OPEN ERROR\n");
		return 0;
	}

	// GET FILE LENGTH
	fseek(f, 0, SEEK_END);
	file_length = ftell(f);
	fseek(f, 0, SEEK_SET);

	printf("file size: %dbyte\n", file_length);

	// SEND FILE SIZE
	sendto(sockfd, &file_length, sizeof(int), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

	// WAIT UNTIL ECHO RESPONSE
	while (recvfrom(sockfd, &response, sizeof(int), 0, NULL, NULL)) {
		printf("connection response: %d \n", response);
		if (response == file_length) {
			break;
		} else {
			// RETRY
			sendto(sockfd, &file_length, sizeof(int), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		}
	}

	// SEND IT ONCE
	printf("file sending...\n");
	while (0 < fread(p.content, 1, BUF_SIZE, f)) {
		p.num = n;
		sendto(sockfd, &p, sizeof(struct packet), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		n++;
	}
	printf("file sended...\n");

	// LOST SIGNAL
	while (recvfrom(sockfd, &lost_slice_num, sizeof(int), 0, NULL, NULL)) {
		if (lost_slice_num == file_length) {
			printf("completed: %d\n", lost_slice_num);
			// SENDED WELL
			break;
		}
		printf("lost: %d\n", lost_slice_num);

		p.num = lost_slice_num;
		fseek(f, lost_slice_num * BUF_SIZE, SEEK_SET);
		fread(p.content, 1, BUF_SIZE, f);
		sendto(sockfd, &p, sizeof(struct packet), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
	}
	return 0;
}




