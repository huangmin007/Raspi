
#pragma once

#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>


//typedef unsigned short uint16_t;
//typedef unsigned int   uint32_t;

typedef void (*TCPClientStatusErrorCallback)(int status);
typedef void (*TCPClientConnectFailedCallback)(int code);
typedef void (*TCPClientConnectCloseCallback)();
typedef void (*TCPClientReceiveDataCallback)(char *buffer, uint32_t length);

class TCPClient
{
	public:
		TCPClient();
		~TCPClient();	
		TCPClient(char *addr, uint16_t port);
		int Connect(char *addr, uint16_t port);
		bool Connected();
		int Send(char *data, uint32_t len);
		int Close();
		void ReceiveCallback(void (*fptr)(char *buffer, uint32_t length))
		{
			_receiveDataCallback = fptr;
		}

		void ConnectFailedCallback(void (*fptr)(int code))
		{
			_connectFailedCallback = fptr;
		}

		void ConnectCloseCallback(void (*fptr)())
		{
			_connectCloseCallback = fptr;
		}

	protected:
		static void *ReceiveData(void *args);

	private:
		int sockfd = -1;
		struct sockaddr_in servaddr;
		
		char *address;
		uint16_t port;

		pthread_t tid;
		char *buffer;

		bool connected = false;
		
		
		TCPClientReceiveDataCallback _receiveDataCallback = nullptr;
		TCPClientConnectCloseCallback _connectCloseCallback = nullptr;
		TCPClientConnectFailedCallback _connectFailedCallback = nullptr;
};


