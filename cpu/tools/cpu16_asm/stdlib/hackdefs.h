/*
 *  hackdef.h
 *
 *      Hack architecture definitions.
 *
 */

// keyboard buffer
#define KBD             24576

// key codes
#define HACK_KEY_NONE   0
#define HACK_KEY_0      16
#define HACK_KEY_1      17
#define HACK_KEY_2      18
#define HACK_KEY_3      19
#define HACK_KEY_4      20
#define HACK_KEY_5      21
#define HACK_KEY_6      22
#define HACK_KEY_7      23
#define HACK_KEY_8      24
#define HACK_KEY_9      25
#define HACK_KEY_ENTR   26
#define HACK_KEY_UP     27
#define HACK_KEY_DOWN   28
#define HACK_KEY_RIGHT  29
#define HACK_KEY_CNCL   30
#define HACK_KEY_LEFT   31

// 4 display buffers, least significant (rightmost) first
#define DISPLAY1        16384
#define DISPLAY2        16385
#define DISPLAY3        16386
#define DISPLAY4        16387

// on-board LED (0=off, 1=on)
#define LED             28672

// GPIO data pin (0=off, 1=on) and direction buffer (0=in, 1=out)
#define GPIO_DATA       28673
#define GPIO_DIR        28674

