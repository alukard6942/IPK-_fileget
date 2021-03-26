#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdio.h>
#include <iostream>

int main ( int argc, char **argv ){

	// get user args
	char *addres = NULL;
	char *port = NULL;

	for (int i =1; i< argc; i++){
		if (!strcmp(argv[i], "-a") && ++i < argc){
			addres = argv[i];
			printf("addres: %s\n", argv[i]);
		}
		else if (!strcmp(argv[i], "-p") && ++i < argc){
			port = argv[i];
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
	hints.ai_protocol = 0;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_CANONNAME;

	// get linked list of posible results from hints
	int errcode;
	if ((errcode = getaddrinfo(addres, port, &hints, &result)) != 0){
		fprintf(stderr, "getaddrinfo vraci err: %i \n%s", errcode, gai_strerror(errcode));
		return errcode;
	}

	for (auto e = result; e; e = e->ai_next){

		void *addr;

		// pretipovani pode IPv?
		if (e->ai_family == AF_INET){ // ipv4
			struct sockaddr_in *ipv4 = (struct sockaddr_in *) e->ai_addr;
			addr = &(ipv4->sin_addr);
		}
		else { // ipv6
			struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) e->ai_addr;
			addr = &(ipv6->sin6_addr);
		}

		char ip_s[INET6_ADDRSTRLEN];
		inet_ntop(e->ai_family, addr, ip_s, sizeof(ip_s));
		printf(" ip addresa je: %s\n", ip_s); 

	}



	// fee the linked list
	freeaddrinfo(result);

}
