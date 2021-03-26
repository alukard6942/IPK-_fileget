/**
 * File: Server_fsp.h
 * Author: alukard <alukard@github>
 * Date: 25.03.2021
 */

#include <iostream> 
#include <netdb.h>
#include <vector>

#define BUFFER 1024 // 1kb by mohlo stacit
#define OWNER "xkoval18"

class Server {
	protected:
		int send(std::string message);
		int recv();
    	
    	
		std::string Netwrk;
		void parse_netw(std::string network);
		char *Address;
		char *Port;
    	
		std::string Surl;
		void parse_surl(std::string surl);
		std::string Domain;
		std::string File;
    	
		struct addrinfo *Addr;
    	
		char Buffer[BUFFER];
		int Socket;

		bool begins(std::string origin, std::string prefix);
};

class Server_UDP : public Server {
	public:
		Server_UDP(std::string netwrk);
		~Server_UDP();
    	
		std::string lookup (std::string surl);
    	
	private:
    	
		int send(std::string message);
		int recv();
};


class Server_TCP : public Server {
	public:
		Server_TCP(std::string netwrk);
		~Server_TCP();
    	
		std::vector<char> selftext(std::string surl);
		int download(std::string surl);
		int downloadall( std::string surl);
    	
	private:
		int GET(std::string file, std::string host, std::string agent);
    	
		int send(std::string message);
		int recv();

		int parse_len(std::string header);
};
