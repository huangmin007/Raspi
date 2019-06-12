

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include < pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#include "TCPServer.h"


TCPServer::TCPServer()
{
	memset(client_fds, -1, MAX_CONNECT_COUNT);
	listen_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(listen_fd < 0)
	{
		perror("Server create socket error...");
		printf("%s\n", listen_fd);
		exit(1);
	}
}

TCPServer::~TCPServer()
{
}

bool TCPServer::Bind(uint16_t port)
{
	if(listen_fd < 0)
	{
		printf("Socket init failed...\n");
		return false;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	bind_ret = bind(listen_fd, (struct sockaddr*)&servaddr, sizeof(servaddr));
	if(bind_ret < 0)
	{
		printf("Bind Error ... \n");
		return false;
	}

	return true;
}

bool TCPServer::Start()
{
	if(listen_fd < 0 || bind_ret < 0)
	{
		printf("Start Failed ... \n");
		return false;
	}

	if(listen(listen_fd, LISTENQ) < 0)
	{
		printf("Start Failed, Listen error ... \n");
		return false;
	}

	int ret = pthread_create(&listen_pt, NULL, ServerAcceptClient, (void *)this);
	if(ret != 0)
	{
		printf("Start Failed, Create accept pthread error ... \n");
		return false;
	}
	
	return true;
}

bool TCPServer::Stop()
{

}

int TCPServer::Boardcast(uint8_t *data, uint32_t length)
{
	return 0;
}

void TCPServer::ServerAcceptClient(void *arags)
{
	TCPServer *server = (TCPServer*)args;

	int client_fd;
	sockaddr_in client_addr;
	socklen_t csize = sizeof(sockaddr_in);

	while(server->listen_fd > 0)
	{
		client_fd = accept(server->listen_fd, (struct sockaddr*)&client_addr, &csize));
		if(client_fd < 0)
		{
			perror("Accept error ...\n");
			continue;
		}

		//查找空位，检查客户端列表数量是否超出
		int index = GetClientIndex();
		if(index < 0 || index > MAX_CLIENT_COUNT)
		{
			close(client_fd);
			perror("connect clients out of range count ..\n");
			continue;
		}

		printf("server got connection from:%s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
		
		//创建客户端数据接收线程
		pthread_t client_pt;
		if(pthread_create(&client_pt, NULL, ClientReceiveData, (void *)client_fd) < 0)
		{
			perror("Accept Error, client create pthread error...\n");
			continue;
		}
		
		//
		client_fds[index] = client_fd;
		client_pts[index] = client_pt;
	}
}

void TCPServer::ClientReceiveData(void *args)
{
	int client_fd = *(int*)args;

	struct sockaddr_in from_addr;
	socklen_t from_len = sizeof(from_addr);

	bool running = true;
	int length, bsize = CLIENT_BUFFER;
	uint8_t *buffer = (uint8_t8)malloc(sizeof(uint8_t) * bsize);

	while(running)
	{
		length = recvfrom(client_fd, buffer, bsize, 0, (struct sockaddr*)&from_addr, &from_len);
		if(length > 0)
		{
			printf("recv from:%s:%hu length:%d\n",inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port), length);

			bzero(&from_addr);
			memset(buffer, 0x00, bsize);	
			continue;		
		}

		if(!(lengh < 0 && errno == EAGAIN))
		{
			running = false;
			//close
			DisposeClient(client_fd);
			printf("client %s:%hu exit...\n", inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port));
		}
	}
}

int TCPServer::GetClientsIndex()
{
	for(int i = 0; i < MAX_CLIENT_COUNT; i ++)
		if(client_fds[i] == -1)	return i;

	return -1;
}
int TCPServer::GetClientsIndex(int client_fd)
{
	for(int i = 0; i < MAX_CLIENT_COUNT; i ++)
		if(client_fds[i] == client_fd)	return i;

	return -1;
}

bool TCPServer::DisposeClient(int client_fd)
{
	for(int i = 0; i < MAX_CLIENT_COUNT; i ++)
	{
		if(client_fds[i] == client_fd)
		{
			close(client_fds[i]);
			pthread_cancel(client_pts[i]);

			client_fds[i] = -1;
			client_pts[i] = NULL;
			return true;
		}
	}

	return false;
}

bool TCPServer::DisposeClients()
{
	for(int i = 0; i < MAX_CLIENT_COUNT; i ++)
	{
		if(client_fds[i] != -1)
		{
			close(client_fds[i]);
			pthread_cancel(client_pts[i]);

			client_fds[i] = -1;
			client_pts[i] = NULL;
		}
	}

	return true;
}


