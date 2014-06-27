#include "hello_serv.h"

// http_client

http_client_t * http_client_create(pthread_mutex_t * mutx) {
	http_client_t * new_client = (http_client_t * ) malloc (sizeof(http_client_t));

	if (new_client != NULL) {
		new_client->mutx = mutx;
	}

	return new_client;
}

void http_client_destroy(http_client_t * client) {
	if (client != NULL) {
		free(client);
	}
}

void http_client_record_sock(http_client_t * client, int sock) {
	time_t rawtime;
	
	if (client != NULL) {
		time(&rawtime);
		client->last_conn_time = rawtime;
		client->sock = sock;
	}
}


// http_client_buffer
void http_client_buffer_init(http_client_buffer_t * clients) {
	clients->bufsiz = 1;
	clients->buf = (http_client_t **) malloc (clients->bufsiz * sizeof(http_client_t *));
	clients->bufptr = 0;
}

void http_client_buffer_destroy(http_client_buffer_t * clients) {
	if (clients != NULL && clients->buf != NULL) {
		free(clients->buf);
	}
}

int http_client_buffer_extend(http_client_buffer_t * clients) {
	size_t i;
	http_client_t ** new_buf;

	if (clients == NULL) {
		// error handling
		return -1;
	}

	clients->bufsiz *= 2;
	new_buf = (http_client_t **) malloc(clients->bufsiz  * sizeof(http_client_t *));
	for(i = 0; i < clients->bufptr; i++) {
		new_buf[i] = clients->buf[i];
	}
	free(clients->buf);
	clients->buf = new_buf;

	return 0;
}

int http_client_buffer_add(http_client_buffer_t * clients, http_client_t * client) {
	if (clients == NULL || clients->buf == NULL) {
		return -1;
	}

	if (clients->bufptr >= clients->bufsiz) {
		http_client_buffer_extend(clients);
	}
	clients->buf[clients->bufptr++] = client;

	return clients->bufptr;
}

int http_client_buffer_remove(http_client_buffer_t * clients, http_client_t * client) {
	int idx;
	if (clients == NULL || clients->buf == NULL) {
		return -1;
	}

	if ( (idx = http_client_buffer_index_of(clients, client)) != -1) {
		clients->buf[idx] = clients->buf[--clients->bufptr];
		clients->buf[clients->bufptr] = NULL;

		return clients->bufptr;
	}

	return -1;
}


int http_client_buffer_index_of(http_client_buffer_t * clients, http_client_t * client) {
	size_t i;
	http_client_t * cur_client;

	if (clients == NULL || clients->buf == NULL) {
		return -1;
	}

	for (i = 0; i < clients->bufptr; i++) {
		cur_client = clients->buf[i];
		if (cur_client == client) {
			return i;
		}
	}

	return -1;
}