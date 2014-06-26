#include "hello_serv.h"

void response_conn_close ( FILE * fp ) {

	char protocol [] = "HTTP/1.0 200 OK\r\n";
	char server [] = "Server:Linux Web Server\r\n";
	char cnt_type [] = "Connection:close\r\n\r\n";

	fputs ( protocol, fp );
	fputs ( server, fp );
	fputs ( cnt_type, fp );

}

void response_error ( FILE * fp ) {

	char protocol [] = "HTTP/1.0 400 Bad Request\r\n";
	char server [] = "Server:Linux Web Server\r\n";
	char cnt_len [] = "Content-length:2048\r\n";
	char cnt_type [] = "Content-type:text/html\r\n\r\n";
	char content [] = "<html><head><title>Network</title></head><body><font size = 5><br>error! not found</font></body></html>";

	fputs ( protocol, fp );
	fputs ( server, fp );
	fputs ( cnt_len, fp );
	fputs ( cnt_type, fp );
	fputs ( content, fp );

	fflush ( fp );
}

void response_data ( FILE * fp, char * content_type, char * file_name ) {

	int protocol_status_code = 200;
	char protocol [ BUF_SIZE ];
	char server [] = "Server:Linux Web Server\r\n";
	char cnt_len [ BUF_SIZE ];
	char cnt_type [ BUF_SIZE ];
	char buf [ BUF_SIZE ];
	char connection [] = "Connection: Keep-Alive\r\n";
	char destination[1024];

	FILE * send_file;
	int str_len;
	long content_sz;

	sprintf ( protocol, "HTTP/1.0 %d OK\r\n", protocol_status_code );
	sprintf ( cnt_type, "Content-type:%s\r\n\r\n", content_type );


	// printf ( "file_name: %s \n", file_name );
	strcpy(destination, ROOT_DIR);
	strcat(destination, file_name);
	// printf ("dest: %s\n", destination);

	// send_file = fopen ( file_name, "rb" );
	send_file = fopen ( destination, "rb" );

	// content size 측정
	fseek ( send_file, 0L, SEEK_END );
	content_sz = ftell ( send_file );
	printf ("filesz: %ld\n", content_sz);
	fseek ( send_file, 0L, SEEK_SET );

	sprintf ( cnt_len, "Content-length:%ld\r\n", content_sz );

	fputs ( protocol, fp );
	fputs ( server, fp );
	fputs ( cnt_len, fp );
	fputs ( connection, fp );
	fputs ( cnt_type, fp );
	fflush ( fp );

	while ( ( str_len = fread ( buf, 1, BUF_SIZE, send_file ) ) > 0 ) {
		fwrite ( buf, 1, str_len, fp );
		memset ( buf, 0, sizeof ( buf ) );
		fflush ( fp );
	}

	fclose(send_file);
	
}

char * content_type ( char * file ) {
	printf ("path: %s\n", file);

	char extenstion [ BUF_SIZE ];
	char file_name [ BUF_SIZE ];

	strcpy ( file_name, file );
	strtok ( file_name, "." );
	strcpy ( extenstion, strtok ( NULL, "." ) );

	if ( !strcmp ( extenstion, "html" ) || !strcmp ( extenstion, "htm" ) )
		return "text/html";
	else if ( !strcmp ( extenstion, "png" ) )
		return "image/png";
	else if ( !strcmp ( extenstion, "pdf" ) )
		return "application/pdf";
	else
		return "text/plain";
}