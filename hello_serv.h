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



// request
int request_handler ( int * req_status, FILE * clnt_read, FILE * clnt_write );



// response
void response_error ( FILE * fp );
void response_data ( FILE * fp, char * ct, char * file_name );

char * content_type ( char * file );
// void response_conn_close ( FILE * fp );


// error
void error_less_argc ( int argc, char * argv[] );
void error_handling ( char * message );


// worker thread
void * worker_main ( void * arg );


// disconnection
// void * disconnect_worker ( void * arg );

// utility


// monitor
void * monitor_main ( void * arg );

// int clnt_socks[MAX_CLIENT];
// int clnt_cnt;
pthread_mutex_t mutx;

// struct

typedef struct {
	long sock_fd;
	time_t last_conn_time;	
} sock_t;