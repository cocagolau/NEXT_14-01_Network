#include "hello_serv.h"

void * monitor_main (void * arg) {

	int i;
	// sock_t * clnt_socks = ( sock_t * ) arg;
	// sock_t * temp_sock;
	http_client_buffer_t * clients = (http_client_buffer_t *) arg;
	http_client_t * cur_client;


	while ( 1 ) {
		sleep ( 5 );
		for ( i = 0; i < clients->bufptr; i++ ) {
			cur_client = clients->buf[i];
			printf ("SOCK(%d).sock: %ld \n", i, cur_client->sock);
		}
	}
	printf ( "\n" );


	return NULL;
}