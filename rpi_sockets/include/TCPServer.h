
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


typedef void (*TCPServerStatusChangedCallback)(int status);
typedef void (*TCPServerClientStatusChangedCallback)(int status);
typedef void (*TCPServerClientDataCallback)(uint8_t *data, uint32_t length);

class TCPServer
{
	public:
		TCPServer();
		~TCPServer();
		bool Bind(uint16_t port);
		bool Start();
		bool Stop();
		int Send(int client_fd);
		const int *GetClients();
		int Boardcast(uint8_t *data, uint32_t length);

		void StatusChangedCallback(void (*fptr)(int status))
		{
			_StatusChangedCallback = fptr;
		}

		void ClientStatusChangedCallback(void (*fptr)(int status))
		{
			_ClientStatusChangedCallback = fptr;
		}

		void ClientDataCallback(void (*fptr)(uint8_t *data, uint32_t length))
		{
			_ClientDataCallback = fptr;
		}


	protected:
		static void *ServerAcceptClient(void *args);
		static void *ClientReceiveData(void *args);
		int GetClientsIndex();
		int GetClientsIndex(int client_fd);
		bool DisposeClient(int client_fd);
		bool DisposeClients();

	private:

		int client_fds[MAX_CONNECT_COUNT];
		pthread_t client_pts[MAX_CONNECT_COUNT];
		
		int listen_fd;
		int bind_ret;
		pthread_t listen_pt;
		sockaddr_in servaddr;

		TCPServerStatusChangedCallback _StatusChangedCallback = nullptr;
		TCPServerClientStatusChangedCallback _ClientStatusChangedCallback = nullptr;
		TCPServerClientDataCallback _ClientDataCallback = nullptr;


}
