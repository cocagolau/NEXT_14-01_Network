#include "hello_serv.h"


int request_handler ( int * req_status, FILE * clnt_read, FILE * clnt_write ) {
	
	int i = 0;
	char req_line [ BUF_SIZE ];
	

	char method [ 10 ];
	char con_type [ 15 ];
	char file_name [ 30 ];

	// request 상태 초기화
	*req_status = 0;
	
	// header의 첫번째 라인 읽기
	if ( fgets ( req_line, BUF_SIZE, clnt_read ) == NULL )
		return SOCK_DISCONN;


	// confirm HTTP request
	if ( strstr ( req_line, "HTTP/" ) == NULL ) {
		*req_status = 1;	
		return SOCK_CONN;
	}

	strcpy ( method, strtok ( req_line, " /" ) );
	strcpy ( file_name, strtok ( NULL, " /" ) );
	strcpy ( con_type, content_type ( file_name ) );


	if ( strcmp ( method, "POST" ) == 0 ) {
		return SOCK_DISCONN;
	}
	// else if ( strcmp ( method, "GET" ) == 0 ) {
		
	// }

	// 2번째 라인부터 헤더 읽기
	while ( fgets ( req_line, BUF_SIZE, clnt_read ) ) {
		if ( strcmp ( req_line, CRLF ) == 0 ) {
			memset ( req_line, 0, sizeof ( req_line ) );
			break;
		}
	}
	response_data ( clnt_write, con_type, file_name );

	return SOCK_CONN;
	
}