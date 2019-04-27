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
//#include <wiringPi.h>
#include "TCPClient.h"
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
	//struct sigaction sa =
	//{
	//	.sa_handler = ctrl_c_handler,
	//};

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

void receiveDataHandler(char *buffer, uint32_t length)
{
	char data[length] = {};
	memcpy(data, buffer, length);
	printf("data: %s\n", data);
}
void connectFailedHandler(int code)
{
	printf("connect failed : %d\n", code);
}
void connectCloseHandler()
{
	printf("socket close..\n");
}

// program main
int main(int argc, char *argv[])
{
	printf("start ... \n");
	setup_sigaction();

	char *addr = "192.168.51.253";
	//char *addr = "169.254.119.138";
	uint16_t port = 3000;

	//client = TCPClient();
	client.ConnectFailedCallback(connectFailedHandler);
	client.ReceiveCallback(receiveDataHandler);
	client.ConnectCloseCallback(connectCloseHandler);
	client.Connect(addr, port);
	//setup();

	while(running)
	{
		if(!client.Connected())
		{
			sleep(3);
			client.Connect(addr, port);
			continue;
		}

	//	loop();
		client.Send(addr, 14);
		sleep(1);
	}

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



