
#pragma once

#include <pthread.h>
#include <stdint.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/can.h>
#include <linux/can/raw.h>

#define COMPILE_USE_SOCK_RAW 	1

enum CANSocketStatus
{
	CONNECT_SUCCESS,
	CONNECT_FAILED,
	CONNECT_CLOSE,
};

typedef void (*CANSocketStatusChangedCallback)(int status);
typedef void (*CANSocketReceiveDataCallback)(const uint8_t *data, uint32_t length);

class CANSocket
{
	public:
		CANSocket();
		~CANSocket();
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
		
		struct sockaddr_can addr;
		struct ifreq ifr;
		
		char *address;
		uint16_t port;

		pthread_t tid;
		uint8_t *buffer;

		bool connected = false;
		
		CANSocketReceiveDataCallback _ReceiveDataCallback = nullptr;
		CANSocketStatusChangedCallback _StatusChangedCallback = nullptr;
};


