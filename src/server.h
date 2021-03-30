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
	public:
		std::string Domain;
		char Address[100]; //adresy sou kratke
		char Port   [100];
	protected:
		int send(std::string message);
		int recv();
    	
    	
		std::string Netwrk;
		void parse_netw(std::string network);
    	
		std::string Surl;
		void parse_surl(std::string surl);
		std::string File;
    	
		struct addrinfo *Addr;
    	
		char Buffer[BUFFER];
		int Socket = 0;

		bool begins(std::string origin, std::string prefix);
};

class Server_TCP : public Server {
	public:
		Server_TCP(std::string netwrk, std::string domain);
		~Server_TCP();
    	
		std::vector<std::string> index();
		std::string selftext(std::string surl);
		int download(std::string surl);
		int download_all();
		int download_file( std::string surl);
    	
	private:
		void connect();
		int GET(std::string file, std::string host, std::string agent);

		void check_header(std::string header);
		std::string basename (std::string filename);
    	
		int send(std::string message);
		int recv();

		long long parse_len(std::string header);
		int data_form_header(std::string header);
};

class Server_UDP : public Server {
	public:
		Server_UDP(std::string netwrk);
		~Server_UDP();
    	
		std::string lookup (std::string surl);
		Server_TCP *file_server_of (std::string surl);
    	
	private:
    	
		int send(std::string message);
		int recv();
};
