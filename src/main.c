/**
 * File: main.c
 * Author: xchuda04
 * Date: 31.03.2021
 */
#include <netdb.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define B_SIZE 1024
#define STRING_SIZE 128 

char address[STRING_SIZE]; // udp address
char port   [STRING_SIZE]; // udp port 
char file   [STRING_SIZE]; // fsp file to get
char domain [STRING_SIZE]; // domain name of fsp
char *dummy = "xchuda04";

char buffer[B_SIZE];
char index_buff [B_SIZE];

int socket_fd;
struct addrinfo *res;

/// in: buffer out: new address and port
void parseAddresPort( char *string){
	// last instace of
	char *sep;
	for(char *tmp = string; tmp; tmp = strstr(tmp+1, ":")){
		sep =tmp;
	}
	// left is address righ is port
	memset(address, 0, sizeof(address));
	memcpy(address, string, sep - string);
	strcpy(port, sep+1); 
}

void parseArgs( int argc, char **argv){
	for (int i =1; i< argc; i++){
		if (strcmp(argv[i], "-n") == 0 && ++i < argc){
			parseAddresPort(argv[i]);
		}
		else if (strcmp(argv[i], "-f") == 0 && ++i < argc){
			// find last of /
			char *sep;
			for(char *tmp = argv[i]; tmp; tmp = strstr(tmp+1, "/")){
				sep =tmp;
			}

			// fsp:// => 6 * char
			memset(domain, 0, sizeof(domain));
			memcpy(domain, argv[i] + 6 , sep - argv[i] - 6);
			strcpy(file, sep+1); 
		}
		else {
			fprintf(stderr, "err: %s", argv[i]); // TODO
		}
	}
}
int download_index (){

	char *dwnl = "index";

	if (socket_fd) close(socket_fd);
	socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (socket_fd <= 0){
		fprintf(stderr, "failed to create socket");
		exit (1);
	}

	// tcp => connect rcv and send
	int err = connect(socket_fd, res->ai_addr, res->ai_addrlen);
	if( err == -1){
		fprintf(stderr, "failed to connect");
		exit(1);
	}

	// header as defind in documentaiton
	sprintf(buffer, "GET %s FSP/1.0\r\n"
					"Hostname: %s\r\n"
					"Agent: %s\r\n"
					"\r\n", dwnl, domain, dummy);

	err = send(socket_fd, buffer, strlen(buffer)+1, 0);
	if (err <= 0){
		fprintf(stderr, "failed to send %i", err);
		exit (1);
	}

	// first recive is only header
	memset(buffer, 0, B_SIZE);
	memset(index_buff, 0, B_SIZE);
	err = recv(socket_fd, buffer, B_SIZE, 0);	
	if (err <= 0){
		fprintf(stderr, "failed to recever header");
		exit(1);
	}

	long long Lengh = atoll(strstr(buffer, "Length:") + 7 );

	char *end_of_heder = strstr(buffer, "\r\n\r\n") + 4;

	int end_of_heder_len = strlen(end_of_heder);
	
	int index_index;
	for (index_index = 0; index_index < end_of_heder_len; index_index ++)
		index_buff[index_index] = end_of_heder[index_index];

	
	int recived = end_of_heder_len;
	int rcv_len = err;
	while( (recived < Lengh && rcv_len >= 0) ){ 
		rcv_len = recv(socket_fd, buffer, B_SIZE, 0);

		for (int i; recived < Lengh; i++){
			index_buff[index_index++] = buffer[i];
			recived++;
		}
	}

	return close(socket_fd);
}

int download (char *dwnl){

	if (socket_fd) close(socket_fd);
	socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (socket_fd <= 0){
		fprintf(stderr, "failed to create socket");
		exit (1);
	}

	// tcp => connect rcv and send
	int err = connect(socket_fd, res->ai_addr, res->ai_addrlen);
	if( err == -1){
		fprintf(stderr, "failed to connect");
		exit(1);
	}

	// header as defind in documentaiton
	sprintf(buffer, "GET %s FSP/1.0\r\n"
					"Hostname: %s\r\n"
					"Agent: %s\r\n"
					"\r\n", dwnl, domain, dummy);

	err = send(socket_fd, buffer, strlen(buffer)+1, 0);
	if (err <= 0){
		fprintf(stderr, "failed to send %i", err);
		exit (1);
	}

	// first recive is only header
	memset(buffer, 0, B_SIZE);
	err = recv(socket_fd, buffer, B_SIZE, 0);	
	if (err <= 0){
		fprintf(stderr, "failed to recever header");
		exit(1);
	}

	long long Lengh = atoll(strstr(buffer, "Length:") + 7 );
	
	char *based_mane = dwnl; 
	for (char *tmp = strstr(dwnl+1, "/"); tmp; tmp = strstr(tmp+1, "/")){
		based_mane = tmp+1;
	}

	FILE *fd = fopen(based_mane, "w");
	if ( !fd ){
		fprintf(stderr, "failed to open file\n %s", dwnl);
		exit(1);
	}

	char *end_of_heder = strstr(buffer, "\r\n\r\n") + 4;
	if (end_of_heder - err > 1){
		// data was appended after header
		fwrite(end_of_heder, 1 , err - (int)(end_of_heder - buffer), fd);	// how dum is this fc?
	}

	int rcv_len;
	int recived =0;
	while( rcv_len = recv(socket_fd, buffer, B_SIZE, 0) ){ 

		fwrite(buffer, 1 , rcv_len, fd);	// how dum is this fc?
		recived += rcv_len;

		if (recived >= Lengh || rcv_len <= 0) break;
	}

	fclose(fd);

	close(socket_fd);
}

int main( int argc, char **argv){

	parseArgs( argc, argv);

	// connect to the udp server
	
	struct addrinfo hints;
	//struct addrinfo *res;

	// iniciace hints
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM; // !!

	int err = getaddrinfo(address, port, &hints, &res);
	if (err || !res) { // may heve ben -1
		fprintf(stderr, "address of port not resolved %s", gai_strerror(err));
		exit(1);
	}

	// res -> linked list of candidats
	for (struct addrinfo *node = res; node; node = node->ai_next){
		break;
	}

	int socket_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if (socket_fd <= 0){
		fprintf(stderr, "failed to create socket");
		exit (1);
	}

	

	// udp => sendto and recvfrom
	sprintf(buffer, "WHEREIS %s", domain);
	err = sendto(socket_fd, buffer , strlen(buffer), 0, res->ai_addr, res->ai_addrlen);
	if(err == -1){
		fprintf(stderr, "TODO: err msg"); // TODO
		exit(1);
	}

	err =recvfrom(socket_fd, buffer, B_SIZE, 0, res->ai_addr, &res->ai_addrlen);
	if(err == -1){
		fprintf(stderr, "TODO: err msg"); // TODO
		exit(1);
	}

	close(socket_fd);
	freeaddrinfo(res);

	parseAddresPort(buffer);

	// serch for server is the same
	memset(&hints, 0, sizeof (struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	err = getaddrinfo(address +3, port, &hints, &res);
	if (err || !res) { // may heve ben -1
		fprintf(stderr, "address of port not resolved %s", gai_strerror(err));
		exit(1);
	}

	if (!strcmp(file, "*")){

		download_index();

		char file_buffer[128];
		memset(file_buffer, 0, sizeof(file_buffer));
		int file_index= 0;

		for (int i =0; i < strlen(index_buff); i++){
			if (index_buff[i] == '\r' ){
				if (file_index) download(file_buffer);
				memset(file_buffer, 0, sizeof(file_buffer));
				file_index = 0;
				i++;
			} else if (index_buff[i] == '\n') {
				exit(4);
			} else {
				if (file_index +1 >= sizeof(file_buffer)){ 
					exit (5); 
				}
				file_buffer[file_index++] = index_buff[i];
			}
		}

	} else {
		download(file);
	}


	freeaddrinfo(res);
}
