#include <stdio.h>
void *serial_BUTTON(void *parm);
void *serial_DIST2(void *parm);
void *serial_DIST1(void *parm);
#define PROGRAM_NAME "server"
        
      #define BAUDRATE_BUTTON B115200
      #define MODEMDEVICE_BUTTON "/dev/ttyUSB0"
      #define BAUDRATE_DIST B19200
      #define MODEMDEVICE_DIST1 "/dev/ttyUSB1"
      #define MODEMDEVICE_DIST2 "/dev/ttyUSB2"
      #define _POSIX_SOURCE 1 /* POSIX compliant source */
      #define FALSE 0
      #define TRUE 1

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include <paths.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <X11/cursorfont.h>
#include <X11/Xproto.h>  /* to get request code */
#include <X11/Xaw/Box.h>
#include <X11/Xaw/Form.h>
#include <X11/Xaw/Command.h>
#include <X11/Xaw/Repeater.h>
#include <X11/Xaw/Label.h>
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>
#include <X11/Xaw/AsciiText.h>
#include <X11/Xaw/Viewport.h>
#include <X11/Xaw/List.h>
#include <X11/Xmu/WinUtil.h>
#include <X11/keysymdef.h>
#include <X11/Xlib.h>

#ifdef USE_I18N
# include <X11/Xlocale.h>
#endif

#ifdef USE_XTEST
# include <X11/extensions/XTest.h>
#endif

//#include "resources.h"
#define PROGRAM_NAME_WITH_VERSION "xvkbd (v2.8)"

/*
 * Default keyboard layout is hardcoded here.
 * Layout of the main keyboard can be redefined by resources.
 */
#define NUM_KEY_ROWS     6
#define NUM_KEY_COLS    20

static char *keys_normal[NUM_KEY_ROWS][NUM_KEY_COLS] = {
  { "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "BackSpace" },
  { "Escape", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "\\", "`" },
  { "Tab", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "Delete" },
  { "Control_L", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "Return" },
  { "Shift_L", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "Multi_key", "Shift_R" },
  { "MainMenu", "Caps_Lock", "Alt_L", "Meta_L", "space", "Meta_R", "Alt_R",
    "Left", "Right", "Up", "Down", "Focus" },
};
static char *keys_shift[NUM_KEY_ROWS][NUM_KEY_COLS] = {
  { "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "BackSpace" },
  { "Escape", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "|", "~" },
  { "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "Delete" },
  { "Control_L", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", "Return" },
  { "Shift_L", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "Multi_key", "Shift_R" },
  { "MainMenu", "Caps_Lock", "Alt_L", "Meta_L", "space", "Meta_R", "Alt_R",
    "Left", "Right", "Up", "Down", "Focus" },
};
static char *keys_altgr[NUM_KEY_ROWS][NUM_KEY_COLS] = { { NULL } };
static char *keys_shift_altgr[NUM_KEY_ROWS][NUM_KEY_COLS] = { { NULL } };

static char *key_labels[NUM_KEY_ROWS][NUM_KEY_COLS] = {
  { "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "Backspace" },
  { "Esc", "!\n1", "@\n2", "#\n3", "$\n4", "%\n5", "^\n6",
    "&\n7", "*\n8", "(\n9", ")\n0", "_\n-", "+\n=", "|\n\\", "~\n`" },
  { "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{\n[", "}\n]", "Del" },
  { "Control", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":\n;", "\"\n'", "Return" },
  { "Shift", "Z", "X", "C", "V", "B", "N", "M", "<\n,", ">\n.", "?\n/", "Com\npose", "Shift" },
  { "MainMenu", "Caps\nLock", "Alt", "Meta", "", "Meta", "Alt",
    "left", "right", "up", "down", "Focus" },
};
static char *normal_key_labels[NUM_KEY_ROWS][NUM_KEY_COLS] = {
  { "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "Backspace" },
  { "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "\\", "`" },
  { "Tab", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "Del" },
  { "Ctrl", "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "Return" },
  { "Shift", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "Comp", "Shift" },
  { "MainMenu", "Caps", "Alt", "Meta", "", "Meta", "Alt",
    "left", "right", "up", "down", "Focus" },
};
static char *shift_key_labels[NUM_KEY_ROWS][NUM_KEY_COLS] = {
  { "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10", "F11", "F12", "Backspace" },
  { "Esc", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "|", "~" },
  { "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "Del" },
  { "Ctrl", "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", "Return" },
  { "Shift", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "Comp", "Shift" },
  { "MainMenu", "Caps", "Alt", "Meta", "", "Meta", "Alt",
    "left", "right", "up", "down", "Focus" },
};
static char *altgr_key_labels[NUM_KEY_ROWS][NUM_KEY_COLS] = { { NULL } };
static char *shift_altgr_key_labels[NUM_KEY_ROWS][NUM_KEY_COLS] = { { NULL } };


#define NUM_KEYPAD_ROWS  5
#define NUM_KEYPAD_COLS  5

static char *keypad_shift[NUM_KEYPAD_ROWS][NUM_KEYPAD_COLS] = {
  { "Num_Lock",  "KP_Divide",   "KP_Multiply", "Focus"       },
  { "KP_7",      "KP_8",        "KP_9",        "KP_Add"      },
  { "KP_4",      "KP_5",        "KP_6",        "KP_Subtract" },
  { "KP_1",      "KP_2",        "KP_3",        "KP_Enter"    },
  { "KP_0",                     "."                          },
};
static char *keypad_label[NUM_KEYPAD_ROWS][NUM_KEYPAD_COLS] = {
  { "Num\nLock", "/",           "*",           "Focus"       },
  { "7\nHome",   "8\nUp  ",     "9\nPgUp",     "+"           },
  { "4\nLeft",   "5\n    ",     "6\nRight",    "-"           },
  { "1\nEnd ",   "2\nDown",     "3\nPgDn",     "Enter"       },
  { "0\nIns ",                  ".\nDel "                    },
};

#define NUM_SUN_FKEY_ROWS 6
#define NUM_SUN_FKEY_COLS 3

static char *sun_fkey[NUM_SUN_FKEY_ROWS][NUM_SUN_FKEY_COLS] = {
  { "L1", "L2"  },
  { "L3", "L4"  },
  { "L5", "L6"  },
  { "L7", "L8"  },
  { "L9", "L10" },
  { "Help"      },
};
static char *sun_fkey_label[NUM_SUN_FKEY_ROWS][NUM_SUN_FKEY_COLS] = {
  { "Stop \nL1", "Again\nL2"  },
  { "Props\nL3", "Undo \nL4"  },
  { "Front\nL5", "Copy \nL6"  },
  { "Open \nL7", "Paste\nL8"  },
  { "Find \nL9", "Cut  \nL10" },
  { "Help"                    },
};

/*
 * Image for arrow keys
 */
#define up_width 7
#define up_height 13
static unsigned char up_bits[] = {
   0x08, 0x1c, 0x1c, 0x3e, 0x2a, 0x49, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08,
   0x08};

#define down_width 7
#define down_height 13
static unsigned char down_bits[] = {
   0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x49, 0x2a, 0x3e, 0x1c, 0x1c,
   0x08};

#define left_width 13
#define left_height 13
static unsigned char left_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x20, 0x00, 0x18, 0x00, 0x0e, 0x00,
   0xff, 0x1f, 0x0e, 0x00, 0x18, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00};

#define right_width 13
#define right_height 13
static unsigned char right_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x03, 0x00, 0x0e,
   0xff, 0x1f, 0x00, 0x0e, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00};

#define check_width 16
#define check_height 16
static unsigned char check_bits[] = {
  0x00, 0x00, 0x00, 0x00, 0x00, 0x70, 0x00, 0x38, 0x00, 0x1e, 0x08, 0x0f,
  0x8c, 0x07, 0xde, 0x03, 0xfe, 0x03, 0xfc, 0x01, 0xf8, 0x00, 0xf0, 0x00,
  0x70, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00};

#define back_width 18
#define back_height 13
static unsigned char back_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00,
   0x78, 0x00, 0x00, 0xfe, 0xff, 0x03, 0xff, 0xff, 0x03, 0xfe, 0xff, 0x03,
   0x78, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00};

/*
 * Resources and options
 */
#define Offset(entry) XtOffset(struct appres_struct *, entry)
#undef Offset

static XrmOptionDescRec options[] = {
  { "-geometry", ".windowGeometry", XrmoptionSepArg, NULL },
  { "-windowgeometry", ".windowGeometry", XrmoptionSepArg, NULL },
  { "-debug", ".debug", XrmoptionNoArg, "True" },
#ifdef USE_XTEST
  { "-xtest", ".xtest", XrmoptionNoArg, "True" },
  { "-xsendevent", ".xtest", XrmoptionNoArg, "False" },
  { "-no-jump-pointer", ".jumpPointer", XrmoptionNoArg, "False" },
  { "-no-back-pointer", ".jumpPointerBack", XrmoptionNoArg, "False" },
#endif
  { "-no-sync", ".noSync", XrmoptionNoArg, "True" },
  { "-always-on-top", ".alwaysOnTop", XrmoptionNoArg, "True" },  /* EXPERIMENTAL */
  { "-quick", ".quickModifiers", XrmoptionNoArg, "True" },
  { "-modifiers", ".positiveModifiers", XrmoptionSepArg, NULL },
  { "-text", ".text", XrmoptionSepArg, NULL },
  { "-file", ".file", XrmoptionSepArg, NULL },
  { "-window", ".window", XrmoptionSepArg, NULL },
  { "-widget", ".widget", XrmoptionSepArg, NULL },
  { "-altgr-lock", ".altgrLock", XrmoptionNoArg, "True" },
  { "-no-altgr-lock", ".altgrLock", XrmoptionNoArg, "False" },
  { "-no-repeat", ".autoRepeat", XrmoptionNoArg, "False" },
  { "-norepeat", ".autoRepeat", XrmoptionNoArg, "False" },
  { "-no-keypad", ".keypad", XrmoptionNoArg, "False" },
  { "-nokeypad", ".keypad", XrmoptionNoArg, "False" },
  { "-no-functionkey", ".functionkey", XrmoptionNoArg, "False" },
  { "-nofunctionkey", ".functionkey", XrmoptionNoArg, "False" },
  { "-highlight", ".highlightForeground", XrmoptionSepArg, NULL },
  { "-compact", ".compact", XrmoptionNoArg, "True" },
  { "-keypad", ".keypadOnly", XrmoptionNoArg, "True" },
  { "-true-keypad", ".keypadKeysym", XrmoptionNoArg, "True" },
  { "-truekeypad", ".keypadKeysym", XrmoptionNoArg, "True" },
  { "-no-add-keysym", ".autoAddKeysym", XrmoptionNoArg, "False" },
  { "-altgr-keycode", ".altgrKeycode", XrmoptionSepArg, NULL },
  { "-list", ".listWidgets", XrmoptionNoArg, "True" },
  { "-modal", ".modalKeytop", XrmoptionNoArg, "True" },
  { "-minimizable", ".minimizable", XrmoptionNoArg, "True" },
  { "-secure", ".secure", XrmoptionNoArg, "True" },
  { "-nonexitable", ".nonexitable", XrmoptionNoArg, "True" },
  { "-xdm", ".Secure", XrmoptionNoArg, "True" },
  { "-dict", ".dictFile", XrmoptionSepArg, NULL },
  { "-keyfile", ".keyFile", XrmoptionSepArg, NULL },
  { "-customizations", ".customizations", XrmoptionSepArg, NULL },
};

/*
 * Global variables
 */
static int argc1;
static char **argv1;
static Widget toplevel;
static Display *dpy;


static KeySym *keysym_table = NULL;
static int min_keycode, max_keycode;
static int keysym_per_keycode;
//static Boolean error_detected;

static int alt_mask = 0;
static int meta_mask = 0;
static int altgr_mask = 0;
static KeySym altgr_keysym = NoSymbol;

static int shift_state = 0;
static int mouse_shift = 0;

static Display *target_dpy = NULL;

static Window toplevel_parent = None;
static Window focused_window = None;
static Window focused_subwindow = None;

static Pixmap xvkbd_pixmap = None;

//static int AddKeysym(KeySym keysym, Boolean top);  /* forward */
//static void SendString(const unsigned char *str);
//static void MakeKeyboard(Boolean remake);
//static void MakeKeypad(Widget form, Widget from_vert, Widget from_horiz);
//static void MakeSunFunctionKey(Widget form, Widget from_vert, Widget from_horiz);
//static void MakeDeadkeyPanel(Widget form);
//static void RefreshMainMenu(void);
//static void PopupFunctionKeyEditor(void);
//static void DeleteWindowProc(Widget w, XEvent *event, String *pars, Cardinal *n_pars);
