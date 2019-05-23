/*********************************************************
 *
 *	APA102 led 
 *	use spi
 *	CI		SCLK
 *	DI		MOSI
 *
 *
 *********************************************************/



#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <time.h>
#include <string.h>

#include <signal.h>
#include <wiringPi.h>
#include <wiringPiSPI.h>


#define BSWAP_8(x)	((x) & 0xFF)
#define BSWAP_16(x)	((BSWAP_8(x) << 8) | BSWAP_8((x) >> 8))
#define BSWAP_32(x)	((BSWAP_16(x) << 16) | BSWAP_16((x) >> 16))
#define BSWAP_64(x)	((BSWAP_32(x) << 32) | BSWAP_32((x) >> 32))


int testWS2812();
int runAPA102();
void setup_sigaction();
void ctrl_c_handler(int signum);

int running = 1;

int main(int argc, char *argv[])
{
	setup_sigaction();

	wiringPiSetup();

	//testWS2812();
	runAPA102();

	return 0;
}

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

void setAPA102LED(uint8_t r, uint8_t g, uint8_t b, uint8_t brightness)
{
	uint8_t color[4];
	color[0] = 0B11100000 | (0B00011111 & brightness);
	color[1] = b;
	color[2] = g;
	color[3] = r;

	wiringPiSPIDataRW(0, color, 4);
}

int runAPA102()
{
	if(wiringPiSPISetup(0, 24000000) < 0)
	{
		printf("set spi failed...\n");
		return -1;
	}

	uint32_t i = 0;
						//birghtness B	   G     R
	uint8_t color[32] = {
						0B11111111, 0x00, 0x00, 0xFF,
						0B11101111, 0x00, 0x00, 0xFF,
						0B11100111, 0x00, 0x00, 0xFF,
						0B11100011, 0x00, 0x00, 0xFF,
						0B11100001, 0x00, 0x00, 0xFF,
						0B11011111, 0x00, 0x00, 0xFF,
						0B10011111, 0x00, 0x00, 0xFF,
						0B10000011, 0x00, 0x00, 0xFF,
	};

	unsigned char startFrame[4] = {0x00, 0x00, 0x00, 0x00};
	unsigned char endFrame[4] = {0xFF, 0xFF, 0xFF, 0xFF};
	
	uint8_t brightness = 0;
	unsigned char green[4] = {0xE0, 0x00, 0xFF, 0x00};

	//open
	while(running)
	{
	    	//start frame
		wiringPiSPIDataRW(0, startFrame, 4);

		
		brightness ++;
		if(brightness >= 0x1F)	brightness = 1;
		
		color[0] = 0B11100000 | (0B00011111 & brightness);

		//printf("%02X\n", color[0]);

		uint8_t data[sizeof(color)];
		memcpy(data, color, sizeof(color));
		wiringPiSPIDataRW(0, data, sizeof(data));
		
		
		//green[0] = 0xE0 | ( 0x1F & brightness);
		//for(int i = 0; i < 8; i ++)
		//{
		    //uint32_t *c = ((0xE0 | (0x1F & brightness)) < 32) | 0x0000FF00;
		    //green[0] = 0xE0 | (0x1F & brightness);
		//    wiringPiSPIDataRW(0, green, 4);
		//}

		
		//end frame
		wiringPiSPIDataRW(0, endFrame, 4);
		//for(int i = 0; i < 8; i += 16)
		//    wiringPiSPIDataRW(0, 0x00, 1);
		
		//printf("%02X\n",endFrame[1]);
		delay(200);
	}

	//close all
	wiringPiSPIDataRW(0, startFrame, 4);
	unsigned char clear[40];
	for(i = 0; i < 10; i ++)
	{
		clear[i * 4 + 0] = 0xFF;
		clear[i * 4 + 1] = 0x00;
		clear[i * 4 + 2] = 0x00;
		clear[i * 4 + 3] = 0x00;
	}
	wiringPiSPIDataRW(0, clear, 40);
	wiringPiSPIDataRW(0, endFrame, 4);
	//for(int i = 0; i < 10; i += 16)
	//    wiringPiSPIDataRW(0, 0x00, 1);

	return 0;
}

int testWS2812()
{
	if(wiringPiSPISetup(0, 500000) < 0)
	{
		printf("set spi failed...\n");
		return -1;
	}

	uint8_t color[9] = 
	{
		0xFF, 0x00, 0x00,
		0x00, 0xFF, 0x00,
		0x00, 0x00, 0xFF,
	};

	while(running)
	{
		wiringPiSPIDataRW(0, (unsigned char *)0x7C, 1);

		wiringPiSPIDataRW(0, color, 3);
		delay(500);
	}

	return 0;
}
