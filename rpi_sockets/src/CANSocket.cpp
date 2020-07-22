
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
#include <stdexcept>
//#include <linux/can.h>
//#include <linux/can/raw.h>


CANSocket::CANSocket()
{
	//sockfd = socket(AF_INET, SOCK_STREAM, 0);
	//if(sockfd == -1)
	//{
	//	perror("scoket error...1");
	//}

	//printf("sockfd %d\n", sockfd);
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

int CANSocket::Connect(const char *can_name, uint32_t protocol)
{
	printf("sockfd: %d\n", sockfd);
	if(sockfd == -1)
	{
		if(protocol == CAN_RAW)
			sockfd = socket(PF_CAN, SOCK_RAW, CAN_RAW); //原始套接字协议 raw socket protocol
		else if(protocol == CAN_BCM)
			sockfd = socket(PF_CAN, SOCK_DGRAM, CAN_BCM);//广播管理协议 broadcast manager
		else
			throw std::invalid_argument("protocol error, only support CAN_RAW and CAN_BCM, in <linux/can.h>");

		if(sockfd == -1)
		{
			perror("can socket init bad ... \n");
			return -1;
		}
	}
	if(connected)	return 1;

	//struct can_filter filter[2];
	//filter[0].can_id = 0x123;
	//filter[0].can_mask = CAN_SFF_MASK;
	//filter[1].can_id = 0x200;
	//filter[1].can_mask = 0x700;
	//setsockopt(sockfd, SOL_CAN_RAW, CAN_RAW_FILTER, &filter, sizeof(filter));

	//strcpy(ifr.ifr_name, "can0");
	strcpy(ifr.ifr_name, can_name);
	//ioctl(sockfd, SIOCGIFINDEX, &ifr);	//指定接口索引需要调用ioctl(), 比如对于没有错误检查 CAN_RAW 套接字

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
		//如果套接字跟所有的CAN接口都绑定了（addr.can_index = 0），推荐使用recvfrom(2)获取数据源接口的信息
		//len = recvfrom(client->sockfd, &frame, sizeof(struce can_frame), 0, 
		//				(struct sockaddr*)&(client->addr), &sizeof(client->addr));
		
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

