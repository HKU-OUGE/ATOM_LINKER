/*---------------------------------------------------------------------------------------------

 *  Last Modify:2021/4/27
 *  Version:     1.0
 *  Description:serial i/o on ROS/Linux
 *--------------------------------------------------------------------------------------------*/
#include    <stdio.h>
#include 	<string.h>
#include    <stdlib.h>
#include    <sys/types.h>
#include    <sys/stat.h>
#include    <fcntl.h>
#include    <termios.h>
#include    <errno.h>
#include 	<pthread.h>
#include 	<unistd.h> 
#include    <time.h>
#include    "../ros_inc/macro.h"
#include    "../ros_inc/protocol.h"

#define B9600	 0000015	    //9600 b/s
#define B38400	 0000017	    //38400 b/s
#define B57600   0010001	    //57600 b/s
#define B115200  0010002	    //115200 b/s
#define B230400  0010003	    //230400 b/s
#define B460800  0010004	    //460800 b/s
#define B500000  0010005	    //500000 b/s
#define B576000  0010006	    //576000 b/s
#define B921600  0010007	    //921600 b/s
#define B1500000 0010012	    //1500000 b/s

signed char OpenCOMDevice(const  char *Port,const int iBaudrate)
{
    signed char nFd = -1;
    printf("Connecting %s\n",Port);
	nFd=open(Port, O_RDWR|O_NOCTTY|O_NDELAY );
	printf("nFd %d\n",nFd);
	if(nFd<0)
	{
		printf("Connect %s failed!\n",Port);
		return -1;
	}

	struct termios options = { 0 };   
	
	int setSpeed;

	switch(iBaudrate)
	{
		case 9600:	setSpeed=B9600;
				  	break;
		case 38400:	setSpeed=B38400;
					break;
		case 57600: setSpeed=B57600;
					break;
		case 115200:setSpeed=B115200;
					break;
		case 230400:setSpeed=B230400;
					break;
		case 460800:setSpeed=B460800;
					break;
		case 921600:setSpeed=B921600;
					break;
		case 1500000:setSpeed=B1500000;
					 break;
		default: setSpeed=B115200;
				break;
	}

	options.c_cflag = (setSpeed | CLOCAL | CREAD | CS8); 

	cfsetispeed( &options , setSpeed );
	cfsetospeed( &options , setSpeed );

	options.c_cc[ VTIME ] = 1;	
	options.c_cc[ VMIN ] = 0;	
	tcflush(nFd , TCIOFLUSH );	

	fcntl(nFd,F_SETFL,0);//zuse

	if ( (tcsetattr(nFd , TCSAFLUSH , &options )) == -1 )
	{ 
		nFd = -1;
	}

	
	printf("%s connected!\n",Port);

	//if(pthread_create(&receiveDataThread, NULL, &ReceiveCOMData, NULL)!=0)
	//	printf("Create receive com data thread failed!\n");
	//else
	//	printf("Create receive com data thread successfully!\n");

	return nFd;
}

signed char OpenCom(const char *Port, const int iBaudrate)
{
    unsigned char r;
    signed char ret=OpenCOMDevice(Port,iBaudrate);
    if(ret >= 0)
    {
        //	r = 0x00;
    }
    else
        ret = 0xFF;
    return ret;
}

void CloseCOMDevice(signed char  nFd)
{
    close(nFd);
}

void CloseSerialPort(signed char  nFd)
{
    CloseCOMDevice(nFd);
}

signed char SendUARTMessageLength(signed char  nFd, const char chrSendBuffer[],const unsigned short usLen)
{
	write(nFd,chrSendBuffer,usLen);
	return 0;
}

signed char SetBaudrate(signed char  nFd, const int iBaudrate)
{
	struct termios options = { 0 };   
	options.c_cflag = (iBaudrate | CLOCAL | CREAD | CS8); 

	cfsetispeed( &options , iBaudrate );
	cfsetospeed( &options , iBaudrate );

	if ( (tcsetattr( nFd , TCSANOW , &options )) == -1 )
		return -1;

	return 0;
}

