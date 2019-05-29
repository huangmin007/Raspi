
#pragma once

#include <pthread.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/io.h>


typedef void (*UDPClientStatusErrorCallback)(int32_t status);
typedef void (*UDPClientReceiveDataCallback)(uint8_t buffer, uint32_t length);


class UDPClient
{
	public:
		UDPClient();
		~UDPClient();

		//int bind(char *localAddr = "0.0.0.0", uint16_t localPort = 0);

		int connect(char *remote_addr, uint16_t remote_port);
		int send(uint8_t *buffer, uint32_t length);
		int send(uint8_t *buffer, uint32_t offset, uint32_t length);
		int send(uint8_t *buffer, uint32_t offset, uint32_t length, char *remote_addr, uint16_t remote_port);
		int close();

		void ReceiveCallback(void (*fptr)(uint8_t *buffer, uint32_t length))
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
		struct sockaddr_in servaddr;

		char *remoteAddr;
		uint16_t remotePort;

		char *localAddr;
		uint16_t localPort;

		bool bound;

		pthread_t pid;
		uint8_t *buffer;

		UDPClientStatusErrorCallback _StatusErrorCallback = nullptr;
		UDPClientReceiveDataCallback _ReceiveDataCallback = nullptr;
}
