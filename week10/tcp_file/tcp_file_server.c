#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define SIZE 4096


int main(int argc, char**argv) {
	int sockfd, clisock, n;
	struct sockaddr_in servaddr,cliaddr;
	struct timeval tv;
	socklen_t cliaddr_size;

	FILE * fp_dest;
	FILE * clnt_readf;
	char sock_buf[SIZE];
	char * file_size;
	int ifile_size;
	char * file_buf;
	int * file_index_buf;

	int cur_byte;

	// socket
	sockfd = socket(AF_INET,SOCK_STREAM,0);
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(32000);

	if (bind(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) == -1)
		printf("bind() error");
	if (listen(sockfd, 5) == -1)
		printf("listen() error");

	fp_dest = fopen( "./reading01.pdf", "wb");
	cliaddr_size = sizeof(cliaddr);
	clisock = accept(sockfd, (struct sockaddr*) &cliaddr, &cliaddr_size);
	printf("connected! \n");
	
	clnt_readf = fdopen(clisock, "rb");
	printf("ok..\n");

/*	fgets(file_size, sizeof(int), clnt_readf);
	ifile_size = atoi(file_size);
	printf("file size: %d\n", ifile_size);
	
	if (ifile_size == 0) {
		fclose(clnt_readf);
		return -1;
	}
*/
	while(1) {
		memset(sock_buf, 0, SIZE);
		n = fread(sock_buf, 1, SIZE, clnt_readf);
		printf("get: %d\n", n);
		if (n <= 0) {
			break;
		}
		fwrite(sock_buf, 1, n, fp_dest);
	}

	fclose(clnt_readf);
	fclose(fp_dest);
	
	return 0;
}