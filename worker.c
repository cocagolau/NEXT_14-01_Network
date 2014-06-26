#include "hello_serv.h"

void * worker_main ( void * arg ) {

	// extern int clnt_socks[MAX_CLIENT];
	// extern int clnt_cnt;
	// extern pthread_mutex_t mutx;

	sock_t * clnt = ( sock_t * ) arg;

	int clnt_sock = clnt->sock_fd;

	int str_len = 0, i, req_result = 0, req_status;
	char msg [ BUF_SIZE ];

	FILE * clnt_read;
	FILE * clnt_write;



	printf ( "SOCK(%d) CONN\n", clnt_sock );

	clnt_read = fdopen ( clnt_sock, "r" );
	clnt_write = fdopen ( dup ( clnt_sock ), "wb" );
	

	while ( !req_result ) {
		
		req_result = request_handler ( &req_status, clnt_read, clnt_write );
		
	}

	fclose ( clnt_write );	
	fclose ( clnt_read );
	

	printf ( "SOCK(%d) DISCONN\n", clnt_sock );
	free ( arg );
	
	
	return NULL;
}
