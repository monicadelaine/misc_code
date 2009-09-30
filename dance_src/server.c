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
#include <GL/vms_x_fix.h>

#ifdef __MINGW32__
#undef main /* Prevents SDL from overriding main() */
#endif

#include "resources.h"
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
      XQueryTree(target_dpy, RootWindow(target_dpy, DefaultScreen(target_dpy)),
                 &root, &toplevel_parent, &children, &n_children);
      XFree(children);
    }
    if (cur_focus == None || cur_focus == PointerRoot
	|| cur_focus == XtWindow(toplevel) || cur_focus == toplevel_parent) {
      /* notice user when no window focused or the xvkbd window is focused */
      return;
    }
  }

  found = FALSE;
  keycode = 0;
  if (keysym != NoSymbol) {
    for (phase = 0; phase < 2; phase++) {
      for (keycode = min_keycode; !found && (keycode <= max_keycode); keycode++) {
	/* Determine keycode for the keysym:  we use this instead
	   of XKeysymToKeycode() because we must know shift_state, too */
	inx = (keycode - min_keycode) * keysym_per_keycode;
	if (keysym_table[inx] == keysym) {
	  shift &= ~altgr_mask;
	  if (keysym_table[inx + 1] != NoSymbol) shift &= ~ShiftMask;
	  found = TRUE;
	  break;
	} else if (keysym_table[inx + 1] == keysym) {
	  shift &= ~altgr_mask;
	  shift |= ShiftMask;
	  found = TRUE;
	  break;
	}
      }
      if (!found && altgr_mask && 3 <= keysym_per_keycode) {
	for (keycode = min_keycode; !found && (keycode <= max_keycode); keycode++) {
	  inx = (keycode - min_keycode) * keysym_per_keycode;
	  if (keysym_table[inx + 2] == keysym) {
	    shift &= ~ShiftMask;
	    shift |= altgr_mask;
	    found = TRUE;
	    break;
	  } else if (4 <= keysym_per_keycode && keysym_table[inx + 3] == keysym) {
	    shift |= ShiftMask | altgr_mask;
	    found = TRUE;
	    break;
	  }
	}
      }
      if (found ) break;
      //if (found || !appres.auto_add_keysym) break;

      if (0xF000 <= keysym) {
	/* for special keys such as function keys,
	   first try to add it in the non-shifted position of the keymap */
	if (AddKeysym(keysym, TRUE) == NoSymbol) AddKeysym(keysym, FALSE);
      } else {
	AddKeysym(keysym, FALSE);
      }
    }
    if (1) {
      if (found)
	fprintf(stderr, "SendKeyPressedEvent: keysym=0x%lx, keycode=%ld, shift=0x%lX\n",
		(long)keysym, (long)keycode, (long)shift);
      else
	fprintf(stderr, "SendKeyPressedEvent: keysym=0x%lx - keycode not found\n",
		(long)keysym);
    }
  }

  event.display = target_dpy;
  event.window = cur_focus;
  event.root = RootWindow(event.display, DefaultScreen(event.display));
  event.subwindow = None;
  event.time = CurrentTime;
  event.x = 1;
  event.y = 1;
  event.x_root = 1;
  event.y_root = 1;
  event.same_screen = TRUE;

#ifdef USE_XTEST
  if (appres.xtest) {
    Window root, child;
    int root_x, root_y, x, y;
    unsigned int mask;

    XQueryPointer(target_dpy, event.root, &root, &child, &root_x, &root_y, &x, &y, &mask);

    event.type = KeyRelease;
    event.state = 0;
    if (mask & ControlMask) {
      event.keycode = XKeysymToKeycode(target_dpy, XK_Control_L);
      SendEvent(&event);
    }
    if (mask & alt_mask) {
      event.keycode = XKeysymToKeycode(target_dpy, XK_Alt_L);
      SendEvent(&event);
    }
    if (mask & meta_mask) {
      event.keycode = XKeysymToKeycode(target_dpy, XK_Meta_L);
      SendEvent(&event);
    }
    if (mask & altgr_mask) {
      if (0 < appres.altgr_keycode)
	event.keycode = appres.altgr_keycode;
      else
	event.keycode = XKeysymToKeycode(target_dpy, altgr_keysym);
      SendEvent(&event);
    }
    if (mask & ShiftMask) {
      event.keycode = XKeysymToKeycode(target_dpy, XK_Shift_L);
      SendEvent(&event);
    }
    if (mask & LockMask) {
      event.keycode = XKeysymToKeycode(target_dpy, XK_Caps_Lock);
      SendEvent(&event);
    }
  }
#endif
//printf("sending key press");
  event.type = KeyPress;
  event.state = 0;
  if (shift & ControlMask) {
    event.keycode = XKeysymToKeycode(target_dpy, XK_Control_L);
    SendEvent(&event);
    event.state |= ControlMask;
  }
  if (shift & alt_mask) {
    event.keycode = XKeysymToKeycode(target_dpy, XK_Alt_L);
    SendEvent(&event);
    event.state |= alt_mask;
  }
  if (shift & meta_mask) {
    event.keycode = XKeysymToKeycode(target_dpy, XK_Meta_L);
    SendEvent(&event);
    event.state |= meta_mask;
  }
  if (shift & altgr_mask) {
    if (0 < altgr_keycode)
      event.keycode = altgr_keycode;
    else
      event.keycode = XKeysymToKeycode(target_dpy, altgr_keysym);
    SendEvent(&event);
    event.state |= altgr_mask;
  }
  if (shift & ShiftMask) {
    event.keycode = XKeysymToKeycode(target_dpy, XK_Shift_L);
    SendEvent(&event);
    event.state |= ShiftMask;
  }

  if (keysym != NoSymbol) {  /* send event for the key itself */
    event.keycode = found ? keycode : XKeysymToKeycode(target_dpy, keysym);
    if (event.keycode == NoSymbol) {
      if ((keysym & ~0x7f) == 0 && isprint(keysym))
        fprintf(stderr, "%s: no such key: %c\n",
                PROGRAM_NAME, (char)keysym);
      else if (XKeysymToString(keysym) != NULL)
        fprintf(stderr, "%s: no such key: keysym=%s (0x%lX)\n",
                PROGRAM_NAME, XKeysymToString(keysym), (long)keysym);
      else
        fprintf(stderr, "%s: no such key: keysym=0x%lX\n",
                PROGRAM_NAME, (long)keysym);
    } else {
      SendEvent(&event);
      //event.type = KeyRelease;
      //SendEvent(&event);
    }
  }
return;
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
	pthread_t thread1;
	pthread_attr_t attr1;
	pthread_t thread2;
	pthread_attr_t attr2;
	pthread_t thread3;
	pthread_attr_t attr3;
int change = 0;

pthread_create(&thread1,&attr1,serial_BUTTON,(void *)NULL);
pthread_create(&thread2,&attr2,serial_DIST1,(void *)NULL);
pthread_create(&thread3,&attr3,serial_DIST2,(void *)NULL);


  argc1 = argc;
  argv1 = malloc(sizeof(char *) * (argc1 + 5));
  memcpy(argv1, argv, sizeof(char *) * argc1);
  argv1[argc1] = NULL;
  
  //putenv("DISPLAY=:0");

#ifdef USE_I18N
  XtSetLanguageProc(NULL, NULL, NULL);
#endif
  toplevel = XtVaAppInitialize(NULL, "temp",
                               NULL, 0,
                               &argc, argv, NULL, NULL);

  dpy = XtDisplay(toplevel);
  //app_con = XtWidgetToApplicationContext(toplevel);
  //XtAppAddActions(app_con, actions, XtNumber(actions));

  target_dpy = dpy;

  if (1 < argc) {
    fprintf(stderr, "%s: illegal option: %s\n\n", PROGRAM_NAME, argv[1]);
  }

 // XtGetApplicationResources(toplevel, &appres,
    //        application_resources, XtNumber(application_resources),
      //      NULL, 0);
    if (strcmp("video", "root") == 0) {
      focused_window = RootWindow(dpy, DefaultScreen(dpy));
    } else if (sscanf("video", "0x%lX%c", &focused_window, &ch) != 1) {
      if (sscanf("video", "%ld%c", &focused_window, &ch) != 1) {
        focused_window = FindWindow(RootWindow(dpy, DefaultScreen(dpy)),
                                    "video");
        if (focused_window == None) {
          fprintf(stderr, "%s: no such window: %s\n", PROGRAM_NAME, "video");
        }
      }
  }
  focused_subwindow = focused_window;

  //ReadKeymap();
  //if (!altgr_mask && appres.auto_add_keysym) AddModifier(XK_Mode_switch);

  //  appres.keypad_keysym = TRUE;
 ///   if (focused_window != None &&
  ///      (appres.list_widgets || strlen(appres.widget) != 0)) {
  ///    XtVaSetValues(toplevel, XtNwidth, 1, XtNheight, 1, NULL);
   ///   XtRealizeWidget(toplevel);
   ///   child = FindWidget(toplevel, focused_window, appres.widget);
    ///  if (child != None) focused_subwindow = child;
   /// }
//    if (strlen(appres.text) != 0)
//      SendString(appres.text);
//    else
//      SendFileContent(appres.file);
KeySym keysym;

SendKeyPressedEvent((KeySym)0x70, FALSE);

while(1) {
printf("%d %d a %d %d %d %d b %d %d %d %d\n",PRESSED1,PRESSED2,CLOSE1[0],CLOSE1[1],CLOSE1[2],CLOSE1[3],CLOSE2[0],CLOSE2[1],CLOSE2[2],CLOSE2[3]);
}
    exit(0);
 }
