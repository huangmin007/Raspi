
#pragma once

#include <stdint.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define MAX_CLIENT_COUNT	 254
#define CLIENT_BUFFER		 1023

enum Status
{
	Running,
	Stopped,
	Client_In,
	Client_Out,
	Client_Data,
	Client_Connected,
	Client_Disconnect,
	Error,
	IO_Error,
};

typedef void (*TCPServerStatusChangedCallback)(int status);
typedef void (*TCPServerClientStatusChangedCallback)(const Status status, const struct sockaddr_in *addr);
typedef void (*TCPServerClientDataCallback)(const uint8_t *data, uint32_t length, const struct sockaddr_in *addr);

class TCPServer
{
	public:
		TCPServer();
		~TCPServer();
		bool Bind(uint16_t port);
		bool Start();
		bool Stop();
		//const int *GetClients();
		int Send(const uint8_t *data, uint32_t length);
		int Send(const uint8_t *data, uint32_t length, int client_fd);

		void StatusChanged(void (*fptr)(int status))
		{
			_StatusChangedCallback = fptr;
		}

		void ClientStatusChanged(void (*fptr)(const Status status, const struct sockaddr_in *addr))
		{
			_ClientStatusChangedCallback = fptr;
		}

		void ClientData(void (*fptr)(const uint8_t *data, uint32_t length, const struct sockaddr_in *addr))
		{
			_ClientDataCallback = fptr;
		}


	protected:
		static void *ServerAcceptClient(void *args);
		static void *ClientReceiveData(void *args);
		int GetClientIndex();
		int GetClientIndex(int client_fd);
		bool DisposeClient(int client_fd);
		bool DisposeClients();

	private:

		int client_fds[MAX_CLIENT_COUNT];
		pthread_t client_pts[MAX_CLIENT_COUNT];
		
		int listen_fd;
		int bind_ret;
		bool running = false;
		pthread_t listen_pt;
		sockaddr_in servaddr;

		TCPServerStatusChangedCallback _StatusChangedCallback = nullptr;
		TCPServerClientStatusChangedCallback _ClientStatusChangedCallback = nullptr;
		TCPServerClientDataCallback _ClientDataCallback = nullptr;
};

struct client_pthread_args
{
	int client_fd;
	sockaddr_in *client_addr;
	TCPServer *server;
};

