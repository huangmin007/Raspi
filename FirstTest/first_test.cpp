//gcc -Wall -o first_test first_test.cpp -lwiringPi

#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <sys/time.h>

#include <wiringPi.h>
#include <wiringSerial.h>

void serial();
void testms();

int outPin = 4;

int main()
{
	fprintf(stdout, "hello Raspberry Pi.");
	if(wiringPiSetupGpio() == -1)
	{
		fprintf(stdout, "Unable to start wiringPi:%s\n", strerror(errno));
		exit(-1);
	}

	//test();
	//serial();
	testms();
	
	return 0;
}
/*
struct timeval
{
	long tv_sec;
	long tv_usec;
};
*/
/*
struct tm
{
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	int tm_isdst;
};
*/

void testms()
{
	struct timeval tv;
	char buf[64];

	while(true)
	{
		if(millis() % 30 == 0)
		{
			gettimeofday(&tv, NULL);
			strftime(buf, sizeof(buf) - 1, "%Y-%m-%d %H:%M:%S", localtime(&tv.tv_sec)); 
			//fprintf(stdout, "millis:%d\n", millis());
			fprintf(stdout, "millis:%d  micros:%d  time:%s\n", millis(), micros(), buf);
		}
	}
}

void test()
{
	pinMode(outPin, OUTPUT);
	while(true)
	{
		//digitalWrite(outPin, !digitalRead(outPin));
		digitalWrite(outPin, HIGH);
		delay(1000);
		digitalWrite(outPin, LOW);
		delay(1000);
	}
}

void serial()
{
	int fd = serialOpen("/dev/ttyACM0", 9600);
	if(fd < 0)
	{
		fprintf(stderr, "Open serial device: %s\n", strerror(errno));
		return;
	}
	for(;;)
	{
		putchar(serialGetchar(fd));
		fflush(stdout);
	}
	/*
	//while(true)
	//{
		while(serialDataAvail(fd) > 0)
		{
			printf(" -> %3d", serialGetchar(fd));
			fflush(stdout);
		}
	//}
	*/
}
