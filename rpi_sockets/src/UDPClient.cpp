#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>
//#include <sys/errno.h>
#include <unistd.h>

#include "UDPClient.h"



UDPClient::UDPClient()
{
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd == -1)
		perror("socket error ...\n");
	printf("sockfd:%d\n", sockfd);
}

UDPClient::~UDPClient()
{
	Close();
}


int UDPClient::Connect(char *remote_addr, uint16_t remote_port)
{
	bzero(&dest_addr, sizeof(dest_addr));
	dest_addr.sin_family = AF_INET;
	dest_addr.sin_port = htons(remote_port);
	if(inet_pton(AF_INET, remote_addr, &dest_addr.sin_addr) < 0)
	{
		perror("address is error...");
		printf("%s\n", remote_addr);
		return -1;
	}
	
	int ret = pthread_create(&pid, NULL, ReceiveData, (void *)this);
	if(ret != 0)
	{
		printf("pthrad create error %s\n", strerror(ret));
		return -1;
	}

	return 1;
}

int UDPClient::Bind(char *local_addr, uint16_t local_port)
{
	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(local_port);
	if(inet_pton(AF_INET, local_addr, &addr.sin_addr) < 0)
	{
		perror("bind address error...\n");
		return -1;
	}

	return bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
}

int UDPClient::Close()
{
	return close(sockfd);
}

int UDPClient::Send(uint8_t *buffer, uint32_t length)
{
	if(sockfd == -1) return -1;
	printf("send...%d\n", length);	

	//return send(sockfd, buffer, length, MSG_NOSIGNAL);
	return sendto(sockfd, buffer, length, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
}

int UDPClient::Send(uint8_t *buffer, uint32_t length, char *remote_addr, uint16_t remote_port)
{
	if(sockfd == -1) return -1;

	struct sockaddr_in addr;
	bzero(&addr, sizeof(addr));
	addr.sin_family = AF_INET;
	addr.sin_port = htons(remote_port);
	if(inet_pton(AF_INET, remote_addr, &addr.sin_addr) < 0)
	{
		perror("remote address error...");
		printf("%s\n", remote_addr);
		return -1;
	}
	return sendto(sockfd, buffer, length, 0, (struct sockaddr*)&addr, sizeof(addr));
}

void *UDPClient::ReceiveData(void *args)
{
	UDPClient *client = (UDPClient*)args;
	
	struct sockaddr_in from_addr;
	int addr_len = sizeof(struct sockaddr_in);

	int len, bsize = 1024;
	client->buffer = (uint8_t *)malloc(sizeof(uint8_t) * (bsize));

	while(1)
	{
		len = recvfrom(client->sockfd, client->buffer, bsize, 0, (struct sockaddr*)&from_addr, (socklen_t*)&addr_len);
		if(len > 0)
		{
			printf("recv length:%d\n", len);
			printf("recv from:%u:%u:%u:%u:%hu\n", 
					((uint8_t*)&(from_addr.sin_addr))[0],
					((uint8_t*)&(from_addr.sin_addr))[1],
					((uint8_t*)&(from_addr.sin_addr))[2],
					((uint8_t*)&(from_addr.sin_addr))[3],
				   	ntohs(from_addr.sin_port));

			if(client->_ReceiveDataCallback != nullptr)
				client->_ReceiveDataCallback(client->buffer, len);
			
			memset(client->buffer, 0x00, bsize);
			memset(&from_addr, 0x00, addr_len);
			continue;
		}
	}

}

