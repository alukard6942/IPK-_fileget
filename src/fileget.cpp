/**
 * File: fileget.cpp
 * Author: alukard <alukard@github>
 * Date: 25.03.2021
 */

#include <string.h>
#include <unistd.h>
#include <err.h>

#include "log.h"
#include "server.h"

int main(int argc, char **argv){

	
	// --- arg parsing --------------
	std::string address;
	std::string file;
	for (int i =1; i< argc; i++){
		if (!strcmp(argv[i], "-n") && i+1 < argc){
			address = argv[++i];
		}
		else if (!strcmp(argv[i], "-f") && i+1 < argc){
			file = argv[++i];
		}
		else {
			fprintf(stderr, "nerozumim argumntu %s", argv[i]);
		}
	}
	if (address.empty())
		err(4, "chybi addresa");

	// --- geting fileserver
	auto udp = new Server_UDP(address);

	auto tcp = udp->file_server_of(file);

	tcp->download(file);

	delete udp;
	delete tcp;

	LOG(tcp)

	return 0;
}
