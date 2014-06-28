#include "hello_serv.h"

// #define file_sector_size 4096
// #define clnt_size 5

// typedef struct {
// 	int clnt_num;
// 	int start_pos;
// } file_sector_t;

// http_server_t * http_server_init(int port, pthread_mutex_t * mutx) {
http_server_t * http_server_init(int port) {
	http_server_t * new_server = (http_server_t *) malloc (1 * sizeof(http_server_t));
	pthread_mutex_t * mutx;

	if (new_server != NULL) {
		new_server->sock = unix_tcp_create_socket();
		unix_tcp_allocate_address(&new_server->addr, port);

		// bind
		unix_tcp_bind(new_server->sock, (struct sockaddr *) &new_server->addr, sizeof(new_server->addr));

		// listen
		unix_tcp_listen(new_server->sock);

		// mutex init
		mutx = (pthread_mutex_t *) malloc (1 * sizeof(pthread_mutex_t));
		if (mutx != NULL) {
			new_server->mutx = mutx;
			pthread_mutex_init (mutx, NULL);
		}

		// client buffer init
		http_client_buffer_init(&new_server->clients);
	}

	return new_server;
}

void http_server_service(http_server_t * server) {
	pthread_t t_id;
	int clnt_adr_size;
	http_client_t * new_client;

	// 타임아웃이 필요한 소켓 확인 
	// pthread_create(&t_id, NULL, monitor_main, (void *)&server->clients);
	pthread_create(&t_id, NULL, http_server_monitor_main, (void *)server);
	pthread_detach(t_id);

	while ( 1 ) {
		new_client = http_client_create(server->mutx);
		clnt_adr_size = sizeof(new_client->addr);

		// client socket을 유지할 메모리 할당 
		// Need : 소켓이 생성된 시간을 담는 구조체 만들 필요가 있음 
		http_client_record_sock(new_client, accept(server->sock, (struct sockaddr *) &new_client->addr, &clnt_adr_size));

		
		// 구조체 포인터를 각각 소켓의 디스크립터 번호를 인덱스로 저장
		// need: 소켓을 관리할 새로운 자료구조 필요 / linked_list
		http_server_monitor_add_client(server, new_client);

		printf("Connected client IP: %s (%d) \n", inet_ntoa (new_client->addr.sin_addr), ntohs (new_client->addr.sin_port));
		printf("Last Connected time: %ld \n\n", new_client->last_conn_time );

		// pthread_create(&t_id, NULL, worker_main, (void *)new_client);
		pthread_create(&t_id, NULL, http_client_worker_main, (void *)new_client);
		new_client->thread_id = t_id;
		pthread_detach(t_id);
	}
}

void http_server_destroy(http_server_t * server) {
	
	if (server !=NULL ) {
		if (server->mutx != NULL) {
			free (server->mutx);
		}

		close(server->sock);
		free(server);
	}
}

void http_server_monitor_add_client(http_server_t * server, http_client_t * client) {
	pthread_mutex_lock(server->mutx);
	http_client_buffer_add(&server->clients, client);
	pthread_mutex_unlock(server->mutx);
}

void http_server_monitor_remove_client(http_server_t * server, http_client_t * client) {
	pthread_mutex_lock(server->mutx);
	http_client_buffer_remove(&server->clients, client);
	pthread_mutex_unlock(server->mutx);
}


// http_client_monitor
void * http_server_monitor_main (void * arg) {
	int i;
	time_t cur_time;
	// http_client_buffer_t * clients = (http_client_buffer_t *) arg;
	http_server_t * server = (http_server_t *) arg;
	http_client_buffer_t * clients = &server->clients;

	http_client_t * cur_client;

	// 15초가 지난 소켓은 종료하도록	
	while (1) {
		time(&cur_time);
		sleep (5);
		printf ("monitor -time: %ld \n", cur_time);
		for (i = 0; i < clients->bufptr; i++) {
			cur_client = clients->buf[i];
			printf ("SOCK(%ld) in clients[%d] \n", cur_client->sock, i);

			if (cur_client->state == -1) {
				http_client_destroy(server, cur_client);
			}
		}
	}
	printf ("------------------------\n\n");


	return NULL;
}
