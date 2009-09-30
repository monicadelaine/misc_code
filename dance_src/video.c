// Code based on FFplay, Copyright (c) 2003 Fabrice Bellard, 
// and a tutorial by Martin Bohme (boehme@inb.uni-luebeckREMOVETHIS.de)
// Tested on Gentoo, CVS version 5/01/07 compiled with GCC 4.1.1
// Use
//
// gcc -o tutorial02 tutorial02.c -lavformat -lavcodec -lz -lm -lavutil `sdl-config --cflags --libs`
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
#define FALSE 0
#define TRUE 1

int PRESSED=0;
int CLOSE=0;
int ignore=0;
    void PrintKeyInfo( SDL_KeyboardEvent *key );
    void PrintModifiers( SDLMod mod );

        
int main(int argc, char *argv[]) {
  AVFormatContext *pFormatCtx;
  int             i, videoStream;
  AVCodecContext  *pCodecCtx;
  AVCodec         *pCodec;
  AVFrame         *pFrame[1]; 
  AVPacket        packet[910];
  int             frameFinished;
  float           aspect_ratio;
  int startM1=0,endM1=0,startM2=0,endM2=0,modM1=0,modM2=0;
  int startM3=0,endM3=0,modM3=0;
int start=0,modval=0;
	pthread_t thread;
	pthread_attr_t attr;
int ignore=FALSE;Uint8  *keystate;
int change = 0;
int display=-9;

  SDL_Overlay     *bmp;
  SDL_Surface     *screen;
  SDL_Rect        rect;
  SDL_Event       event;
//system("export DISPLAY=0.1\n");
  if(argc < 2) {
    fprintf(stderr, "Usage: test <file>\n");
    exit(1);
  }
	display=atoi(argv[1]);
if (display != 0 && display != 1) {
    fprintf(stderr, "Usage: video [0|1] \n");
    exit(1);
  }
  // Register all formats and codecs
  char buffer[20];
  sprintf(buffer,"DISPLAY=:0.%d",display);
  printf("%s\n",buffer);
  putenv(buffer);
  av_register_all();
  	pthread_attr_init(&attr);

  if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER)) {
    fprintf(stderr, "Could not initialize SDL - %s\n", SDL_GetError());
    exit(1);
  }
SDL_WM_SetCaption("video","video");

  // Open video file
  if(av_open_input_file(&pFormatCtx, "./Noise.avi", NULL, 0, NULL)!=0)
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
        screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 0, SDL_RESIZABLE);
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


  }
  endM1=i-1;
  // Open video file
  if(av_open_input_file(&pFormatCtx, "./Static.avi", NULL, 0, NULL)!=0)
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
        screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 0, SDL_RESIZABLE);
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


  }
  endM2=i-1;

  // Open video file
  if(av_open_input_file(&pFormatCtx, "./Blank.avi", NULL, 0, NULL)!=0)
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
        screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 0, SDL_RESIZABLE);
#else
        screen = SDL_SetVideoMode(pCodecCtx->width, pCodecCtx->height, 24, 0);
#endif
  if(!screen) {
    fprintf(stderr, "SDL: could not set video mode - exiting\n");
    exit(1);
  }


  startM3=endM2+1;
  i=startM3;
  printf("startm3=%d",i);fflush(stdout);
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


  }
  endM3=i-1;

	//pthread_create(&thread,&attr,serial_DIST,(void *)NULL);
//	pthread_create(&thread,&attr,serial_BUTTON,(void *)NULL);

ignore=FALSE;
modM1=endM1-startM1;
modM2=endM2-startM2;
modM3=endM3-startM3;
start=startM1;
modval=modM1;
int count=0;
//	printf("start1=%d modvalM1=%d startM2=%d modvalM2=%d\n",startM1,modM1,startM2,modM2);
while(1) {

   Uint8 *keystate1 = SDL_GetKeyState(NULL);
     //if (keystate1[SDLK_i]) {ignore=TRUE;//printf("still down\n");}
    // Free the packet that was allocated by av_read_frame
   // av_free_packet(&packet[i]);
//printf("checking...\n",event.type);fflush(stdout);
    if (SDL_PollEvent(&event) > 0) {
//printf("polling...%d\n",event.type);fflush(stdout);
    switch(event.type) {
    case SDL_QUIT:
      SDL_Quit();
      exit(0);
      break;
    case SDL_KEYDOWN:
     if (event.key.keysym.sym==0x69) {ignore=TRUE;}
     if (event.key.keysym.sym==0x63) {CLOSE=TRUE;}
     if (event.key.keysym.sym==0x70) {PRESSED=TRUE;}
     if (event.key.keysym.sym==0x6a) {ignore=FALSE;}
     if (event.key.keysym.sym==0x64) {CLOSE=FALSE;}
     if (event.key.keysym.sym==0x71) {PRESSED=FALSE;}
printf("\nKey down");
PrintKeyInfo( &event.key );
	break;    
    case SDL_KEYUP:
     //if (event.key.keysym.scancode=0x1f) {ignore=TRUE;}
     //if (event.key.keysym.scancode=0x64) {CLOSE=TRUE;}
     //if (event.key.keysym.scancode=0x21) {PRESSED=TRUE;}
     //if (event.key.keysym.scancode=0x20) {ignore=FALSE;}
     //if (event.key.keysym.scancode=0x37) {CLOSE=FALSE;}
     //if (event.key.keysym.scancode=0x22) {PRESSED=FALSE;}
	printf("\nKey up");
	PrintKeyInfo( &event.key );
	break;    
    default:
      break;
    } fflush(stdout);}
	if (PRESSED==FALSE && (CLOSE == FALSE ||ignore==TRUE)) {
			start=startM2;modval=modM2;			
			if (change != 2)
			{
	printf("CLOSE=%d ignore=%d pressed=%d change=%d %d\n",CLOSE,ignore,PRESSED,change,start);
				count=10;
				//start = 0;
				change = 2;
				i = start;
				
			}
		}
     else if (PRESSED==TRUE && ignore==FALSE) {
			start=startM3;modval=modM3;			
			if (change != 0)
			{
	printf("CLOSE=%d ignore=%d pressed=%d change=%d %d\n",CLOSE,ignore,PRESSED,change,start);
				count=0;
	//			start = 0;
				change = 0;
				i = start;
				
			}
	
				}
	 else { 
	//if (CLOSE == TRUE || ignore==FALSE)
		
    			if (count <=0 ) {
start=startM1;modval=modM1;//printf("Turned off");
			if (change != 1)
			{
	printf("CLOSE=%d ignore=%d pressed=%d change=%d %d\n",CLOSE,ignore,PRESSED,change,start);
	//			start = 0;
				change = 1;
				i = start;
			}
	} else {count--;}
			}

//	i=((((i-start)+1)%modval)+start);
 //   	printf("i=%d moval=%d mod=%d start=%d",i,modval,(i+1)%modval,start);
	i=(((i+1)%modval)+start);
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

    /* Print all information about a key event */
    void PrintKeyInfo( SDL_KeyboardEvent *key ){
        /* Is it a release or a press? */
        if( key->type == SDL_KEYUP )
            printf( "Release:- " );
        else
            printf( "Press:- " );

        /* Print the hardware scancode first */
        printf( "Sym: 0x%02X", key->keysym.sym );
        /* Print the name of the key */
        printf( ", Name: %s", SDL_GetKeyName( key->keysym.sym ) );
        /* We want to print the unicode info, but we need to make */
        /* sure its a press event first (remember, release events */
        /* don't have unicode info                                */
        if( key->type == SDL_KEYDOWN ){
            /* If the Unicode value is less than 0x80 then the    */
            /* unicode value can be used to get a printable       */
            /* representation of the key, using (char)unicode.    */
            printf(", Unicode: " );
            if( key->keysym.unicode < 0x80 && key->keysym.unicode > 0 ){
                printf( "%c (0x%04X)", (char)key->keysym.unicode,
                        key->keysym.unicode );
            }
            else{
                printf( "? (0x%04X)", key->keysym.unicode );
            }
        }
        printf( "\n" );
        /* Print modifier info */
        PrintModifiers( key->keysym.mod );
    }

    /* Print modifier info */
    void PrintModifiers( SDLMod mod ){
        printf( "Modifers: " );

        /* If there are none then say so and return */
        if( mod == KMOD_NONE ){
            printf( "None\n" );
            return;
        }

        /* Check for the presence of each SDLMod value */
        /* This looks messy, but there really isn't    */
        /* a clearer way.                              */
        if( mod & KMOD_NUM ) printf( "NUMLOCK " );
        if( mod & KMOD_CAPS ) printf( "CAPSLOCK " );
        if( mod & KMOD_LCTRL ) printf( "LCTRL " );
        if( mod & KMOD_RCTRL ) printf( "RCTRL " );
        if( mod & KMOD_RSHIFT ) printf( "RSHIFT " );
        if( mod & KMOD_LSHIFT ) printf( "LSHIFT " );
        if( mod & KMOD_RALT ) printf( "RALT " );
        if( mod & KMOD_LALT ) printf( "LALT " );
        if( mod & KMOD_CTRL ) printf( "CTRL " );
        if( mod & KMOD_SHIFT ) printf( "SHIFT " );
        if( mod & KMOD_ALT ) printf( "ALT " );
        printf( "\n" );
    }
