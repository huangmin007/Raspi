
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "TCPClient.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>


TCPClient::TCPClient()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("scoket error...1");
	}

	printf("sockfd %d\n", sockfd);
}

TCPClient::~TCPClient()
{
	Close();
	printf("~TCPClient..\n");
}

bool TCPClient::Connected()
{
	return connected;
}

int TCPClient::Connect(char *addr, uint16_t port)
{
	printf("sockfd: %d\n", sockfd);
	if(sockfd == -1)
	{
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
		if(sockfd == -1)
		{
			perror("socket init bad ... \n");
			return -1;
		}
	}
	if(connected)	return 1;

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(port);
	if(inet_pton(AF_INET, addr, &servaddr.sin_addr) < 0)
	{
		perror("adderss is error ...\n ");
		printf("address is error: %s\n", addr);
		return -1;
	}
	
	int cr = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if(cr < 0)
	{
		connected = false;
		if(_StatusChangedCallback != nullptr)	_StatusChangedCallback(TCPClientStatus::CONNECT_FAILED);

		return cr;
	}
	
	connected = true;

	//receive data pthread
	int ret = pthread_create(&tid, NULL, ReceiveDataThread, (void *)this);
	if(ret != 0)
	{
		printf("pthread error %s\n", strerror(ret));
		return 0;
	}

	return 1;
}

void *TCPClient::ReceiveDataThread(void *args)
{
	TCPClient *client = (TCPClient *)args;
	
	bool c_receive = true;
	int len, bsize = 1024;
	client->buffer = (uint8_t*)malloc(sizeof(uint8_t) * bsize);
		
	while(c_receive)
	{
		if(!client->connected)continue;
		
		len = read(client->sockfd, client->buffer, bsize);
		
		if(len > 0)
		{
			printf("read length: %d\n", len);
			if(client->_ReceiveDataCallback != nullptr)
				client->_ReceiveDataCallback(client->buffer, len);
			continue;
		}
		
		if(!(len < 0 && errno == EAGAIN))
		{
			client->Close();
			if(client->_StatusChangedCallback != nullptr)
				client->_StatusChangedCallback(TCPClientStatus::CONNECT_CLOSE);

			c_receive = false;
		}
	}

	//return (void*)1;
}

int TCPClient::Send(const uint8_t *data, uint32_t len)
{
	if(sockfd == -1 || !connected) return -1;
	
	return send(sockfd, data, len, MSG_NOSIGNAL);
}

int TCPClient::Close()
{
	if(sockfd == -1 || !connected)	return 1;

	free(buffer);
	buffer = NULL;
	connected = false;

	int c = close(sockfd);
	sockfd = -1;

	pthread_cancel(tid);
	return c;
}

