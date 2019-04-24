/******************************************
 *  
 * Test 74HC595
 * scons
 * sudo ./test
 *
 ******************************************/ 

#include <stdio.h>
#include <signal.h>
#include <wiringPi.h>
#include <sr595.h>

#define PIN_BASE 100
#define NUM_PINS 8

int running = 1;
int d_index = 0;
unsigned int data = 0x00;

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

void run1()
{
	for(int i = 0; i < NUM_PINS; i ++)
	{
		digitalWrite(PIN_BASE + i, ( data >> i) & 0x01);
	}

	data = data | (1 << d_index);
	d_index ++;
	if(d_index > NUM_PINS)
	{
		d_index = 0;
		data = 0x00;
	}

	//printf("running...%d %d\n", d_index, data);
}

void clear()
{
	d_index = 0;
	data = 0x00;

	for(int i = 0; i < NUM_PINS; i ++)
	{
		digitalWrite(PIN_BASE + i, 0);
	}

	printf("clear...\n");
}

int main(int argc, char *argv[])
{
	setup_sigaction();
	printf("start ...\n");	

	wiringPiSetup();
	//sr595Setup(pinBase, numPins, dataPin, clockPin, latchPin)
	//Use wiringPi pins 0, 1, 2, for data, clock, latch
	sr595Setup(PIN_BASE, NUM_PINS, 0, 1, 2);
	
	while(running)
	{
		run1();
		delay(500);
	}

	clear();
	printf("exit...\n");
	
	return 0;
}



