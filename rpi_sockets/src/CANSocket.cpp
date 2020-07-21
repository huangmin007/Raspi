
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include "CANSocket.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <errno.h>
//#include <linux/can.h>
//#include <linux/can/raw.h>


CANSocket::CANSocket()
{
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd == -1)
	{
		perror("scoket error...1");
	}

	printf("sockfd %d\n", sockfd);
}

CANSocket::~CANSocket()
{
	Close();
	printf("~CANSocket..\n");
}

bool CANSocket::Connected()
{
	return connected;
}

int CANSocket::Connect(char *can_addr, uint16_t port)
{
	printf("sockfd: %d\n", sockfd);
	if(sockfd == -1)
	{
#if COMPILE_USE_SOCK_RAW
		sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW);	//原始套接字协议 raw socket protocol
#else
		sockfd = socket(PF_CAN, SOCK_DGRAM, CAN_BCM);	//广播管理协议BCM broadcast manager
#endif
		if(sockfd == -1)
		{
			perror("can socket init bad ... \n");
			return -1;
		}
	}
	if(connected)	return 1;

	strcpy(ifr.ifr_name, "can0");
	//ioctl(sockfd, SIOCGIFINDEX, &ifr);	//指定 can0 设备 接收

	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	int cr = bind(sockfd, (struct sockaddr*)&addr, sizeof(addr));
	if(cr < 0)
	{
		connected = false;
		if(_StatusChangedCallback != nullptr)	_StatusChangedCallback(CANSocketStatus::CONNECT_FAILED);

		return cr;
	}
	
	connected = true;

	//receive data pthread
	int ret = pthread_create(&tid, NULL, ReceiveDataThread, (void *)this);
	if(ret != 0)
	{
		printf("pthread error %s\n", strerror(ret));
		return 0;
	}

	return 1;
}

void *CANSocket::ReceiveDataThread(void *args)
{
	CANSocket *client = (CANSocket *)args;
	
	bool c_receive = true;
	int len, bsize = 1024;
	//client->buffer = (uint8_t*)malloc(sizeof(uint8_t) * bsize);
	
	struct can_frame frame;
		
	while(c_receive)
	{
		if(!client->connected)continue;
		
		len = read(client->sockfd, &frame, sizeof(struct can_frame));
		
		if(len > 0)
		{
			printf("read length: %d\n", len);
			printf("0x%03X [%d]", frame.can_id, frame.can_dlc);
			//if(client->_ReceiveDataCallback != nullptr)
			//	client->_ReceiveDataCallback(client->buffer, len);
			continue;
		}
		
		if(!(len < 0 && errno == EAGAIN))
		{
			client->Close();
			if(client->_StatusChangedCallback != nullptr)
				client->_StatusChangedCallback(CANSocketStatus::CONNECT_CLOSE);

			c_receive = false;
		}
	}

	//return (void*)1;
}

int CANSocket::Send(const uint8_t *data, uint32_t len)
{
	if(sockfd == -1 || !connected) return -1;
	
	struct can_frame frame;
	frame.can_id = 0x555;
	frame.can_dlc = 5;
	frame.data[0] = 'H';
	frame.data[1] = 'e';
	frame.data[2] = 'l';
	frame.data[3] = 'l';
	frame.data[4] = 'o';

	return write(sockfd, &frame, sizeof(struct can_frame));// != sizeof(struct can_frame);	
	//return send(sockfd, data, len, MSG_NOSIGNAL);
}

int CANSocket::Close()
{
	if(sockfd == -1 || !connected)	return 1;

	free(buffer);
	buffer = NULL;
	connected = false;

	int c = close(sockfd);
	sockfd = -1;

	pthread_cancel(tid);
	return c;
}

