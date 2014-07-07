#include "hello_serv.h"

Line_t http_header_parse_line(char line[]) {
	Line_t newLine = {0,};

	strcpy(newLine.key, strtok(line, ": "));
	strcpy(newLine.val, strtok(NULL, "\r\n")+1);

	return newLine;
}

Line_t * http_header_find_key(http_header_t * header, char * key) {
	int count = header->count;
	int i;

	for (i=0; i<count; ++i) {
		if (strcmp(header->lines[i].key, key) == 0) {
			return header->lines + i;
		}
	}

	return NULL;
}

int http_header_add_line(http_header_t * header, Line_t line) {

	header->lines[header->count++] = line;

}

http_header_t * http_header_init(char * header_line) {
	if ( header_line == NULL || strlen ( header_line ) == 0 ) {
		// error_handling
		return NULL;
	}

	http_header_t * header = (http_header_t *) malloc(sizeof(http_header_t));
	if (header == NULL) {
		// error_handling
		return NULL;
	}

	http_header_parse_header(header, header_line);

	return header;
}

void http_header_destroy(http_header_t * header) {

	if (header != NULL) {
		free(header);
	}

}

void http_header_parse_header(http_header_t * header, char line[]) {
	sscanf(line, "%s %s %s", header->method, header->location, header->version );
}