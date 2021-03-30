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
#include <vector>
#include <cerrno>
#include <err.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "server.h"
#include "log.h"


// ------ Constructors ---------------------

Server_UDP::Server_UDP(std::string netwrk){
	parse_netw(netwrk); // network -> ip and port
	LOG(netwrk)

	struct addrinfo hints;
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	int rv;
	if((rv = getaddrinfo(Address, Port, &hints, &Addr)) || !Addr){
			LOG(Address);
			LOG(Port);
			err(1, "%s", gai_strerror(rv));
	}

	Socket = socket(Addr->ai_family, Addr->ai_socktype, Addr->ai_protocol);
}

Server_TCP::Server_TCP(std::string netwrk, std::string domain){
	parse_netw(netwrk); // network -> ip and port
	Domain = domain;

	struct addrinfo hints;
	memset(&hints, 0, sizeof (hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	int rv;
	if((rv = getaddrinfo(Address, Port, &hints, &Addr)) || !Addr){
			LOG(Address);
			LOG(Port);
			err(1, "%s", gai_strerror(rv));
	}

}


Server_UDP::~Server_UDP(){
	LOG(Addr);
	freeaddrinfo(Addr);
}

Server_TCP::~Server_TCP(){
	freeaddrinfo(Addr);
	LOG(Addr);
	if(Socket) close(Socket);
}


// ----------- Public ----------------------


std::string Server_UDP::lookup(std::string surl){
	parse_surl(surl); // => Domain, File

	LOG(Domain)
	send("WHEREIS "+Domain+"\r\n");
	recv();
	LOG(Buffer);

	std::string ret = Buffer;

	if( begins(ret,"OK ")){
		return ret.substr(3);
	} 
	else if( begins(ret, "ERR Not Found")){
		err(2, "%s", ret.c_str());
	} else {
		err(1, "%s", ret.c_str());
	}
}

Server_TCP *Server_UDP::file_server_of (std::string surl){
	parse_surl(surl);

	auto tcp_addres = lookup(surl);

	auto tcp = new Server_TCP(tcp_addres, Domain);	

	return tcp;
}

// download file from server 
// in: SURL (pr: fsp://example.com/index.txt
std::string Server_TCP::selftext(std::string surl){
	parse_surl(surl); 

	connect();

	GET(File, Domain, OWNER); // send request

	recv(); // will only recive header
	std::string header = Buffer; // recv returns size of responce

	check_header(header);

	int len = parse_len(header); // len of data

	std::string data;

	int bytes = data_form_header(header);
	
	while(42){
		len -= bytes;
		data.reserve(bytes);
		for (int i=0; i < bytes; i++){
			char tmp = Buffer[i];
			data.push_back(tmp);
		}

		if (len <= 0) break;


		bytes = recv();
		if (bytes <= 0){
			LOG(bytes)
			LOG(File)
			LOG(Buffer)
			err(1, " didnt recv");
		}
	}
	
	return data;
}
int Server_TCP::download(std::string surl){
	parse_surl(surl);


	LOG(File);

	if (File == "*"){
		return download_all();
	} else {
		return download_file(surl);
	}
}


int Server_TCP::download_file(std::string surl){

	connect();

	parse_surl(surl); 

	LOG(File)
	LOG(Domain)

	GET(File, Domain, OWNER); // send request


	recv(); // will only recive header
	std::string header = Buffer; // recv returns size of responce
	LOG(header)

	check_header(header);

	int len = parse_len(header); // len of data
	LOG(len)
	
	FILE *file = fopen(&basename(File)[0], "w");
	if (!file){
		err(139, "could open file %s", &File[0]);
	}

	int bytes = data_form_header(header);
	
	while(1){
		len -= bytes;
		if (bytes) fwrite( Buffer, bytes, 1, file );
		LOG(BUFFER)
		
		if (len <= 0) break;

		bytes = recv();
		if (bytes <= 0){
			LOG(bytes)
			LOG(File)
			LOG(Buffer)
			err(1, " didnt recv");
		}
	}

	fclose(file);

	return 1;
}

int Server_TCP::download_all(){

	auto dir = index(); 

	for (auto f : dir){
		LOG("fsp://"+Domain+"/"+f)
		download_file("fsp://"+Domain+"/"+f);
	}

	return 0;
}

// predpoklada ze nechceme aby se zaroven jeste stahoval index a my stahovali nova data
std::vector<std::string> Server_TCP::index (){

 	auto resp = selftext("fsp://"+Domain+"/index");

	std::vector<std::string> index;

	std::string colector;
	for (unsigned long i = 0; i< resp.length(); i++) switch(resp[i]){
		case '\r':
		case '\n':
		case ' ':
			if( !colector.empty() )
				index.push_back(colector);
			colector = "";
			break;
		default:
			colector.push_back(resp[i]);
	}

	return index;
}


// ------------ Private --------------------

void Server_TCP::connect(){
	if (Socket) close(Socket);

	Socket = socket(Addr->ai_family, Addr->ai_socktype, Addr->ai_protocol);

	if(::connect(Socket, Addr->ai_addr, Addr->ai_addrlen) == -1){
	 	err(1, "didnt connect");
	}
}

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


	free(buffer);
	return didsend;
}


int Server_UDP::recv(){

	// this somehow makes it so there is a timeout for recv
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	int tmp = ::recvfrom(Socket, Buffer, BUFFER-2, 0, Addr->ai_addr, &Addr->ai_addrlen);
	Buffer[tmp] = 0;
	return tmp;
}
int Server_TCP::recv(){

	// this somehow makes it so there is a timeout for recv
	struct timeval tv;
	tv.tv_sec = 5;
	tv.tv_usec = 0;
	setsockopt(Socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	int tmp = ::recv(Socket, Buffer, BUFFER-2, 0);
	if (tmp <= 0 || tmp > BUFFER -2){ err(2, "no recv");}
	Buffer[tmp] = 0;
	return tmp;
}

void Server_TCP::check_header(std::string header){

	LOG(header);

	if (begins( header, "FSP/1.0 Success")){ // sucess
		return;
		
	} else
	if (begins( header, "FSP/1.0 Not Found")){
		err(1, "not found %s", header.c_str());

	} else
	if (begins( header, "FSP/1.0 Bad Request")){
		err(1, "bad request %s", header.c_str());
	} 
	else {
		err(404, "no header found");
	}
	
}

std::string Server_TCP::basename (std::string filename){
	return (filename.substr(filename.find_last_of("/")+1));
}

void Server::parse_netw(std::string netwrk){

	Netwrk = netwrk;
	int separ = netwrk.find_last_of(":");
	if (separ <= 0){
		err(3, "da fuck");
	}
	memcpy(Address, &netwrk[0], separ);
	Address[separ] = 0;

	int port_l = netwrk.length() - separ;
	memcpy(Port, &netwrk[separ+1], port_l );
	Port[port_l] = 0;
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

int Server_TCP::data_form_header(std::string header){
	// cuold just find the first intstance of 
	std::string target = "\r\n\r\n";
	int start = header.find(target) + target.length();
	int ret   = header.length() - start;

	LOG(start);

	for(int i= 0; i < ret+1; i++){
		Buffer[i] = Buffer[i +start];
	}

	LOG(ret)
	return ret;
}

long long Server_TCP::parse_len(std::string header){

	std::string target = "Length:";

	auto len_st = header.find(target);
	if(!len_st) err(1, "could not determine leng of responce");
	
	int from = len_st + target.length();
	std::string len = header.substr(from, header.find('\n', from));

	return std::stoll( len );
}




