/*********************************************************
 *
 *	Template
 *	main.c
 *	
 *********************************************************/
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
//#include <sys/types.h>
//#include <sys/socket.h>
//#include <netinet/in.h>
#include <arpa/inet.h>
//#include <wiringPi.h>
#include "TCPClient.h"
#include "UDPClient.h"
#include "TCPServer.h"
#include <unistd.h>
using namespace std;

// define macro
#define		C_DEBUG		1		//Code Debug


// sigaction handler
int running = 1;	//rinning
void ctrl_c_handler(int signum)
{
	running = 0;
}
void setup_sigaction()
{
	struct sigaction sa;
	sa.sa_handler = ctrl_c_handler;
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}

// function propotype
void setup();
void loop();


// variables
TCPClient client;
//UDPClient client;
//TCPServer server;

void ReceiveDataHandler(const uint8_t *buffer, uint32_t length)
{
	char data[length] = {};
	memcpy(data, buffer, length);
	printf("data: %s\n", data);
}
void StatusChangedHandler(int status)
{
	printf("Client Status:%d\n", status);
}


void ClientStatusChangedHandler(const int status, const sockaddr_in *addr)
{
	printf("client::%s:%hu status:%d\n", inet_ntoa(addr->sin_addr), ntohs(addr->sin_port), status);
}
void ClientReceiveDataHandler(const uint8_t *data, uint32_t length, const sockaddr_in *addr)
{
	printf("client data length:%d %s:%hu\n", length, inet_ntoa(addr->sin_addr), ntohs(addr->sin_port));
	
	char c[length + 1] = {};
	memcpy(c, data, length);
	c[length + 1] = '\0';
	printf("%s\n", c);
}


// program main
int main(int argc, char *argv[])
{
	printf("start ... \n");
	setup_sigaction();

	//char *addr = "192.168.51.253";
	char *addr = "169.254.119.138";
	uint16_t port = 3000;

	client.StatusChanged(StatusChangedHandler);
	client.ReceiveData(ReceiveDataHandler);
	client.Connect(addr, port);
	
	//server.Bind(5000);
	//server.ClientReceiveData(ClientReceiveDataHandler);
	//server.ClientStatusChanged(ClientStatusChangedHandler);
	//server.Start();
	
	char hw[16] = "Hello world ...";

	while(running)
	{
		//if(!client.Connected())
	//	{
	//		sleep(3);
	//		client.Connect(addr, port);
	//		continue;
	//	}

	//	loop();
		client.Send((uint8_t*)addr, 14);
		//client.Send((uint8_t*)addr, 14, addr, 3001);
		sleep(2);
		

		client.Send((uint8_t *)hw, 16);
		sleep(2);
	}

	//server.Stop();
	client.Close();

	printf("exit...\n");
	return 0;
}


// wiring Pi Setup
void setup()
{
	//wiringPiSetup();
}


// wiring Pi loop
void loop()
{
	// loop 
}



