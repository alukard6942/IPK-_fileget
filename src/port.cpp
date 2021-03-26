#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <cerrno>
#include <err.h>

int main ( int argc, char **argv ){


	//  input
	std::string addres = "127.0.0.1";
	std::string port   = "6966";
	for (int i =1; i< argc; i++){
		if (!strcmp(argv[i], "-a") && i+1 < argc){
			addres = argv[++i];
			printf("addres: %s\n", argv[i]);
		}
		else if (!strcmp(argv[i], "-p") && i+1 < argc){
			port = argv[++i];
			printf("port: %s\n", argv[i]);
		}
		else {
			fprintf(stderr, "nerozumim argumntu %s", argv[i]);
		}
	}

	struct addrinfo hints;
	struct addrinfo *result;

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	// 		adress init	
	int errcode;
	if ((errcode = getaddrinfo(addres.c_str(), port.c_str(), &hints, &result)) != 0){
		std::cerr<< gai_strerror(errcode) << std::endl;
		return errcode;
	}

	auto sock = socket( result->ai_family, result->ai_socktype, result->ai_protocol);

	std::cout<< connect(sock, result->ai_addr, result->ai_addrlen);


	//if( bind (sock, result->ai_addr, result->ai_addrlen) == -1)
	//if( connect(sock, result->ai_addr, result->ai_addrlen) == -1)
	//	err (1, "unenagle to connect");
	//
	//listen(sock, 1);

	// fee the linked list
	freeaddrinfo(result);

}
