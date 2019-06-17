

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
//#include <strings.h>
#include <unistd.h>
#include <errno.h>

#include "TCPServer.h"


TCPServer::TCPServer()
{
	memset(client_fds, -1, MAX_CLIENT_COUNT);
	memset(client_pts, 0, MAX_CLIENT_COUNT);

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
	Stop();
}

bool TCPServer::Bind(uint16_t port)
{
	if(bind_ret > 0) return true;

	if(listen_fd < 0)
	{
		printf("Socket init failed...\n");
		return false;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(port);

	int reuse = 1;
	setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
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
	if(sock_listening)	return true;
	if(listen_fd < 0 || bind_ret < 0)
	{
		printf("Start Failed ... \n");
		return false;
	}

	if(listen(listen_fd, 10) < 0)
	{
		printf("Start Failed, Listen error ... \n");
		return false;
	}

	int ret = pthread_create(&listen_pt, NULL, ServerAcceptClientThread, (void *)this);
	if(ret != 0)
	{
		printf("Start Failed, Create accept pthread error ... \n");
		return false;
	}
	
	sock_listening = true;
	printf("start...\n");
	return true;
}

bool TCPServer::Stop()
{
	if(sock_listening)
	{
		printf("stopping...\n");
		sock_listening = false;

		DisposeClients();

		close(listen_pt);
		pthread_cancel(listen_pt);

		bind_ret = -1;
		listen_fd = -1;

		return true;
	}

	return false;
}

int TCPServer::Send(const uint8_t *data, uint32_t length)
{
	if(!sock_listening) return -1;

	int count = 0;
	for(int i = 0; i < MAX_CLIENT_COUNT; i ++)
	{
		if(client_fds[i] != -1)
		{
			count ++;
			send(client_fds[i], data, length, MSG_NOSIGNAL);
		}
	}
	return count;
}
int TCPServer::Send(const uint8_t *data, uint32_t length, int client_fd)
{
	if(!sock_listening)	return -1;
	for(int i = 0; i < MAX_CLIENT_COUNT; i ++)
	{
		if(client_fds[i] == client_fd)
			return send(client_fds[i], data, length, MSG_NOSIGNAL);
	}

	return -1;
}

void *TCPServer::ServerAcceptClientThread(void *args)
{
	TCPServer *server = (TCPServer*)args;

	int client_fd;
	sockaddr_in client_addr;
	socklen_t csize = sizeof(sockaddr_in);

	while(server->sock_listening)
	{
		client_fd = accept(server->listen_fd, (struct sockaddr*)&client_addr, &csize);
		if(client_fd < 0)
		{
			perror("Accept error ...\n");
			continue;
		}
		printf("server got connection from:%s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

		//检查客户端列表数量是否超出
		int index = server->GetClientIndex();
		if(index < 0 || index > MAX_CLIENT_COUNT)
		{
			close(client_fd);
			perror("connect clients out of range count ..\n");
			continue;
		}

		struct client_pthread_args *cps;
	    cps->client_fd = client_fd;
		cps->client_addr = &client_addr;
		cps->server = server;
		
		//创建客户端数据接收线程
		pthread_attr_t attr;
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

		pthread_t client_pt;
		//if(pthread_create(&client_pt, NULL, ClientReceiveDataThread, (void *)cps) < 0)
		if(pthread_create(&client_pt, &attr, ClientReceiveDataThread, (void *)cps) < 0)
		{
			perror("Accept Error, client create pthread error...\n");
			continue;
		}
		
		server->client_fds[index] = client_fd;
		server->client_pts[index] = client_pt;

		if(server->_ClientStatusChangedCallback)
			server->_ClientStatusChangedCallback(TCPServer_Status::Client_In, &client_addr);
	}
}

void *TCPServer::ClientReceiveDataThread(void *args)
{
	//int client_fd = *(int*)args;
	struct client_pthread_args *cps = (struct client_pthread_args*)args;
	int client_fd = cps->client_fd; 

	struct sockaddr_in client_addr;
	socklen_t csize = sizeof(sockaddr_in);
	client_addr = *(cps->client_addr);

	bool c_sock_listening = true;
	int length, bsize = CLIENT_BUFFER;
	uint8_t *buffer = (uint8_t*)malloc(sizeof(uint8_t) * bsize);

	while(c_sock_listening)
	{
		length = recvfrom(client_fd, buffer, bsize, 0, 0, 0);
		//length = recvfrom(client_fd, buffer, bsize, 0, (struct sockaddr*)&client_addr, &csize);
		if(length > 0)
		{
			printf("recv from:%s:%hu length:%d\n",inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), length);

			if(cps->server->_ClientReceiveDataCallback)
				cps->server->_ClientReceiveDataCallback(buffer, length, &client_addr);
			
			memset(buffer, 0x00, bsize);
			continue;		
		}

		if(!(length < 0 && errno == EAGAIN))
		{
			printf("client %s:%hu exit...\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
			free(buffer);
			c_sock_listening = false;				

			if(cps->server->_ClientStatusChangedCallback)
				cps->server->_ClientStatusChangedCallback(TCPServer_Status::Client_Out, &client_addr);

			cps->server->DisposeClient(client_fd);
			bzero(&client_addr, sizeof(client_addr));
		}
	}
}

int TCPServer::GetClientIndex()
{
	for(int i = 0; i < MAX_CLIENT_COUNT; i ++)
		if(client_fds[i] == -1)	return i;

	return -1;
}
int TCPServer::GetClientIndex(int client_fd)
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
			client_pts[i] = 0;
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
			client_pts[i] = 0;
		}
		else
		{
			if(client_pts[i] != 0)
				pthread_cancel(client_pts[i]);
			client_pts[i] = 0;
		}
	}

	return true;
}


