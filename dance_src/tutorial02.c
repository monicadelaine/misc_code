// tutorial02.c
// A pedagogical video player that will stream through every video frame as fast as it can.
//
// Code based on FFplay, Copyright (c) 2003 Fabrice Bellard, 
// and a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1
// Use
//
// gcc -o tutorial02 tutorial02.c -lavformat -lavcodec -lz -lm `sdl-config --cflags --libs`
// to build (assuming libavformat and libavcodec are correctly installed, 
// and assuming you have sdl-config. Please refer to SDL docs for your installation.)
//
// Run using
// tutorial02 myvideofile.mpg
//
// to play the video stream on your screen.


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
#include <SDL.h>
#include <SDL_thread.h>

#ifdef __MINGW32__
#undef main /* Prevents SDL from overriding main() */
#endif

#include <stdio.h>
void *serial_BUTTON(void *parm);
void *serial_DIST(void *parm);

        
      #define BAUDRATE_BUTTON B115200
      #define MODEMDEVICE_BUTTON "/dev/ttyUSB1"
      #define BAUDRATE_DIST B19200
      #define MODEMDEVICE_DIST "/dev/ttyUSB0"
      #define _POSIX_SOURCE 1 /* POSIX compliant source */
      #define FALSE 0
      #define TRUE 1
        
      volatile int PRESSED=FALSE; 
      volatile int CLOSE=FALSE; 
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

	printf("temp");fflush(stdout);       /* so we can printf... */

        fd = open(MODEMDEVICE_BUTTON, O_RDWR | O_NOCTTY ); 
        if (fd <0) {perror(MODEMDEVICE_BUTTON); exit(-1); }
        
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
         for (i=0;i<res;i++) printf("%d:%d ", i, buf[i]);
	  //val=(buf[15]<<8)|buf[14];
          printf("\n\t\t7:...%d:%d\n\n", val,buf[1]);
	  if (buf[1] ==0) PRESSED=TRUE;
	  else PRESSED=FALSE;
          //if (buf[0]=='z') STOP=TRUE;
        }
        tcsetattr(fd,TCSANOW,&oldtio);
      }      
      void *serial_DIST(void * parm)
      {
        int fd,c, res;
        struct termios oldtio,newtio;
        unsigned char buf[255];
        char range[5];
int cnt=0;

	range[0]=0x55;
	range[1]=0xe0;
	range[2]=0x00;
	range[3]=0x01;
	range[4]=0x58;

        char readrange[4];

	readrange[0]=0x55;
	readrange[1]=0xe1;
	readrange[2]=0x02;
	readrange[3]=0x02;
	int i;int val;int j;

	printf("temp");fflush(stdout);       /* so we can printf... */

        fd = open(MODEMDEVICE_DIST, O_RDWR | O_NOCTTY ); 
        if (fd <0) {perror(MODEMDEVICE_DIST); exit(-1); }
        
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

	//range[4]=0xA0;
	//  write(fd,range,5);
	//range[4]=0xAA;
	//  write(fd,range,5);
	//range[4]=0xA5;
	//  write(fd,range,5);
	//range[4]=0xe8;
	//  write(fd,range,5);
	//exit(0);	
        
        
        while (TRUE) {       /* loop for input */
	  for (j=0xe2;j<=0xe6;j+=2){
		range[1]=j;
	  	write(fd,range,5);usleep(200000);
         	res = read(fd,buf,255);   /* returns after lull in input */
         printf("\nrc=%d:%d\n\n", val,buf[0]);
		}
	cnt=0;
 	  for (j=0xe3;j<=0xe7;j+=2){
		readrange[1]=j;
	  write(fd,readrange,4);usleep(200000);
         res = read(fd,buf,255);   /* returns after lull in input */
          buf[res]=0;        
         for (i=0;i<res;i++) printf("%d:%d ", i, buf[i]);
	  //val=(buf[15]<<8)|buf[14];
          printf("\n\t\t7:...%d:%d\n\n", val,buf[1]);
	  if (buf[1] < 30) cnt++;
	}
	if (cnt >0) CLOSE=FALSE;
	else CLOSE=TRUE;
       //   sleep(1);

        }
        tcsetattr(fd,TCSANOW,&oldtio);
      }
int main(int argc, char *argv[]) {
  AVFormatContext *pFormatCtx;
  int             i, videoStream;
  AVCodecContext  *pCodecCtx;
  AVCodec         *pCodec;
  AVFrame         *pFrame[1]; 
  AVPacket        packet[610];
  int             frameFinished;
  float           aspect_ratio;
  int startM1=0,endM1=0,startM2=0,endM2=0,modM1=0,modM2=0;
int start=0,modval=0;
	pthread_t thread;
	pthread_attr_t attr;



  SDL_Overlay     *bmp;
  SDL_Surface     *screen;
  SDL_Rect        rect;
  SDL_Event       event;

  if(argc < 2) {
    fprintf(stderr, "Usage: test <file>\n");
    exit(1);
  }
  // Register all formats and codecs
  av_register_all();
  	pthread_attr_init(&attr);

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
    fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
    exit(1);
  }

  // Open video file
  if(av_open_input_file(&pFormatCtx, argv[1], NULL, 0, NULL)!=0)
    return -1; // Couldn't open file
  
  // Retrieve stream information
  if(av_find_stream_info(pFormatCtx)<0)
    return -1; // Couldn't find stream information
  
  // Dump information about file onto standard error
  dump_format(pFormatCtx, 0, argv[1], 0);
  
  // Find the first video stream
  videoStream=-1;
  for(i=0; i<pFormatCtx->nb_streams; i++)
    if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO) {
      videoStream=i;
      break;
    }
  if(videoStream==-1)
    return -1; // Didn't find a video stream
  
  // Get a pointer to the codec context for the video stream
  pCodecCtx=pFormatCtx->streams[videoStream]->codec;
  
  // Find the decoder for the video stream
  pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
  if(pCodec==NULL) {
    fprintf(stderr, "Unsupported codec!\n");
    return -1; // Codec not found
  }
  
  // Open codec
  if(avcodec_open(pCodecCtx, pCodec)<0)
    return -1; // Could not open codec
  
  // Allocate video frame
  //for (i=0;i<300;i++)
  pFrame[0]=avcodec_alloc_frame();

  // Make a screen to put our video
#ifndef __DARWIN__
        screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 0, 0);
#else
        screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 24, 0);
#endif
  if(!screen) {
    fprintf(stderr, "SDL: could not set video mode - exiting\n");
    exit(1);
  }
  
  // Allocate a place to put our YUV image on that screen
  bmp = SDL_CreateYUVOverlay(pCodecCtx->width,
				 pCodecCtx->height,
				 SDL_YV12_OVERLAY,
				 screen);


  // Read frames and save first five frames to disk
  startM1=0;i=0;
  while(av_read_frame(pFormatCtx, &(packet[i]))>=0) {

    // Is this a packet from the video stream?
    if(packet[i].stream_index==videoStream) {
      // Decode video frame
      avcodec_decode_video(pCodecCtx, pFrame[0], &frameFinished, 
			   packet[i].data, packet[i].size);
      
      // Did we get a video frame?
      if(frameFinished) {
	SDL_LockYUVOverlay(bmp);

	AVPicture pict;
	pict.data[0] = bmp->pixels[0];
	pict.data[1] = bmp->pixels[2];
	pict.data[2] = bmp->pixels[1];

	pict.linesize[0] = bmp->pitches[0];
	pict.linesize[1] = bmp->pitches[2];
	pict.linesize[2] = bmp->pitches[1];

	// Convert the image into YUV format that SDL uses
	img_convert(&pict, PIX_FMT_YUV420P,
                    (AVPicture *)pFrame[0], pCodecCtx->pix_fmt, 
		    pCodecCtx->width, pCodecCtx->height);
	
	SDL_UnlockYUVOverlay(bmp);
	
	rect.x = 0;
	rect.y = 0;
	rect.w = pCodecCtx->width;
	rect.h = pCodecCtx->height;
	SDL_DisplayYUVOverlay(bmp, &rect);
 	i++;     
      }
    }
    
    // Free the packet that was allocated by av_read_frame
   // av_free_packet(&packet[i]);
    SDL_PollEvent(&event);
    switch(event.type) {
    case SDL_QUIT:
      SDL_Quit();
      exit(0);
      break;
    default:
      break;
    }

  }
  endM1=i-1;
  // Open video file
  if(av_open_input_file(&pFormatCtx, "ffmpeg/Static.avi", NULL, 0, NULL)!=0)
    return -1; // Couldn't open file
  
  // Retrieve stream information
  if(av_find_stream_info(pFormatCtx)<0)
    return -1; // Couldn't find stream information
  
  // Dump information about file onto standard error
  dump_format(pFormatCtx, 0, argv[1], 0);
  
  // Find the first video stream
  videoStream=-1;
  for(i=0; i<pFormatCtx->nb_streams; i++)
    if(pFormatCtx->streams[i]->codec->codec_type==CODEC_TYPE_VIDEO) {
      videoStream=i;
      break;
    }
  if(videoStream==-1)
    return -1; // Didn't find a video stream
  
  // Get a pointer to the codec context for the video stream
  pCodecCtx=pFormatCtx->streams[videoStream]->codec;
  
  // Find the decoder for the video stream
  pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
  if(pCodec==NULL) {
    fprintf(stderr, "Unsupported codec!\n");
    return -1; // Codec not found
  }
  
  // Open codec
  if(avcodec_open(pCodecCtx, pCodec)<0)
    return -1; // Could not open codec
  
  // Allocate video frame
  //for (i=300;i<600;i++)
  pFrame[0]=avcodec_alloc_frame();

  // Make a screen to put our video
#ifndef __DARWIN__
        screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 0, 0);
#else
        screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 24, 0);
#endif
  if(!screen) {
    fprintf(stderr, "SDL: could not set video mode - exiting\n");
    exit(1);
  }


  startM2=endM1+1;
  i=startM2;
  printf("startm2=%d",i);fflush(stdout);
  while(av_read_frame(pFormatCtx, &(packet[i]))>=0) {

    // Is this a packet from the video stream?
    if(packet[i].stream_index==videoStream) {
      // Decode video frame
      avcodec_decode_video(pCodecCtx, pFrame[0], &frameFinished, 
			   packet[i].data, packet[i].size);
      
      // Did we get a video frame?
      if(frameFinished) {
	SDL_LockYUVOverlay(bmp);

	AVPicture pict;
	pict.data[0] = bmp->pixels[0];
	pict.data[1] = bmp->pixels[2];
	pict.data[2] = bmp->pixels[1];

	pict.linesize[0] = bmp->pitches[0];
	pict.linesize[1] = bmp->pitches[2];
	pict.linesize[2] = bmp->pitches[1];

	// Convert the image into YUV format that SDL uses
	img_convert(&pict, PIX_FMT_YUV420P,
                    (AVPicture *)pFrame[0], pCodecCtx->pix_fmt, 
		    pCodecCtx->width, pCodecCtx->height);
	
	SDL_UnlockYUVOverlay(bmp);
	
	rect.x = 0;
	rect.y = 0;
	rect.w = pCodecCtx->width;
	rect.h = pCodecCtx->height;
	SDL_DisplayYUVOverlay(bmp, &rect);
 	i++;     
      }
    }
   
    // Free the packet that was allocated by av_read_frame
   // av_free_packet(&packet[i]);
    SDL_PollEvent(&event);
    switch(event.type) {
    case SDL_QUIT:
      SDL_Quit();
      exit(0);
      break;
    default:
      break;
    }

  }
  endM2=i-1;
//printf("trying");
	pthread_create(&thread,&attr,serial_DIST,(void *)NULL);

modM1=endM1-startM1;
modM2=endM2-startM2;
start=startM1;
modval=modM1;
while(1) {
    // Free the packet that was allocated by av_read_frame
   // av_free_packet(&packet[i]);
    SDL_PollEvent(&event);
    switch(event.type) {
    case SDL_QUIT:
      SDL_Quit();
      exit(0);
      break;
    case SDL_KEYDOWN:
	/* Any key press quits the app... */
	start=startM2;modval=modM2;
	break;    
    default:
      break;
    }
     if (CLOSE==TRUE) {start=startM1;modval=modM1;}
	else {start=startM2;modval=modM2;}
	i=((((i-start)+1)%modval)+start);
   //   printf("i=%d",i);
      avcodec_decode_video(pCodecCtx, pFrame[0], &frameFinished, 
			   packet[i].data, packet[i].size);
	SDL_LockYUVOverlay(bmp);

	AVPicture pict;
	pict.data[0] = bmp->pixels[0];
	pict.data[1] = bmp->pixels[2];
	pict.data[2] = bmp->pixels[1];

	pict.linesize[0] = bmp->pitches[0];
	pict.linesize[1] = bmp->pitches[2];
	pict.linesize[2] = bmp->pitches[1];

	// Convert the image into YUV format that SDL uses
	img_convert(&pict, PIX_FMT_YUV420P,
                    (AVPicture *)pFrame[0], pCodecCtx->pix_fmt, 
		    pCodecCtx->width, pCodecCtx->height);
	
      //avcodec_decode_video(pCodecCtx, pFrame[i], &frameFinished, 
	//		   packet[i].data, packet[i].size);	
	SDL_UnlockYUVOverlay(bmp);
	
	rect.x = 0;
	rect.y = 0;
	rect.w = pCodecCtx->width;
	rect.h = pCodecCtx->height;
	SDL_DisplayYUVOverlay(bmp, &rect);

 	//i++;     
 } 
  // Free the YUV frame
//for (i=0;i<300;i++) 
 /// av_free(pFrame[i]);
//for (i=0;i<300;i++) 
 // av_free_packet(&(packet[i]));  
  // Close the codec
  avcodec_close(pCodecCtx);
  
  // Close the video file
  av_close_input_file(pFormatCtx);
  printf(" i=%d",i);
  return 0;
}
