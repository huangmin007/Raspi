
#pragma once

#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>

enum TCPClientStatus
{
	CONNECT_SUCCESS,
	CONNECT_FAILED,
	CONNECT_CLOSE,
};

typedef void (*TCPClientStatusChangedCallback)(int status);
typedef void (*TCPClientReceiveDataCallback)(const uint8_t *data, uint32_t length);

class TCPClient
{
	public:
		TCPClient();
		~TCPClient();
		bool Connected();	
		int Connect(char *addr, uint16_t port);

		int Send(const uint8_t *data, uint32_t len);

		int Close();

		void ReceiveData(void (*fptr)(const uint8_t *buffer, uint32_t length))
		{
			_ReceiveDataCallback = fptr;
		}

		void StatusChanged(void (*fptr)(int status))
		{
			_StatusChangedCallback = fptr;
		}

	protected:
		static void *ReceiveDataThread(void *args);

	private:
		int sockfd = -1;
		struct sockaddr_in servaddr;
		
		char *address;
		uint16_t port;

		pthread_t tid;
		uint8_t *buffer;

		bool connected = false;
		
		TCPClientReceiveDataCallback _ReceiveDataCallback = nullptr;
		TCPClientStatusChangedCallback _StatusChangedCallback = nullptr;
};


