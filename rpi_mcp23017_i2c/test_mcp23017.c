/****************************************
 *
 * MCP23017 i2c
 *
 *
 * ***************************************/


#include <stdio.h>
#include <signal.h>
#include <wiringPi.h>
#include <mcp23017.h>


#define PIN_BASE 100
#define NUM_PINS 16
int running = 1;

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


void setup()
{
	printf("setup ... \n");

	wiringPiSetup();
	mcp23017Setup(PIN_BASE, 0x20);
	
	for(int i = 0; i < NUM_PINS; i ++)
		pinMode(PIN_BASE + i, OUTPUT);
}

void loop()
{
	for(int i = 0; i < NUM_PINS; i ++)
	{
		digitalWrite(PIN_BASE + i, 1);
		//printf("pin:%d state:%d\n", i, 1);
		delay(500);
	}
	delay(1000);

	for(int i = 0; i < NUM_PINS; i ++)
	{
		digitalWrite(PIN_BASE + i, 0);
		//printf("pin:%d state:%d\n", i, 0);
		delay(500);
	}
	delay(1000);
}

void clear()
{
	for(int i = 0; i < NUM_PINS; i ++)
	{
		digitalWrite(PIN_BASE + i, 0);
	}

	printf("clear ... \n");
}

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
