#include "jk.h"

void jk_unix_panic(const char* msg) {
	puts(msg);
	exit(-1);
}

int jk_unix_fork() {
	// returns pid if child is created
	// created child process will have value 0
	return fork();
}

int jk_unix_is_parent_pid(int pid) {
	return pid != 0;
}

int jk_unix_is_child_pid(int pid) {
	return pid == 0;
}

unsigned int jk_unix_sleep(unsigned int seconds) {
	int remain_seconds = sleep(seconds);
	// this system call can be interrupted by SIGALARM.
	return remain_seconds;
}

size_t jk_net_send(int sockfd, void* buf, size_t len) {
	// flags: default is 0
	size_t num_bytes = send(sockfd, buf, len, 0);
	return num_bytes;
}

size_t jk_net_recv(int sockfd, void* buf, size_t len) {
	size_t num_bytes = recv(sockfd, buf, len, 0);
	return num_bytes;
}

void jk_transport_address(struct sockaddr_in* socket_details, const char* dotted_addr, int port) {
	socket_details->sin_family = AF_INET;
	socket_details->sin_addr.s_addr = inet_addr (dotted_addr);
	socket_details->sin_port = htons(port);
}

int jk_tcp_create_socket() {
// socket(int sockekt_family, int socket_type, int protocol)
	return socket(AF_INET, SOCK_STREAM, 0);
}

int jk_tcp_connect(int handle, struct sockaddr_in* addr) {
	int err = connect(handle, (struct sockaddr*)addr, sizeof(struct sockaddr));
	switch(err) {
	case 0:
		return 0; // errno
	default:
		jk_unix_panic("tcp_connect error");
		return -1;
	}
}


void jk_unix_init_buffer(struct jk_unix_buffer* buf) {
	buf->bufsiz = 1;
	buf->buf = malloc(buf->bufsiz);
	buf->bufptr = 0;
}

void jk_unix_done_buffer(struct jk_unix_buffer* buf) {
	free(buf->buf);
}

void jk_unix_buffer_putc(struct jk_unix_buffer* buf, int c) {
//	printf("%d %d %c\n", buf->bufsiz, buf->bufptr, c);

	if (buf->bufptr < buf->bufsiz) {
		buf->buf[buf->bufptr++] = c;
	}
	else {
		buf->bufsiz *= 2;
		unsigned char* tmp = malloc(buf->bufsiz);
		for(size_t i = 0; i < buf->bufptr; i++) {
			tmp[i] = buf->buf[i];
		}
		free(buf->buf);
		buf->buf = tmp;
		buf->buf[buf->bufptr++] = c;
	}
}

#define JK_READ_HEADER 0
#define JK_READ_HEADER_CR 1
#define JK_READ_HEADER_CRLF 2
#define JK_READ_HEADER_CRLFCR 3

#define JK_READ_BODY   4


void jk_http_init_client(struct jk_http_client* client) {
	client->state = JK_READ_HEADER;
	jk_unix_init_buffer(&client->header);
	jk_unix_init_buffer(&client->body);
}

int jk_http_client_connect(struct jk_http_client* client, const char* dotted, int port) {
	client->sock = jk_tcp_create_socket();
	jk_transport_address(&client->addr, dotted, port);
	return jk_tcp_connect(client->sock, &client->addr);
}


void jk_http_client_parse_header_line(struct jk_http_client* client, const char* line) {
	char key[BUFSIZ];
	char val[BUFSIZ];
	int keyptr = 0, valptr = 0;
	int state = 0;
	key[0] = val[0] = 0;
	for (int i = 0; line[i]; i++) {
		int c = line[i];
		switch(state) {
		case 0:
			if (c == ':') {
				key[keyptr] = 0;
				state = 1;
			}
			else {
				key[keyptr++] = c;
			}
			break;
		case 1:
			val[valptr++] = c;
			break;
		default:
			printf("Unknown state %d\n", state);
			jk_unix_panic("parse_header_line state");
		}
	}
	val[valptr] = 0;
	if (keyptr > 0 && valptr > 0) {
		printf("key %d = %s\n", keyptr, key);
		printf("val %d = %s\n", valptr, val);
	}
}

void jk_http_client_parse_header(struct jk_http_client* client) {
	char line[BUFSIZ];
	int p = 0;
	int lineptr = 0;
	int state = 0;
	while (p < client->header.bufptr) {
		int c = client->header.buf[p++];
		line[lineptr++] = c;
		switch(state) {
		case 0:
			if (c == 13) {
				state = 1;
			}
			break;
		case 1:
			if (c == 10) {
				state = 0;
				line[lineptr] = 0;
				jk_http_client_parse_header_line(client, line);
				lineptr = 0;
			}
			else {
				state = 0;
			}
			break;
		default:
			printf("Unknown state: %d\n", state);
			jk_unix_panic("state");
		}
	}
}

void jk_http_client_recv(struct jk_http_client* client) {
	char buf[BUFSIZ];

	int num_bytes = jk_net_recv(client->sock, buf, BUFSIZ);
	printf("num_bytes = %d\n", num_bytes);
	for (int i = 0; i < num_bytes; i++) {
		int c = buf[i];

		switch(client->state) {
		case JK_READ_HEADER:
			jk_unix_buffer_putc(&client->header, c);
			switch(c) {
			case 13:
				client->state = JK_READ_HEADER_CR;
				break;
			}
			break;
		case JK_READ_HEADER_CR:
			jk_unix_buffer_putc(&client->header, c);
			switch(c) {
			case 10:
				client->state = JK_READ_HEADER_CRLF;
				break;
			default:
				client->state = JK_READ_HEADER;
			}
			break;
		case JK_READ_HEADER_CRLF:
			jk_unix_buffer_putc(&client->header, c);
			switch(c) {
			case 13:
				client->state = JK_READ_HEADER_CRLFCR;
				break;
			default:
				client->state = JK_READ_HEADER;
			}
			break;
		case JK_READ_HEADER_CRLFCR:
			jk_unix_buffer_putc(&client->header, c);
			switch(c) {
			case 10:
				client->state = JK_READ_BODY;
				jk_unix_buffer_putc(&client->header, 0);
				jk_http_client_parse_header(client);
				break;
			default:
				client->state = JK_READ_HEADER;
			}
			break;
		case JK_READ_BODY:
			break;
		default:
			printf("unknown state %d\n", client->state);
			jk_unix_panic("http_client state_error");
		}
	}
}