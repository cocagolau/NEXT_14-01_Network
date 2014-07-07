#include "hello_serv.h"

void error_handling ( char * message ) {
	fputs ( message, stderr );
	fputc ( '\n', stderr );
	exit ( 1 );
}

void error_less_argc ( int argc, char * argv[] ) {
	printf ( "Usage: %s <port>\n", argv[0] );
	exit ( 1 );
}