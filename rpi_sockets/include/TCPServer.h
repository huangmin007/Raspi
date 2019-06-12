
#pragma once

#include <stdint.h>
#include <pthread.h>

#define MAX_CONNECT_COUNT	 254
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
}


typedef void (*TCPServerStatusCallback)(int status);
typedef void (*TCPServerClientStatusCallback)(int status);
typedef void (*TCPServerClientDataCallback)(uint8_t *data, uint32_t length);

class TCPServer
{
	public:
		TCPServer();
		~TCPServer();
		bool Bind(uint16_t port);
		bool Start();
		bool Stop();
		int Boardcast(uint8_t *data, uint32_t length);


	protected:
		static void *ServerAcceptClient(void *args);
		static void *ClientReceiveData(void *args);

	private:
		int client_fds[MAX_CONNECT_COUNT];
		pthread_t client_pts[MAX_CONNECT_COUNT];

		int listen_fd;
		pthread_t listen_pt;
}
