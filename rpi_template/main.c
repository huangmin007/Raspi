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
void signal_handler(int signum)
{
	running = 0;
	printf("signal arg:%d\n", signum);

	switch(signum)
	{
		case SIGINT:
			break;

		case SIGTERM:
			break;

		case SIGKILL:
			break;

		case SIGRTMIN + 1:
			break;

		default:
			printf("not handler signal:%d", signum);
	}
}
void setup_sigaction()
{
	struct sigaction sa =
	{
		.sa_handler = signal_handler,
	};
	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);
	sigaction(SIGKILL, &sa, NULL);
	//sigaction(SIGRTMIN + 1, &sa, NULL); //test ok
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



