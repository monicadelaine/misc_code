#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>
      volatile unsigned char PRESSED1=0;
      volatile unsigned char PRESSED2=0;
      volatile int CLOSE[8][5];
      int idx1=0;
      int idx2=0;
	extern pthread_mutex_t mIdx1;
	extern pthread_mutex_t mIdx2;
      
#include "resources.h"
       void *serial_BUTTON(void * parm)
      {
        int fd,c, res;
        struct termios oldtio,newtio;
        unsigned char buf[255];
        char cmd[2];
	cmd[0]=254;
	cmd[1]=192;
//	cmd[0]=0;
//	cmd[1]=89;
	int i;int val;


        fd = open(MODEMDEVICE_BUTTON, O_RDWR | O_NOCTTY ); 
        if (fd <0) {perror(MODEMDEVICE_BUTTON); printf("didn't open usb port");fflush(stdout);return; }
        
         tcgetattr(fd,&oldtio); /* save current port settings */
        
        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = BAUDRATE_BUTTON | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
        
        /* set input mode (non-canonical, no echo,...) */
        newtio.c_lflag = 0;
         
        newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
        newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */
        
        tcflush(fd, TCIFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);
	
        
        
        while (TRUE) {       /* loop for input */
	  write(fd,cmd,2);
	  usleep(300000);
          //sleep(1);
          res = read(fd,buf,255);   /* returns after lull in input */
          buf[res]=0;        
	  if (buf[0] > 100) PRESSED1=TRUE;
	  else PRESSED1=FALSE;
	  if (buf[3] > 100) PRESSED2=TRUE;
	  else PRESSED2=FALSE;
        }
        tcsetattr(fd,TCSANOW,&oldtio);
      }      
      void *serial_DIST2(void * parm)
      {
        int fd,c, res;
        struct termios oldtio,newtio;
        unsigned char buf[255];
        char range[5];
	int sensors[3][3];
	double avg;int idx=0;
int cnt=0;

	range[0]=0x55;
	range[1]=0xe0;
	range[2]=0x00;
	range[3]=0x01;
	range[4]=80;

        char readrange[4];

	readrange[0]=0x55;
	readrange[1]=0xe1;
	readrange[2]=0x02;
	readrange[3]=0x02;
	int i;int val;int j;


        fd = open(MODEMDEVICE_DIST2, O_RDWR | O_NOCTTY ); 
        if (fd <0) {perror(MODEMDEVICE_DIST2); return;}
        
         tcgetattr(fd,&oldtio); /* save current port settings */
        
        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = BAUDRATE_DIST | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
        
        /* set input mode (non-canonical, no echo,...) */
        newtio.c_lflag = 0;
         
        newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
        newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */
        
        tcflush(fd, TCIFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);

//	range[4]=0xA0;
//	  write(fd,range,5);
//	range[4]=0xAA;
//	  write(fd,range,5);
//	range[4]=0xA5;
//	  write(fd,range,5);
//	range[4]=0xe4;
//	  write(fd,range,5);
//	exit(0);	
        
        
        while (TRUE) {       /* loop for input */
	  for (j=0xe0;j<=0xe6;j+=2){
		range[1]=j;
	  	write(fd,range,5);usleep(33000);
         	res = read(fd,buf,255);   /* returns after lull in input */
 
		}
	cnt=4;
 	  for (j=0xe1;j<=0xe7;j+=2){
		readrange[1]=j;
	  write(fd,readrange,4);usleep(33000);
         res = read(fd,buf,255);   /* returns after lull in input */
          buf[res]=0;        

	printf("a%d:%d  ", cnt,buf[1]);fflush(stdout);
	CLOSE[cnt][idx2]=buf[1];
	cnt++;


	}

        printf("\n");
	pthread_mutex_lock(&mIdx2);
	idx2=(idx2+1)%5;
	pthread_mutex_unlock(&mIdx2);

        }
        tcsetattr(fd,TCSANOW,&oldtio);
      }

      void *serial_DIST1(void * parm)
      {
        int fd,c, res;
        struct termios oldtio,newtio;
        unsigned char buf[255];
        char range[5];
	int sensors[3][3];
	double avg;int idx=0;
int cnt1=0;

	range[0]=0x55;
	range[1]=0xe0;
	range[2]=0x00;
	range[3]=0x01;
	range[4]=80;

        char readrange[4];

	readrange[0]=0x55;
	readrange[1]=0xe1;
	readrange[2]=0x02;
	readrange[3]=0x02;
	int i;int val;int j;


        fd = open(MODEMDEVICE_DIST1, O_RDWR | O_NOCTTY ); 
        if (fd <0) {perror(MODEMDEVICE_DIST1); return;}
        
         tcgetattr(fd,&oldtio); /* save current port settings */
        
        bzero(&newtio, sizeof(newtio));
        newtio.c_cflag = BAUDRATE_DIST | CS8 | CLOCAL | CREAD;
        newtio.c_iflag = IGNPAR;
        newtio.c_oflag = 0;
        
        /* set input mode (non-canonical, no echo,...) */
        newtio.c_lflag = 0;
         
        newtio.c_cc[VTIME]    = 0;   /* inter-character timer unused */
        newtio.c_cc[VMIN]     = 0;   /* blocking read until 5 chars received */
        
        tcflush(fd, TCIFLUSH);
        tcsetattr(fd,TCSANOW,&newtio);

//	range[4]=0xA0;
//	  write(fd,range,5);
//	range[4]=0xAA;
//	  write(fd,range,5);
//	range[4]=0xA5;
//	  write(fd,range,5);
//	range[4]=0xe4;
//	  write(fd,range,5);
//	exit(0);	
        
        
        while (TRUE) {       /* loop for input */
	  for (j=0xe0;j<=0xe6;j+=2){
		range[1]=j;
	  	write(fd,range,5);usleep(33000);
         	res = read(fd,buf,255);   /* returns after lull in input */
 
		}
	cnt1=0;
 	  for (j=0xe1;j<=0xe7;j+=2){
		readrange[1]=j;
	  write(fd,readrange,4);usleep(33000);
         res = read(fd,buf,255);   /* returns after lull in input */
          buf[res]=0;        
	printf("b%d:%d  ", cnt1,buf[1]);fflush(stdout);
	CLOSE[cnt1][idx1]=buf[1];
//	printf("b (%d:%d) %d %d %d %d ", idx1, buf[1],CLOSE[6][0],CLOSE[6][1],CLOSE[6][2],CLOSE[6][3]);fflush(stdout);
	cnt1++;

	}

        printf("\n");
	pthread_mutex_lock(&mIdx1);
	idx1=(idx1+1)%5;
	pthread_mutex_unlock(&mIdx1);
        }
        tcsetattr(fd,TCSANOW,&oldtio);
      }
