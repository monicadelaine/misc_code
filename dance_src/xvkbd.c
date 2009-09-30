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

#include "resources.h"

int DIST1_PORT=0;
int DIST2_PORT=1;
int BUTTON_PORT=2;
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
	pthread_t thread1;
	pthread_attr_t attr1;
	pthread_t thread2;
	pthread_attr_t attr2;
	pthread_t thread3;
	pthread_attr_t attr3;
	pthread_mutexattr_t aIdx1;
	pthread_mutexattr_t aIdx2;
	pthread_mutex_t mIdx1;
	pthread_mutex_t mIdx2;

#ifdef __MINGW32__
#undef main /* Prevents SDL from overriding main() */
#endif
        
      extern volatile unsigned char PRESSED1;
      extern volatile unsigned char PRESSED2;
      extern volatile int CLOSE[8][5];
      extern volatile int idx1;
      extern volatile int idx2;
   int altgr_keycode=0;
  int error_detected = FALSE;
#ifdef __cplusplus
  extern "C" {
#endif

int TV1Thresh[8]={36,36,36,36,36,36,36,36};
int TV2Thresh[8]={36,36,36,36,36,36,36,36};

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <errno.h>

#ifndef STDIN_FILENO
  #define STDIN_FILENO 0
#endif
int screenNo=1;

extern int errno;                 

static struct termios termattr, save_termattr;
static int ttysavefd = -1;
static enum 
{ 
  RESET, RAW, CBREAK 
} ttystate = RESET;

int belowThresh(int tv[],int idx,int ignore) {

int below=0;int i;
for (i=0;i<5;i++) 
 if (CLOSE[idx][i] >3 && CLOSE[idx][i] < tv[idx] && i != ignore)
    below++;
//     printf("below (%d:%d) %d < %d\n",i,idx,CLOSE[idx][i],tv[idx]);
return below;


}

/* ***************************************************************************
 *
 * set_tty_raw(), put the user's TTY in one-character-at-a-time mode.
 * returns 0 on success, -1 on failure.
 *
 *************************************************************************** */
int
set_tty_raw(void) 
{
  int i;

  i = tcgetattr (STDIN_FILENO, &termattr);
  if (i < 0) 
  {
    printf("tcgetattr() returned %d for fildes=%d\n",i,STDIN_FILENO); 
    perror ("");
    return -1;
  }
  save_termattr = termattr;

  termattr.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
  termattr.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
  termattr.c_cflag &= ~(CSIZE | PARENB);
  termattr.c_cflag |= CS8;
  termattr.c_oflag &= ~(OPOST);
   
  termattr.c_cc[VMIN] = 1;  /* or 0 for some Unices;  see note 1 */
  termattr.c_cc[VTIME] = 0;

  i = tcsetattr (STDIN_FILENO, TCSANOW, &termattr);
  if (i < 0) 
  {
    printf("tcsetattr() returned %d for fildes=%d\n",i,STDIN_FILENO); 
    perror("");
    return -1;
  }
   
  ttystate = RAW;
  ttysavefd = STDIN_FILENO;

  return 0;
}

/* ***************************************************************************
 *
 * set_tty_cbreak(), put the user's TTY in cbreak mode.
 * returns 0 on success, -1 on failure.
 *
 *************************************************************************** */
int 
set_tty_cbreak() 
{
  int i;

  i = tcgetattr (STDIN_FILENO, &termattr);
  if (i < 0) 
  {
    printf("tcgetattr() returned %d for fildes=%d\n",i,STDIN_FILENO); 
    perror ("");
    return -1;
  }

  save_termattr = termattr;

  termattr.c_lflag &= ~(ECHO | ICANON);
  termattr.c_cc[VMIN] = 1;
  termattr.c_cc[VTIME] = 0;
      
  i = tcsetattr (STDIN_FILENO, TCSANOW, &termattr);
  if (i < 0) 
  {
    printf("tcsetattr() returned %d for fildes=%d\n",i,STDIN_FILENO); 
    perror ("");
    return -1;
  }
  ttystate = CBREAK;
  ttysavefd = STDIN_FILENO;

  return 0;
}

/* ***************************************************************************
 *
 * set_tty_cooked(), restore normal TTY mode. Very important to call
 *   the function before exiting else the TTY won't be too usable.
 * returns 0 on success, -1 on failure.
 *
 *************************************************************************** */
int
set_tty_cooked() 
{
  int i;
  if (ttystate != CBREAK && ttystate != RAW) 
  {
    return 0;
  }
  i = tcsetattr (STDIN_FILENO, TCSAFLUSH, &save_termattr);
  if (i < 0) 
  {
    return -1;
  }
  ttystate = RESET;
  return 0;
}

/* ***************************************************************************
 *
 * kb_getc(), if there's a typed character waiting to be read,
 *   return it; else return 0.
 * 10-sep-2006: kb_getc() fails (it hangs on the read() and never returns
 * until a char is typed) under some Unix/Linux versions: ubuntu, suse, and
 * maybe others. To make it work, please uncomment two source lines below.
 *
 *************************************************************************** */
unsigned char
kb_getc(void) 
{
  int i;
  unsigned char ch;
  ssize_t size;
/*  termattr.c_cc[VMIN] = 0; */ /* uncomment if needed */
  i = tcsetattr (STDIN_FILENO, TCSANOW, &termattr);
  size = read (STDIN_FILENO, &ch, 1);
/*  termattr.c_cc[VMIN] = 1; */ /* uncomment if needed */
  i = tcsetattr (STDIN_FILENO, TCSANOW, &termattr);
  if (size == 0)
  {
    return 0;
  }
  else
  {
    return ch;
  }
}

/* ***************************************************************************
 *
 * kb_getc_w(), wait for a character to be typed and return it.
 *
 *************************************************************************** */
unsigned char
kb_getc_w(void) 
{
  unsigned char ch;
  size_t size;

  while (1)
  {

    usleep(20000);        /* 1/50th second: thanks, Floyd! */

    size = read (STDIN_FILENO, &ch, 1);
    if (size > 0)
    {
      break;
    }
  }
  return ch;
}


#define TEST
#ifdef TEST

void echo(unsigned char ch);

static enum 
{ 
  CH_ONLY, CH_HEX 
} how_echo = CH_ONLY;

void
echo(unsigned char ch)
{
  switch (how_echo)
  {
  case CH_HEX:
    printf("%c,0x%x  ",ch,ch);
    break;
  default:
  case CH_ONLY:
    printf("%c", ch);
    printf("%c,0x%x  ",ch,ch);
    break;
  }

  fflush(stdout);      /* push it out */
}

#endif /* test */


#ifdef __cplusplus
}
#endif




static int MyErrorHandler(Display *my_dpy, XErrorEvent *event)
{
  char msg[200];

  error_detected = TRUE;
  if (event->error_code == BadWindow) {
    if (1)
      fprintf(stderr, "%s: BadWindow - couldn't find target window 0x%lX (destroyed?)\n",
	      PROGRAM_NAME, (long)focused_window);
    return 0;
  }
  XGetErrorText(my_dpy, event->error_code, msg, sizeof(msg) - 1);
  fprintf(stderr, "X error trapped: %s, request-code=%d\n", msg, event->request_code);
  return 0;
}

/*
 * Send event to the focused window.
 * If input focus is specified explicitly, select the window
 * before send event to the window.
 */
static void SendEvent(XKeyEvent *event)
{
  static Boolean first = TRUE;
  if (1) {
    XSync(event->display, FALSE);
    XSetErrorHandler(MyErrorHandler);
  }

  error_detected = FALSE;
  if (focused_window != None) {
    /* set input focus if input focus is set explicitly */
    if (0)
      fprintf(stderr, "Set input focus to window 0x%lX (0x%lX)\n",
              (long)focused_window, (long)event->window);
    XSetInputFocus(event->display, focused_window, RevertToParent, CurrentTime);
    if (1) XSync(event->display, FALSE);
  }
  if (!error_detected) {
    if (0) {

      fprintf(stderr, "%s: this binary is compiled without XTEST support\n",
	      PROGRAM_NAME);
    } else {
      XSendEvent(event->display, event->window, TRUE, KeyPressMask, (XEvent *)event);
      if (1) XSync(event->display, FALSE);

      if (error_detected
	  && (focused_subwindow != None) && (focused_subwindow != event->window)) {
	error_detected = FALSE;
	event->window = focused_subwindow;
	if (1)
	  fprintf(stderr, "   retry: send event to window 0x%lX (0x%lX)\n",
		  (long)focused_window, (long)event->window);
	XSendEvent(event->display, event->window, TRUE, KeyPressMask, (XEvent *)event);
	if (1) XSync(event->display, FALSE);
      }
    }
  }

  if (error_detected) {
    /* reset focus because focused window is (probably) no longer exist */
    printf("error detected");

    focused_window = None;
    focused_subwindow = None;
  }

  XSetErrorHandler(NULL);
}

static Window FindWindow(Window top, char *name)
{
  Window w;
  Window *children, dummy;
  unsigned int nchildren;
  int i;
  XClassHint hint;

  w = None;

  if (0) fprintf(stderr, "FindWindow: id=0x%lX", (long)top);

  if (XGetClassHint(target_dpy, top, &hint)) {
    if (hint.res_name) {
      if (0) fprintf(stderr, " instance=\"%s\"", hint.res_name);
      if (strcmp(hint.res_name, name) == 0) w = top;
      XFree(hint.res_name);
    }
    if (hint.res_class) {
      if (0) fprintf(stderr, " class=\"%s\"", hint.res_class);
      if (strcmp(hint.res_class, name) == 0) w = top;
      XFree(hint.res_class);
    }
  }
  if (0) fprintf(stderr, "\n");

  if (w == None &&
      XQueryTree(target_dpy, top, &dummy, &dummy, &children, &nchildren)) {
    for (i = 0; i < nchildren; i++) {
      w = FindWindow(children[i], name);
      if (w != None) break;
    }
    if (children) XFree((char *)children);
  }

  return(w);
}
/*
 * Send sequence of KeyPressed/KeyReleased events to the focused
 * window to simulate keyboard.  If modifiers (shift, control, etc)
 * are set ON, many events will be sent.
 */
static void SendKeyPressedEvent(KeySym keysym, unsigned int shift)
{
  Window cur_focus;
  int revert_to;
  XKeyEvent event;
  int keycode;
  Window root, *children;
  unsigned int n_children;
  int phase, inx;
  int found;

  if (focused_subwindow != None)
    cur_focus = focused_subwindow;
  else
    XGetInputFocus(target_dpy, &cur_focus, &revert_to);

  if (1) {
    char ch = '?';
    if ((keysym & ~0x7f) == 0 && isprint(keysym)) ch = keysym;
    fprintf(stderr, "SendKeyPressedEvent: focus=0x%lX, key=0x%lX (%c), shift=0x%lX\n",
            (long)cur_focus, (long)keysym, ch, (long)shift);
  }

  if (XtWindow(toplevel) != None) {
    if (toplevel_parent == None) {
      //XQueryTree(target_dpy, RootWindow(target_dpy, DefaultScreen(target_dpy)),
      XQueryTree(target_dpy, RootWindow(target_dpy, screenNo),
                 &root, &toplevel_parent, &children, &n_children);
      XFree(children);
    }
    if (cur_focus == None || cur_focus == PointerRoot
	|| cur_focus == XtWindow(toplevel) || cur_focus == toplevel_parent) {
      /* notice user when no window focused or the xvkbd window is focused */
      return;
    }
  }

	//fprintf(stderr, "SendKeyPressedEvent: keysym=0x%lx, keycode=%ld, shift=0x%lX\n",
		//(long)keysym, (long)keycode, (long)shift);

  event.display = target_dpy;
  event.window = cur_focus;
  event.root = RootWindow(event.display, screenNo);
  //event.root = RootWindow(event.display, DefaultScreen(event.display));
 event.subwindow = None;
  event.time = CurrentTime;
  event.x = 1;
  event.y = 1;
  event.x_root = 1;
  event.y_root = 1;
  event.same_screen = TRUE;


  event.type = KeyPress;
  event.state = 0;
    event.keycode = XKeysymToKeycode(target_dpy, keysym);
      SendEvent(&event);
      //event.type = KeyRelease;
      //SendEvent(&event);
return;
}

static void SendKeyReleasedEvent(KeySym keysym, unsigned int shift)
{
  Window cur_focus;
  int revert_to;
  XKeyEvent event;
  int keycode;
  Window root, *children;
  unsigned int n_children;
  int phase, inx;
  int found;

  if (focused_subwindow != None)
    cur_focus = focused_subwindow;
  else
    XGetInputFocus(target_dpy, &cur_focus, &revert_to);

  if (1) {
    char ch = '?';
    if ((keysym & ~0x7f) == 0 && isprint(keysym)) ch = keysym;
    fprintf(stderr, "SendKeyReleasedEvent: focus=0x%lX, key=0x%lX (%c), shift=0x%lX\n",
            (long)cur_focus, (long)keysym, ch, (long)shift);
  }

  if (XtWindow(toplevel) != None) {
    if (toplevel_parent == None) {
      //XQueryTree(target_dpy, RootWindow(target_dpy, DefaultScreen(target_dpy)),
      XQueryTree(target_dpy, RootWindow(target_dpy, screenNo),
                 &root, &toplevel_parent, &children, &n_children);
      XFree(children);
    }
    if (cur_focus == None || cur_focus == PointerRoot
	|| cur_focus == XtWindow(toplevel) || cur_focus == toplevel_parent) {
      /* notice user when no window focused or the xvkbd window is focused */
      return;
    }
  }


  event.display = target_dpy;
  event.window = cur_focus;
  //event.root = RootWindow(event.display, DefaultScreen(event.display));
  event.root = RootWindow(event.display, screenNo);
 event.subwindow = None;
  event.time = CurrentTime;
  event.x = 1;
  event.y = 1;
  event.x_root = 1;
  event.y_root = 1;
  event.same_screen = TRUE;


  event.type = KeyRelease;
  event.state = 0;
    event.keycode = XKeysymToKeycode(target_dpy, keysym);
      SendEvent(&event);
return;
}
void changeFocus(int no) {
	if (screenNo==no) return;
	   screenNo=no;
        focused_window = FindWindow(RootWindow(dpy, screenNo), "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
  focused_subwindow = focused_window;
}
/*
 * The main program.
 */
int main(int argc, char *argv[])
{

  char ch;
  Window child;
  int op, ev, err;
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
int change = 0;
int SERIAL=FALSE;
int in;
pthread_mutex_init(&mIdx1,&aIdx1);
pthread_mutex_init(&mIdx2,&aIdx2);
int i1=0;
int i2=0;
char buffer[255];

fflush(stdout);
  if (argc == 4) {
	//then we are serial
	DIST1_PORT=atoi(argv[1]);
	DIST2_PORT=atoi(argv[2]);
	BUTTON_PORT=atoi(argv[3]);
	SERIAL=TRUE;
 } 
 if (argc == 2) {
printf("process_video DIST1_USB DIST2_USB DIST3_USB\nprocess_video 0 2 1\n");
}

if (argc != 1 && argc != 4) SERIAL=FALSE;
printf("done %d",SERIAL);
fflush(stdout);
FILE* fid = fopen("threshhold","r");
//read(fid,buffer,255);
//printf(" %s\n",buffer);
fscanf(fid,"%d %d %d %d %d %d %d %d",&TV1Thresh[0],&TV1Thresh[1],&TV1Thresh[2],&TV1Thresh[3],&TV1Thresh[4],&TV1Thresh[5],&TV1Thresh[6],&TV1Thresh[7]);
printf("Threshholds for TV1 %d %d %d %d %d %d %d %d\n",TV1Thresh[0],TV1Thresh[1],TV1Thresh[2],TV1Thresh[3],TV1Thresh[4],TV1Thresh[5],TV1Thresh[6],TV1Thresh[7]);
//read(fid,buffer,255);
//printf(" %s\n",buffer);
fscanf(fid,"%d %d %d %d %d %d %d %d",&TV2Thresh[0],&TV2Thresh[1],&TV2Thresh[2],&TV2Thresh[3],&TV2Thresh[4],&TV2Thresh[5],&TV2Thresh[6],&TV2Thresh[7]);
printf("Threshholds for TV2 %d %d %d %d %d %d %d %d\n",TV2Thresh[0],TV2Thresh[1],TV2Thresh[2],TV2Thresh[3],TV2Thresh[4],TV2Thresh[5],TV2Thresh[6],TV2Thresh[7]);
fclose(fid);


#ifdef USE_I18N
  XtSetLanguageProc(NULL, NULL, NULL);
#endif
putenv("DISPLAY=:0");
  toplevel = XtVaAppInitialize(NULL, "temp",
                               NULL, 0,
                               &argc, argv, NULL, NULL);

  dpy = XtDisplay(toplevel);
  //app_con = XtWidgetToApplicationContext(toplevel);
  //XtAppAddActions(app_con, actions, XtNumber(actions));

  target_dpy = dpy;

    if (strcmp("video", "root") == 0) {
      focused_window = RootWindow(dpy, screenNo);
    } else if (sscanf("video", "0x%lX%c", &focused_window, &ch) != 1) {
      if (sscanf("video", "%ld%c", &focused_window, &ch) != 1) {
        focused_window = FindWindow(RootWindow(dpy, screenNo), "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
      }
  }
  focused_subwindow = focused_window;

if (SERIAL) {
KeySym keysym;
int p1chg=0,p2chg=0;
int c1chg=0,c2chg=0;

printf("doing args %d",argc);
fflush(stdout);
//SendKeyPressedEvent((KeySym)0x70, FALSE);
//sleep(10);
//SendKeyReleasedEvent((KeySym)0x70, FALSE);
pthread_create(&thread1,&attr1,serial_BUTTON,(void *)NULL);
sleep(1);
printf("done1");
fflush(stdout);
pthread_create(&thread2,&attr2,serial_DIST1,(void *)NULL);
sleep(1);
printf("done2");
fflush(stdout);
pthread_create(&thread3,&attr3,serial_DIST2,(void *)NULL);
sleep(1);
printf("done");
fflush(stdout);
int step=0;

while(1) {
usleep(100000);
pthread_mutex_lock(&mIdx1);
i1=idx1;
pthread_mutex_unlock(&mIdx1);
pthread_mutex_lock(&mIdx2);
i2=idx2;
pthread_mutex_unlock(&mIdx2);
if ((i1-1) == -1) i1=4;
else i1--;
if ((i2-1) == -1) i2=4;
else i2--;
	//printf("\n%d %d a %d %d %d %d b %d %d %d %d",PRESSED1,PRESSED2,CLOSE[0][i1],CLOSE[1][i1],CLOSE[2][i1],CLOSE[3][i1],CLOSE[4][i2],CLOSE[5][i2],CLOSE[6][i2],CLOSE[7][i2]);
	//printf("\n%d %d a %d %d %d %d b %d %d %d %d",PRESSED1,PRESSED2,CLOSE[0][i1],CLOSE[1][i1],CLOSE[2][i1],CLOSE[3][i1],CLOSE[4][i2],CLOSE[5][i2],CLOSE[6][i2],CLOSE[7][i2]);
//	printf("\n%d %d %d %d",i1,i2,belowThresh(TV1Thresh,0,i1),belowThresh(TV1Thresh,1,i1));
//	printf("\n %d %d %d %d",PRESSED1,p1chg,PRESSED2,p2chg);
//	printf("%d %d %d",step++,c1chg,c2chg);
fflush(stdout);

	if (PRESSED1 && p1chg==0) {
		printf("changing 1 to pressed");
		changeFocus(0);
		SendKeyPressedEvent((KeySym)0x70, FALSE);
		p1chg=1;
	} else if (!PRESSED1 && p1chg==1){
		printf("changing 1 to released");
		changeFocus(0);
		SendKeyPressedEvent((KeySym)0x71, FALSE);
		p1chg=0;
	}
	if (PRESSED2 && p2chg==0) {
		printf("changing 2 to pressed");
		changeFocus(1);
		SendKeyPressedEvent((KeySym)0x70, FALSE);
		p2chg=1;
	} else if (!PRESSED2 && p2chg==1){
		printf("changing 2 to released");
		changeFocus(1);
		SendKeyPressedEvent((KeySym)0x71, FALSE);
		p2chg=0;
	}
	if (((belowThresh(TV2Thresh,0,i1) > 1) ||
	   (belowThresh(TV2Thresh,1,i1) > 1) ||
	   (belowThresh(TV2Thresh,2,i1) > 1) ||
	   (belowThresh(TV2Thresh,3,i1) > 1) ||
	   (belowThresh(TV2Thresh,4,i2) > 1) ||
	   (belowThresh(TV2Thresh,5,i2) > 1) ||
	   (belowThresh(TV2Thresh,6,i2) > 1) ||
	   (belowThresh(TV2Thresh,7,i2) > 1))
	&&	!PRESSED2 )
		if ( c2chg==0 ){
		printf("changing 2 to close");
		changeFocus(1);
		SendKeyPressedEvent((KeySym)0x63, FALSE);
		c2chg=1;
		changeFocus(0);
		SendKeyPressedEvent((KeySym)0x61, FALSE);
		} else {
		}
	 else if (c2chg ==1 ){
		printf("changing 2 to unclose");
		changeFocus(1);
		SendKeyPressedEvent((KeySym)0x64, FALSE);
		c2chg=0;
		changeFocus(0);
		SendKeyPressedEvent((KeySym)0x61, FALSE);
	}
	if (((belowThresh(TV1Thresh,0,i1) > 1) ||
	   (belowThresh(TV1Thresh,1,i1) > 1) ||
	   (belowThresh(TV1Thresh,2,i1) > 1) ||
	   (belowThresh(TV1Thresh,3,i1) > 1) ||
	   (belowThresh(TV1Thresh,4,i2) > 1) ||
	   (belowThresh(TV1Thresh,5,i2) > 1) ||
	   (belowThresh(TV1Thresh,6,i2) > 1) ||
	   (belowThresh(TV1Thresh,7,i2) > 1))
	&&	!PRESSED1 ) {
		if ( c1chg==0 ){
		printf("changing 1 to close");
		changeFocus(0);
		SendKeyPressedEvent((KeySym)0x63, FALSE);
		c1chg=1;
		changeFocus(1);
		SendKeyPressedEvent((KeySym)0x61, FALSE);
		} else {
		}
	} else if (c1chg ==1 ){
		printf("changing 1 to unclose");
		changeFocus(0);
		SendKeyPressedEvent((KeySym)0x64, FALSE);
		c1chg=0;
		changeFocus(1);
		SendKeyPressedEvent((KeySym)0x61, FALSE);
	}
}
} else {
//keyboard input
  unsigned char ch;

  printf("Test Unix single-character input.\n");

  set_tty_raw();         /* set up character-at-a-time */
  
  while (1)              /* wait here for a typed char */
  {
    usleep(20000);       /* 1/50th second: thanks, Floyd! */
    ch = kb_getc();      /* char typed by user? */
    if (0x03 == ch)      /* might be control-C */
    {
      set_tty_cooked();  /* control-C, restore normal TTY mode */
      return 1;          /* and get out */
    }
    //echo(ch);            /* not control-C, echo it */
   if (ch=='a'){
	   screenNo=0;
        focused_window = FindWindow(RootWindow(dpy, screenNo), "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
  focused_subwindow = focused_window;
	   SendKeyPressedEvent((KeySym)0x69, FALSE);
	//	changeFocus(1);
	//	SendKeyPressedEvent((KeySym)0x61, FALSE);
	   screenNo=1;
        focused_window = FindWindow(RootWindow(dpy, screenNo), "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
  focused_subwindow = focused_window;
	   SendKeyPressedEvent((KeySym)0x69, FALSE);
		changeFocus(0);
		SendKeyPressedEvent((KeySym)0x61, FALSE);
   }
   if (ch=='z'){
	   screenNo=0;
        focused_window = FindWindow(RootWindow(dpy, screenNo), "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
  focused_subwindow = focused_window;
	   SendKeyPressedEvent((KeySym)0x6a, FALSE);
	   screenNo=1;
        focused_window = FindWindow(RootWindow(dpy, screenNo), "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
  focused_subwindow = focused_window;
	   SendKeyPressedEvent((KeySym)0x6a, FALSE); //0x69 is i for ignore
		changeFocus(1);
		SendKeyPressedEvent((KeySym)0x61, FALSE);
   }
   if (ch=='1'){
	   screenNo=1;
        focused_window = FindWindow(RootWindow(dpy, screenNo), "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
  focused_subwindow = focused_window;
	   SendKeyPressedEvent((KeySym)0x63, FALSE);
		changeFocus(0);
		SendKeyPressedEvent((KeySym)0x61, FALSE);
   }
   if (ch=='3'){
	   screenNo=1;
        focused_window = FindWindow(RootWindow(dpy, screenNo), "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
  focused_subwindow = focused_window;
	   SendKeyPressedEvent((KeySym)0x71, FALSE); //0x69 is i for ignore
		changeFocus(0);
		SendKeyPressedEvent((KeySym)0x61, FALSE);
}
   if (ch=='2'){
	   screenNo=1;
        focused_window = FindWindow(RootWindow(dpy, screenNo), "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
  focused_subwindow = focused_window;
	   SendKeyPressedEvent((KeySym)0x64, FALSE); //0x69 is i for ignore
		changeFocus(0);
		SendKeyPressedEvent((KeySym)0x61, FALSE);
   }
  

   if (ch=='9'){
	   screenNo=0;
        focused_window = FindWindow(RootWindow(dpy, screenNo), "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
  focused_subwindow = focused_window;
	   SendKeyPressedEvent((KeySym)0x63, FALSE);
		changeFocus(1);
		SendKeyPressedEvent((KeySym)0x61, FALSE);
   }
   if (ch=='-'){
	   screenNo=0;
        focused_window = FindWindow(RootWindow(dpy, screenNo), "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
  focused_subwindow = focused_window;
	   SendKeyPressedEvent((KeySym)0x71, FALSE); //0x69 is i for ignore
		changeFocus(1);
		SendKeyPressedEvent((KeySym)0x61, FALSE);
   }
   if (ch=='0'){
	   screenNo=0;
        focused_window = FindWindow(RootWindow(dpy, screenNo), "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
  focused_subwindow = focused_window;
	   SendKeyPressedEvent((KeySym)0x64, FALSE); //0x69 is i for ignore
		changeFocus(1);
		SendKeyPressedEvent((KeySym)0x61, FALSE);
   }
  }

}//end if serial
    exit(0);
 }
