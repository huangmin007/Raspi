
#pragma once

#include <stdint.h>
#include <netinet/in.h>

typedef void (*UDPClientStatusChangedCallback)(int32_t status);
typedef void (*UDPClientReceiveDataCallback)(const uint8_t *data, uint32_t length);

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

		void ReceiveData(void (*fptr)(const uint8_t *data, uint32_t length))
		{
			_ReceiveDataCallback = fptr;
		}

		void StatusChanged(void (*fptr)(int32_t status))
		{
			_StatusChangedCallback = fptr;
		}


	protected:
		static void *ReceiveDataThread(void *args);

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

		UDPClientReceiveDataCallback _ReceiveDataCallback = nullptr;
		UDPClientStatusChangedCallback _StatusChangedCallback = nullptr;
};
