#include "hello_serv.h"


char * get_file_name (char * path) {
	char * first_separated = strtok(path, "/");
	char * second_separated = NULL;

	if (first_separated != NULL) {
		while ( (second_separated = strtok(NULL, "/")) != NULL ) {
			first_separated = second_separated;	
		}	
	}
	else {
		first_separated = "index.html";
	}

	return first_separated;
}


char * content_type ( char * file ) {
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
	else if ( !strcmp ( extenstion, "mkv" ) )
		return "video/x-ms-video";
	else
		return "text/plain";
}

size_t get_file_size (const char * file_name) {
	struct stat sb;
		if (stat (file_name, &sb) != 0) {
			fprintf (stderr, "'stat' failed for '%s': %s.\n", file_name, strerror (errno));
			exit (EXIT_FAILURE);
		}
	return sb.st_size;
}