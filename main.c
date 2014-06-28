#include "hello_serv.h"

int main ( int argc, char * argv[] ) {
	http_server_t * server;

	if (argc != 2) {
		error_less_argc(argc, argv);
	}

	server = http_server_init(atoi(argv[1]));

	http_server_service(server);

	http_server_destroy(server);

	return 0;
}