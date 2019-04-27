
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "TCPClient.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
//#include <errno.h>

TCPClient::TCPClient()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("scoket error...1");
		exit(1);
	}
}

TCPClient::~TCPClient()
{
	close(sockfd);
	printf("~TCPClient..\n");
}


TCPClient::TCPClient(char *addr, uint16_t port)
{
}

bool TCPClient::Connect(char *addr, uint16_t port)
{
	printf("connect....\n");

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	if(inet_pton(AF_INET, addr, &servaddr.sin_addr) < 0)
	{
		printf("inet_pton error for %s\n", addr);
		exit(1);
	}

	if(connect(sockfd,(struct sockaddr*)&servaddr, sizeof(servaddr)) < 0)
	{
		perror("connect error.3");
		exit(1);
	}

	return true;
}

bool TCPClient::Send(char *data, uint32_t len)
{
	if(sockfd != -1)
	{
		send(sockfd, data, len, 0);
	}

	return true;
}


