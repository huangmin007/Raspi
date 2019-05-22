/*********************************************************
 *
 *	Template
 *	main.c
 *	
 *********************************************************/


#include <stdio.h>
#include <signal.h>
#include <stdlib.h>  
#include <linux/input.h>  
#include <fcntl.h>  
#include <sys/time.h>  
#include <sys/select.h>
#include <sys/types.h>  
#include <sys/stat.h>  
#include <unistd.h>


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
#define MOVE_STEP 10
static int px;
static int py;
static int mouse_fd;
int max_x = 1920;
int max_y = 1080;

int main(int argc, char *argv[])
{
    setup_sigaction();

    int fd = open("/dev/input/mouse0", O_RDONLY);
    if(fd < 0)
    {
	printf("Failed to open \"/dev/input/mouse0\" \n");
	exit(1);
    }

    unsigned char buf[3];
    while(running)
    {
	if(read(fd, buf, sizeof(buf)))
	{
	    printf("mouse type=0x%02x x=%d y=%d\n", buf[0], buf[1], buf[2]);
	}
    }

    return 0;
}

// program main
int main1(int argc, char *argv[])
{
	setup_sigaction();
	
	int fd, retval;
	char buf[6];
	fd_set readfds;
	struct timeval tv;

	//打开鼠标设备
	fd = open("/dev/input/mice", O_RDONLY);
	
	if(fd < 0)
	{
	    printf("Failed to open \"/dev/input/mice\".\n");
	    exit(1);
	}	    
	else
	{
	    printf("open \"/dev/input/mice\" successfuly.\n");
	}
	
	while(running)
	{
	    tv.tv_sec = 5;
	    tv.tv_usec = 0;

	    FD_ZERO(&readfds);
	    FD_SET(fd, &readfds);

	    retval = select(fd + 1, &readfds, NULL, NULL, &tv);

	    if(retval == 0)	printf("Time out!\n");
	    
	    if(FD_ISSET(fd, &readfds))
	    {
		if(read(fd, buf, 6) <= 0) continue;

		printf("Button type=%d, X=%d, Y=%d, Z=%d\n", (buf[0] & 0x07), buf[1], buf[2], buf[3]);

		if(0 < buf[1])
		{
		    px += (10 > buf[1]) ? MOVE_STEP : (0 - MOVE_STEP);
		}
		if(0 < buf[2])
		{
		    py += (10 > buf[2]) ? (0 - MOVE_STEP) : MOVE_STEP;
		}

		px = (1 > px) ? 0 : px;
		py = (1 > py) ? 0 : py;
		px = (max_x < px) ? max_x : px;
		py = (max_y < py) ? max_y : py;

		printf("x:%d y:%d\n", px, py);
	    }
	}

	close(fd);

	return 0;
}


// wiring Pi Setup
void setup()
{
//	wiringPiSetup();
}


// wiring Pi loop
void loop()
{
	// loop 
}



