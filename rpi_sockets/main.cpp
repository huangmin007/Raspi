/*********************************************************
 *
 *	Template
 *	main.c
 *	
 *********************************************************/

#include <string>
#include <stdio.h>
#include <stdint.h>
#include <signal.h>
//#include <wiringPi.h>
#include "TCPClient.h"

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


// program main
int main(int argc, char *argv[])
{
	printf("start ... \n");
	setup_sigaction();

	char *addr = "192.168.51.253";
	//char *addr = "169.254.119.138";
	uint16_t port = 3000;

	//client = TCPClient();
	client.Connect(addr, port);

	//setup();

	while(running)
	{
	//	loop();
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



