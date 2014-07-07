#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

#define SIZE 1024

int get_last_idx (int last_idx, int cur_idx);
void binarycpy (char * dest, char * src, int size);

int main(int argc, char**argv) {
	int sockfd,n;
	struct sockaddr_in servaddr,cliaddr;
	struct timeval tv;
	socklen_t cliaddr_size;

	FILE * fp_dest;
	char sock_buf[1028] = {0,};
	char * file_buf;
	int * file_index_buf;


	int num_of_buf;
	int recv_size;
	int last_idx;

	// socket
	sockfd=socket(AF_INET,SOCK_DGRAM,0);

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	servaddr.sin_port=htons(32000);

	bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr));
	tv.tv_sec = 0;
	tv.tv_usec = 100000;
	if (setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,&tv,sizeof(tv)) < 0) {
		perror("Error");
	}

	cliaddr_size = sizeof(cliaddr);
	// file받기 준비 단계, 사이즈를 전달받아 버퍼 준비
	while(1) {
		// 처음 파일크기 받음
		n = recvfrom(sockfd, &recv_size, sizeof(recv_size), 0, (struct sockaddr *) &cliaddr, &cliaddr_size);

		// 받지 못하면 계속 대기
		if (n < 0) continue;

		/*
		 * 받으면 잘 받았다고 1을 전달하고 그 크기만큼 버퍼 준비
		 * file_buf: 파일 데이터
		 * file_index_buf: 파일 데이터의 인덱스
		 */
		num_of_buf = recv_size / SIZE;
		++num_of_buf;

		file_buf = (char * ) malloc (sizeof(char) * num_of_buf * SIZE);
		file_index_buf = (int *) malloc (sizeof(int) * num_of_buf);

		sendto(sockfd, &recv_size, sizeof(int), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
		break;
	}
	
	while (1) {
		int idx = 0;
		int received_count = 0;
		memset (sock_buf, 0, SIZE);
		// 받는 크기는: int + buf[1024]
		n = recvfrom(sockfd, sock_buf, 1028, 0, (struct sockaddr *) &cliaddr, &cliaddr_size);
		printf ("n: %d\n", n);
		// 받은 경우
		if (n > 0) {

			idx = *(int*)sock_buf;
			last_idx = get_last_idx(last_idx, idx);

			/// 받은 인덱스의 번호를 file_index_buf에 저장
			file_index_buf[idx] = 1;
			printf ("file_index[%d]: %d\n", idx, file_index_buf[idx]);

			/// 인덱스의 번호부터 사이즈만큼 file_buf로 데이터 카피
			// strncpy (&file_buf[idx * SIZE], sock_buf+4, SIZE);
			binarycpy (&file_buf[idx * SIZE], sock_buf+4, SIZE);
			printf ("test\n");
		}
		
		// 못 받은 경우
		else {
			/// 받은 인덱스 전까지 순회하면서 못받은 인덱스를 요청
			for (int i=0; i<last_idx; i++) {
				if (file_index_buf[i] == 0)
					sendto(sockfd, &i, sizeof(int), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
			}
		}

		// 완료된경우, 버퍼가 꽉찬 경우
		received_count = 0;
		for (int i=0; i<num_of_buf; i++) {
			if (file_index_buf[i] == 0) {
				received_count++;
			}
		}
		if (received_count == 0) {
			sendto(sockfd, &recv_size, sizeof(int), 0, (struct sockaddr *) &cliaddr, sizeof(cliaddr));
			break;
		}
	}

	// 완료되면 file_buf를 파일로..
	printf ("convertion...\n");

	fp_dest = fopen( "./test.pdf", "wb");
	fwrite( file_buf, 1, recv_size, fp_dest);

	fclose( fp_dest);

	printf ("success!!\n");

	// 메모리 해제
	if (file_buf != NULL) {
		free (file_buf);
	}
	if (file_index_buf != NULL) {
		free (file_index_buf);
	}

	return 0;
}
void binarycpy (char * dest, char * src, int size) {

	for (int i=0; i<size; i++) {
		dest[i] = src[i];
	}

}

int get_last_idx (int last_idx, int cur_idx) {
	return (last_idx < cur_idx)? cur_idx : last_idx;
}



