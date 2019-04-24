/************************************************
 *
 *	MCP23S17  raspi   spiPort    devId
 *	#CS		 CE0/CE1    0/1	    A0,A1,A2 addr value
 *	SCK		 SCLK
 *	SI		 MOSI
 *	SO	 	 MISO
 *
 * *********************************************/


#include <stdio.h>
#include <signal.h>
#include <wiringPi.h>
#include <mcp23s17.h>

#define PIN_BASE 100
#define NUM_PINS 16


int running = 1;

//函数原型
void setup();
void loop();
void clear();
void setup_sigaction();
void ctrl_c_handler(int signum);


int main(int argc, char *argv[])
{
	setup_sigaction();

	setup();

	while(running)
	{
		loop();
	}

	clear();

	return 0;
}

void setup()
{
	printf("setup ...\n");
	wiringPiSetup();
	
	//mcp23s17Setup(int pinBase, int spiPort, int devId);
	if(mcp23s17Setup(PIN_BASE, 0, 0) < 0)
	{
		printf("mcp23s17 setup failed ... \n");
		return;
	}
	
	for(int i = 0; i < NUM_PINS; i ++)
	{
		pinMode(PIN_BASE + i, OUTPUT);
	}
}

void loop()
{
	for(int i = 0; i < NUM_PINS; i ++)
	{
		digitalWrite(PIN_BASE + i, 1);
		//printf("pin:%d state:%d \n", i, 1);
		delay(500);
	}
	delay(1000);

	for(int i = 0; i < NUM_PINS; i ++)
	{
		digitalWrite(PIN_BASE + i, 0);
		//printf("pin:%d state:%d \n", i, 0);
		delay(500);
	}
	delay(1000);
}

void clear()
{
	for(int i = 0; i < NUM_PINS; i ++)
		digitalWrite(PIN_BASE + i, 0);

	printf("clear ... \n");
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

void ctrl_c_handler(int signum)
{
	running = 0;
}
