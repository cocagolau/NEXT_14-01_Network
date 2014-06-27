#include "hello_serv.h"
	

int main ( int argc, char * argv[] ) {
	// http_client_t * clnt_socks[MAX_SOCK];
	// http_client_buffer_t clients;
	// int state, thread_status;

	pthread_t t_id;
	int clnt_adr_size;
	http_client_t * new_client;
	http_server_t * server;

	// extern pthread_mutex_t mutx;

	if (argc != 2) {
		error_less_argc(argc, argv);
	}

	
	// server = http_server_init(atoi(argv[1]), &mutx);
	server = http_server_init(atoi(argv[1]));
	// http_client_buffer_init(&clients);

	// 타임아웃이 필요한 소켓 확인 
	pthread_create(&t_id, NULL, monitor_main, (void *)&server->clients);
	pthread_detach(t_id);

	while ( 1 ) {
		new_client = http_client_create(server->mutx);
		clnt_adr_size = sizeof(new_client->addr);

		// client socket을 유지할 메모리 할당 
		// Need : 소켓이 생성된 시간을 담는 구조체 만들 필요가 있음 
		http_client_record_sock(new_client, accept(server->sock, (struct sockaddr *) &new_client->addr, &clnt_adr_size));

		
		// 구조체 포인터를 각각 소켓의 디스크립터 번호를 인덱스로 저장
		// need: 소켓을 관리할 새로운 자료구조 필요 / linked_list

		http_server_add_client(server, new_client);

		// pthread_mutex_lock(server->mutx);
		// http_client_buffer_add(&clients, new_client);
		// pthread_mutex_unlock(server->mutx);
		

		printf ( "Connected client IP: %s (%d) \n", inet_ntoa ( new_client->addr.sin_addr ), ntohs ( new_client->addr.sin_port ) );
		printf ( "Last Connected time: %ld \n\n", new_client->last_conn_time );

		pthread_create(&t_id, NULL, worker_main, (void *)new_client);
		new_client->thread_id = t_id;
		pthread_detach(t_id);
	}

	
	// close(server->sock);
	http_server_destroy(server);

	return 0;
}
