
#pragma once

#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>


//typedef unsigned short uint16_t;
//typedef unsigned int   uint32_t;

typedef void (*TCPClientDataReceiveCallbacl)();

class TCPClient
{
	public:
		TCPClient();
		~TCPClient();	
		TCPClient(char *addr, uint16_t port);
		bool Connect(char *addr, uint16_t port);
		bool Send(char *data, uint32_t len);

	private:
		int sockfd = -1;
		struct sockaddr_in servaddr;
		char *address;
		uint16_t port;
};


