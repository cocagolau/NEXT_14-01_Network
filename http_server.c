#include "hello_serv.h"

// #define file_sector_size 4096
// #define clnt_size 5

// typedef struct {
// 	int clnt_num;
// 	int start_pos;
// } file_sector_t;

http_server_t * http_server_init(int port, pthread_mutex_t * mutx) {
	http_server_t * new_server = (http_server_t *) malloc (sizeof(http_server_t) * 1);

	if (new_server != NULL) {
		new_server->sock = unix_tcp_create_socket();
		unix_tcp_allocate_address(&new_server->addr, port);

		// bind
		unix_tcp_bind(new_server->sock, (struct sockaddr *) &new_server->addr, sizeof(new_server->addr));

		// listen
		unix_tcp_listen(new_server->sock);

		// mutex init
		new_server->mutx = mutx;

		// client buffer init
		// unix_buffer_int_init(&new_server->clients);
	}

	return new_server;
}

// void http_server_service(http_server_t * server) {


// }

void http_server_destroy(http_server_t * server) {
	close(server->sock);
	free(server);
}

// void http_server_add_client(http_server_t * server, int sock) {
// 	pthread_mutex_lock(server->mutx);
// 	server->clients[server->clients_sz++] = sock;
// 	pthread_mutex_unlock(server->mutx);
// }

// void http_server_remove_client(http_server_t * server, int sock) {
// 	int idx = http_server_index_of_client(server, sock);
// 	pthread_mutex_lock(server->mutx);

// 	if (idx != -1) {
// 		server->clients[idx] = server->clients[server->clients_sz];
// 		server->clients[server->clients_sz--] = 0;
// 	}
// 	pthread_mutex_unlock(server->mutx);
// }

// int http_server_index_of_client(http_server_t * server, int sock) {
// 	int i, result_idx;

// 	pthread_mutex_lock(server->mutx);
	
// 	for (i = 0; i < server->clients_sz; i++) {
// 		if (server->clients[i] == sock) {
// 			result_idx = i;
// 			break;
// 		}
// 	}
// 	result_idx = -1;
// 	pthread_mutex_unlock(server->mutx);

// 	return result_idx;	
// }