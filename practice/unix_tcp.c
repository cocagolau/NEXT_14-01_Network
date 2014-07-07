#include "hello_serv.h"

int unix_tcp_create_socket(void) {
	return socket(PF_INET, SOCK_STREAM, 0);
}

void unix_tcp_allocate_address(struct sockaddr_in * addr, const int port) {
	memset(addr, 0, sizeof(*addr));
	addr->sin_family = AF_INET;
	addr->sin_addr.s_addr = htonl(INADDR_ANY);
	addr->sin_port = htons(port);

	// bind(server->sock, (struct sockaddr *) addr, sizeof(addr));
}

void unix_tcp_bind(int sock, struct sockaddr * addr, int addr_sz) {
	if (bind(sock, addr, addr_sz) == -1) {
		error_handling("bind() error");
	}
}

void unix_tcp_listen(int sock) {
	if (listen(sock, 5) == -1) {
		error_handling("listen() error");
	}
}

int unix_tcp_accept(int sock, struct sockaddr_in * sock_addr, int * clnt_sock_size) {
	return accept(sock, (struct sockaddr * ) sock_addr, clnt_sock_size);
}


// clnt_sock = accept ( serv_sock, ( struct sockaddr* ) &clnt_adr, &clnt_adr_size );


void unix_tcp_destroy_safety(int sock) {
	close(sock);
}

/*
// tcp 생성
	unix_tcp_create_socket()

	// tcp에 주소 할당
	unix_tcp_allocate_addresss()
	// service시작
	unix_tcp_listen();

	clnt = unix_tcp_accept();

	// 관련 자원 해제
	unix_tcp_destroy_safety()
*/