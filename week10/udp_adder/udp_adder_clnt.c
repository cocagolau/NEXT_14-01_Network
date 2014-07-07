/* Sample UDP client */

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define FALSE 0
#define TRUE 1

int main(int argc, char**argv) {
	int sockfd,n;
	struct sockaddr_in servaddr,cliaddr;
	struct timeval tv;
	// char sendline[1000000];
	// char recvline[1000000];
	// char buf[BUFSIZ];
	int flag = FALSE;

	int num1[2] = {1,};
	int num2[2] = {2,};
	int * nums[2] = {num1, num2};
	int recv_nums[2] = {0,};
	int result;

	if (argc != 4) {
		printf("usage: ex04c <IP address> <num1> <num2>\n");
		exit(1);
	}
	printf ("num1: %d, num2: %d \n", atoi(argv[2]), atoi(argv[3]));
	num1[1] = atoi(argv[2]);
	num2[1] = atoi(argv[3]);
	

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

	sendto(sockfd, nums[0], sizeof(int)*2, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
	sendto(sockfd, nums[1], sizeof(int)*2, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

	// send
	while (recvfrom(sockfd, &recv_nums, 8, 0, NULL, NULL)) {
		int code = recv_nums[0];
		int value = recv_nums[1];

		if (code == 0) break;
		else {
			printf ("request again: %d\n", code+1);
			sendto(sockfd, nums[code-1], sizeof(int)*2, 0, (struct sockaddr *)&servaddr, sizeof(servaddr));
		}		
	}
	printf ("send success !! \n\n");

	// receive
	n = recvfrom(sockfd, &result, 4, 0, NULL, NULL);
	printf ("result: %d", result);

	return 0;
}




