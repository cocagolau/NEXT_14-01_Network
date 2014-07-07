#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void jk_unix_panic(const char* msg);
int  jk_unix_fork();
int  jk_unix_is_parent_pid(int pid);
int  jk_unix_is_child_pid(int pid);
unsigned int jk_unix_sleep(unsigned int seconds);
size_t jk_net_send(int sockfd, void* buf, size_t len);
size_t jk_net_recv(int sockfd, void* buf, size_t len);
void jk_transport_address(struct sockaddr_in* socket_details, const char* dotted_addr, int port);
int jk_tcp_create_socket();
int jk_tcp_connect(int handle, struct sockaddr_in* addr);

struct jk_unix_buffer {
	unsigned char* buf;
	int bufsiz;
	int bufptr;
};

void jk_unix_init_buffer(struct jk_unix_buffer* buf);
void jk_unix_done_buffer(struct jk_unix_buffer* buf);
void jk_unix_buffer_putc(struct jk_unix_buffer* buf, int c);


#define JK_READ_HEADER 0
#define JK_READ_HEADER_CR 1
#define JK_READ_HEADER_CRLF 2
#define JK_READ_HEADER_CRLFCR 3

#define JK_READ_BODY   4

struct jk_http_client {
	int sock;
	struct sockaddr_in addr;
	int state;
	struct jk_unix_buffer header;
	struct jk_unix_buffer body;
};

void jk_http_init_client(struct jk_http_client* client);
int  jk_http_client_connect(struct jk_http_client* client, const char* dotted, int port);
void jk_http_client_parse_header_line(struct jk_http_client* client, const char* line);
void jk_http_client_parse_header(struct jk_http_client* client);
void jk_http_client_recv(struct jk_http_client* client);