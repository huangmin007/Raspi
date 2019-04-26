/*********************************************************
 *
 *	Template
 *	main.c
 *	
 *********************************************************/



#include <stdio.h>
#include <stdint.h>
#include <signal.h>
#include <wiringPi.h>


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
	struct sigaction sa =
	{
		.sa_handler = ctrl_c_handler,
	};
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
}

// function propotype
void setup();
void loop();


// variables


// program main
int main(int argc, char *argv[])
{
	setup_sigaction();
	
	setup();

	while(running)
	{
		loop();
	}

	return 0;
}


// wiring Pi Setup
void setup()
{
	wiringPiSetup();
}


// wiring Pi loop
void loop()
{
	// loop 
}
