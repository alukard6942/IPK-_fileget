/**
 * File: Server_fsp.cpp
 * Author: alukard <alukard@github>
 * Date: 25.03.2021
 */

#include <cstdlib>
#include <string>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <iostream>
#include <string.h>
#include <cerrno>
#include <err.h>
#include <unistd.h>

#include "server.h"
#include "log.h"


// ------ Constructors ---------------------

Server_UDP::Server_UDP(std::string netwrk){
	parse_netw(netwrk); // network -> ip and port

	struct addrinfo hints;
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	int rv;
	if((rv = getaddrinfo(Address, Port, &hints, &Addr))){
			err(1, gai_strerror(rv));
	}

	Socket = socket(Addr->ai_family, Addr->ai_socktype, Addr->ai_protocol);
}

Server_TCP::Server_TCP(std::string netwrk){
	parse_netw(netwrk); // network -> ip and port

	struct addrinfo hints;
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int rv;
	if((rv = getaddrinfo(Address, Port, &hints, &Addr))){
			err(1, gai_strerror(rv));
	}

	Socket = socket(Addr->ai_family, Addr->ai_socktype, Addr->ai_protocol);

	if(connect(Socket, Addr->ai_addr, Addr->ai_addrlen) == -1){
	 	err(1, "didnt connect");
	}
}


Server_UDP::~Server_UDP(){
	freeaddrinfo(Addr);
}

Server_TCP::~Server_TCP(){
	freeaddrinfo(Addr);
}


// ----------- Public ----------------------


std::string Server_UDP::lookup(std::string surl){
	parse_surl(surl); // => Domain, File

	send("WHEREIS "+Domain+"\r\n");
	recv();

	std::string ret = Buffer;

	if( begins(ret,"OK ")){
		return ret.substr(3);
	} 
	else if( begins(ret, "ERR Not Found")){
		err(2, ret.c_str());
	} else {
		err(1, ret.c_str());
	}
}

// download file from server 
// in: SURL (pr: fsp://example.com/index.txt
std::string Server_TCP::selftext(std::string surl){
	parse_surl(surl);

	GET(File, Domain, OWNER); // send request

	recv(); // will only recive header
	std::string header = Buffer; // recv returns size of responce

	if (begins( header, "FSP/1.0 Success")){ // sucess

		int len = parse_len(header); // len of data

		std::string data;

		while(len > 0){
			int bytes = recv();
			len -= bytes;

			data.append(Buffer);
		}

		return data;
		
	} else
	if (begins( header, "FSP/1.0 Not Found")){

	} else
	if (begins( header, "FSP/1.0 Bad Request")){

	} 
	else {
		err(1, header.c_str());
	}
}


int Server_TCP::download(std::string surl){

	auto data = selftext(surl);

}


// ------------ Private --------------------

int Server_TCP::GET(std::string file, std::string host, std::string agent){
	send("GET " + file +" FSP/1.0\r\n"
		"Hostname: "+ host +"\r\n"
		"Agent: "   + agent+"\r\n"
		"\r\n");

	return 1;
}



int Server_UDP::send(std::string message) {
	int didsend;
	int len = message.length()+1;

	char *buffer = (char *)malloc(len+1);
	for (int i = 0; i< len; i++)
		buffer[i] = message[i];
	buffer[len] = '\0';

	do {
		didsend = sendto(Socket, buffer, len, 0, Addr->ai_addr, Addr->ai_addrlen);
	} while(didsend < len);


	free (buffer);
	return 0;
}

int Server_TCP::send(std::string message) {
	int didsend=0;
	int len = message.length()+1;
	char *buffer = (char *)malloc(len+1);
	for (int i = 0; i< len; i++)
		buffer[i] = message[i];
	buffer[len] = '\0';

	do {
		didsend = ::send(Socket, buffer, len, 0);
		if (didsend < 0){
			err(1, "failed to send");
		}
	} while(didsend < len);


	return didsend;
}


int Server_UDP::recv(){
	memset(Buffer, 0, BUFFER);
	return ::recvfrom(Socket, Buffer, BUFFER-1, 0, Addr->ai_addr, &Addr->ai_addrlen);
}
int Server_TCP::recv(){
	memset(Buffer, 0, BUFFER);
	return ::recv(Socket, Buffer, BUFFER-1, 0);
}



void Server::parse_netw(std::string netwrk){
	Netwrk = netwrk;
	int separ = Netwrk.find_last_of(":");
	Port    = (char *)(&Netwrk[0]) + separ+1;
	Address = (char *)(&Netwrk[0]);
	Address[separ] = '\0';
}

void Server::parse_surl(std::string surl){
	std::string begin = "fsp://";
	int begin_l = begin.length();
	if (surl.substr(0, begin_l) != begin){ // wrong imput
		err(3, "SURL musi zacinat fsp://");
	}

	Surl = surl;

	auto tmp = surl.substr(begin_l);

	int separ = tmp.find("/");
	Domain = tmp.substr(0, separ);
	File   = tmp.substr(separ+1);
}


bool Server::begins(std::string origin, std::string prefix){
	return origin.substr(0, prefix.length()) == prefix;
}


int Server_TCP::parse_len(std::string header){

	std::string target = "Length:";

	auto len_st = header.find(target);
	if(!len_st) err(1, "could not determine leng of responce");
	
	std::string len = header.substr(len_st + target.length());

	return std::stoi( len );
}




