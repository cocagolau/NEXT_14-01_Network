#include "unix_buffer.h"

void unix_buffer_init(unix_buffer_t * buf) {
	buf->bufsiz = 1;
	buf->buf = malloc(buf->bufsiz);
	buf->bufptr = 0;
}

void unix_buffer_destroy(unix_buffer_t * buf) {
	if (buf != NULL && buf->buf != NULL) {
		free(buf->buf);
	}
}

int unix_buffer_extend(unix_buffer_t * buf) {
	size_t i;
	unsigned char * tmp;

	if (buf == NULL) {
		// error handling
		return -1;
	}

	buf->bufsiz *= 2;
	tmp = malloc(buf->bufsiz);
	for(i = 0; i < buf->bufptr; i++) {
		tmp[i] = buf->buf[i];
	}
	free(buf->buf);
	buf->buf = tmp;

	return 0;

}

void unix_buffer_putc(unix_buffer_t * buf, int c) {
	if (buf->bufptr >= buf->bufsiz) {
		unix_buffer_extend(buf);
	}
	
	buf->buf[buf->bufptr++] = c;
}
