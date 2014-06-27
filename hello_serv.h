#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <time.h>
#include <errno.h>
#include "sys/stat.h"



#define SMALL_BUF 1024
#define BUF_SIZE SMALL_BUF * 4
#define CONTENT_SIZE SMALL_BUF * 10

#define EPOLL_SIZE 1

#define SOCK_CONN 0
#define SOCK_DISCONN 1

#define CR '\r'
#define LF '\n'
#define CRLF "\r\n"


// setting
#define MAX_CLIENT 1000
#define MAX_SOCK_PER_CLNT 10
#define MAX_SOCK 100
#define TIME_OUT 30

#define ROOT_FOLDER "public"
#define SRC_FOLDER "src"
// #define ROOT_DIR "~/Documents/work/webserv"
#define ROOT_DIR "root/"


// struct
typedef struct {
	long sock_fd;
	time_t last_conn_time;
} sock_t;



// request
int request_handler ( int * req_status, FILE * clnt_read, FILE * clnt_write );


// response
void response_error ( FILE * fp );
void response_data ( FILE * fp, char * ct, char * file_name );
// void response_conn_close ( FILE * fp );


// error
void error_less_argc ( int argc, char * argv[] );
void error_handling ( char * message );


// worker thread
void * worker_main ( void * arg );


// disconnection
// void * disconnect_worker ( void * arg );



// monitor
void * monitor_main ( void * arg );

// utility
size_t get_file_size (const char * file_name);
char * get_file_name (char * path);
char * content_type ( char * file );


// int clnt_socks[MAX_CLIENT];
// int clnt_cnt;
pthread_mutex_t mutx;



// unix_tcp
int unix_tcp_create_socket(void);

void unix_tcp_allocate_address(struct sockaddr_in * addr, const int port);

void unix_tcp_bind(int sock, struct sockaddr * addr, int addr_sz);

void unix_tcp_listen(int sock);

int unix_tcp_accept(int sock, struct sockaddr_in * sock_addr, int * clnt_sock_size);

void unix_tcp_destroy_safety(int sock);


// http_server
typedef struct {
	long sock;
	struct sockaddr_in addr;
	pthread_mutex_t * mutx;
	// unix_buffer_t clients[CLIENT_COUNT];

	// int clients[CLIENT_COUNT];
	// int clients_sz;
	// unix_buffer_int_t clients;

} http_server_t;

http_server_t * http_server_init(int port, pthread_mutex_t * mutx);
void http_server_service(http_server_t * server);
void http_server_destroy(http_server_t * server);
// void http_server_add_client(http_server_t * server, int sock);
// void http_server_remove_client(http_server_t * server, int sock);
// int http_server_index_of_client(http_server_t * server, int sock);


// http_header
#define LINE_KEY_SZ 128
#define LINE_VAL_SZ 1024
#define LINE_COUNT 128

typedef struct {
	char key[LINE_KEY_SZ];
	char val[LINE_VAL_SZ];
} Line_t;

typedef struct {
	int count;
	char method[128];
	char location[128];
	char version[128];
	Line_t lines[LINE_COUNT];
} http_header_t;

Line_t http_header_parse_line(char line[]);
Line_t * http_header_find_key(http_header_t * header, char * key);
int http_header_add_line(http_header_t * header, Line_t line);
http_header_t * http_header_init(char header_line[]);
void http_header_destroy(http_header_t * header);
void http_header_parse_header(http_header_t * header, char line[]);


// http_client
typedef struct {
	long sock;
	struct sockaddr_in addr;
	pthread_t thread_id;
	int state;
	pthread_mutex_t * mutx;
	time_t last_conn_time;

	// FILE * read;
	// FILE * write;

	http_header_t * header;
} http_client_t;


http_client_t * http_client_create(pthread_mutex_t * mutx);
void http_client_destroy(http_client_t * client);
void http_client_record_sock(http_client_t * client, int sock);


// http_client_buffer
typedef struct {
	http_client_t ** buf;
	int bufsiz;
	int bufptr;
} http_client_buffer_t;


void http_client_buffer_init(http_client_buffer_t * clients);
void http_client_buffer_destroy(http_client_buffer_t * clients);
int http_client_buffer_extend(http_client_buffer_t * clients);
int http_client_buffer_add(http_client_buffer_t * clients, http_client_t * client);
int http_client_buffer_remove(http_client_buffer_t * clients, http_client_t * client);
int http_client_buffer_index_of(http_client_buffer_t * clients, http_client_t * client);