#include "hello_serv.h"

void * monitor_main ( void * arg ) {

	int i;
	sock_t * clnt_socks = ( sock_t * ) arg;
	sock_t * temp_sock;

	while ( 1 ) {
		sleep ( 5 );
		for ( i = 0; i < MAX_SOCK; i++ ) {
			temp_sock = clnt_socks + i;
			if ( temp_sock != NULL ) {
				if ( temp_sock->sock_fd != 0 )
					printf ( "SOCK(%d).sock_fd: %ld \n", i, temp_sock->sock_fd );
			}
		}
	}
	printf ( "\n" );


	return NULL;
}