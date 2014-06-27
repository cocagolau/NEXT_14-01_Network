#include "hello_serv.h"
	

int main ( int argc, char * argv[] ) {

	int serv_sock, clnt_sock;
	int * temp_sock;
	
	sock_t * clnt;
	time_t rawtime;
	int i, j;
	sock_t * clnt_socks [ MAX_SOCK ];

	struct sockaddr_in serv_adr, clnt_adr;
	int clnt_adr_size;
	char buf[BUF_SIZE];

	int state, thread_status;

	pid_t pid;
	pthread_t t_id;
	int num_of_worker = 0;

	extern pthread_mutex_t mutx;


	// initialization
	// clnt_cnt = 0;
	// for ( i=0; i<MAX_CLIENT; i++ ) {
	// 	clnt_socks[i] = 0;
	// }

	if ( argc != 2 ) {
		error_less_argc ( argc, argv );
	}

	// pthread_mutex_init ( &mutx, NULL );
	serv_sock = socket ( PF_INET, SOCK_STREAM, 0 );

	memset ( &serv_adr, 0, sizeof ( serv_adr ) );
	serv_adr.sin_family = AF_INET;
	serv_adr.sin_addr.s_addr = htonl ( INADDR_ANY );
	serv_adr.sin_port = htons ( atoi ( argv[1] ) );

	if ( bind ( serv_sock, ( struct sockaddr* ) &serv_adr, sizeof ( serv_adr ) ) == -1 ) {
		error_handling ( "bind() error" );
	}

	if ( listen ( serv_sock, 5 ) == -1 ) {
		error_handling ( "listen() error" );
	}

	// 타임아웃이 필요한 소켓 확인 
	pthread_create ( &t_id, NULL, monitor_main, ( void * ) clnt_socks );
	pthread_detach ( t_id );

	while ( 1 ) {
		clnt_adr_size = sizeof ( clnt_adr );
		clnt_sock = accept ( serv_sock, ( struct sockaddr* ) &clnt_adr, &clnt_adr_size );


		// client socket을 유지할 메모리 할당 
		// Need : 소켓이 생성된 시간을 담는 구조체 만들 필요가 있음 
		clnt = ( sock_t * ) malloc ( 1 * sizeof ( sock_t ) );
		clnt->sock_fd = clnt_sock;
		time ( &rawtime );
		clnt->last_conn_time = rawtime;

		// 구조체 포인터를 각각 소켓의 디스크립터 번호를 인덱스로 저장
		// need: 소켓을 관리할 새로운 자료구조 필요 / linked_list
		pthread_mutex_lock ( &mutx );
		clnt_socks [ clnt_sock ] = clnt;
		pthread_mutex_unlock ( &mutx );
		

		printf ( "Connected client IP: %s (%d) \n", inet_ntoa ( clnt_adr.sin_addr ), ntohs ( clnt_adr.sin_port ) );
		printf ( "Last Connected time: %ld \n\n", clnt->last_conn_time );

		// pthread_mutex_lock ( &mutx );
		// clnt_socks [ clnt_cnt++ ] = clnt_sock;
		// pthread_mutex_unlock ( &mutx );

		pthread_create ( &t_id, NULL, worker_main, ( void * ) clnt );
		pthread_detach ( t_id );
	}

	close ( serv_sock );

	return 0;
}
