
#pragma once

//#include <pthread.h>
#include <stdint.h>
//#include <sys/types.h>
//#include <sys/socket.h>
#include <netinet/in.h>


//typedef uint8_t unsigned char;
//typedef uint32_t unsigned int;

typedef void (*UDPClientStatusErrorCallback)(int32_t status);
typedef void (*UDPClientReceiveDataCallback)(const uint8_t *buffer, uint32_t length);


class UDPClient
{
	public:
		UDPClient();
		~UDPClient();

		int Bind(char *local_addr, uint16_t local_port);

		int Connect(char *remote_addr, uint16_t remote_port);
		int Send(uint8_t *buffer, uint32_t length);
		int Send(uint8_t *buffer, uint32_t length, char *remote_addr, uint16_t remote_port);
		int Close();

		void ReceiveCallback(void (*fptr)(const uint8_t *buffer, uint32_t length))
		{
			_ReceiveDataCallback = fptr;
		}

		void StatusErrorCallback(void (*fptr)(int32_t status))
		{
			_StatusErrorCallback = fptr;
		}


	protected:
		static void *ReceiveData(void *args);

	private:
		int sockfd = -1;
		struct sockaddr_in dest_addr;		

		char *remoteAddr;
		uint16_t remotePort;

		char *localAddr;
		uint16_t localPort;

		bool bound;

		pthread_t pid;
		uint8_t *buffer;

		UDPClientStatusErrorCallback _StatusErrorCallback = nullptr;
		UDPClientReceiveDataCallback _ReceiveDataCallback = nullptr;
};
