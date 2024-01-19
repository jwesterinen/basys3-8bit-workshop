// Basys3 I/O port definitions

#include <inttypes.h>

#define __IOR(x) (*(volatile uint8_t *)(0x20+(x)))
#define __IOW(x) (*(volatile uint16_t *)(0x20+(x)))

#define IO_BASE_PORTOUT0	0x00

#define SW_LSB      __IOR(IO_BASE_PORTOUT0+0x00)    // switches
#define SW_MSB      __IOR(IO_BASE_PORTOUT0+0x01)
#define BUTTONS     __IOR(IO_BASE_PORTOUT0+0x02)    // buttons
#define KEYPAD      __IOR(IO_BASE_PORTOUT0+0x03)    // keypad
#define LED_LSB     __IOR(IO_BASE_PORTOUT0+0x04)    // LEDs
#define LED_MSB     __IOR(IO_BASE_PORTOUT0+0x05)
#define DISPCTRL    __IOR(IO_BASE_PORTOUT0+0x08)    // display control, pattern mode = 0, raw mode = !0
#define DISPLAY0    __IOR(IO_BASE_PORTOUT0+0x0c)    // 7-segment displays
#define DISPLAY1    __IOR(IO_BASE_PORTOUT0+0x0d)
#define DISPLAY2    __IOR(IO_BASE_PORTOUT0+0x0e)
#define DISPLAY3    __IOR(IO_BASE_PORTOUT0+0x0f)

// button codes
#define BUTTON_NONE         0x00
#define BUTTON_C            0x01
#define BUTTON_U            0x02 
#define BUTTON_L            0x04 
#define BUTTON_R            0x08 
#define BUTTON_D            0x10

// display codes
#define DISPLAY_CHAR_0      0x00
#define DISPLAY_CHAR_1      0x01
#define DISPLAY_CHAR_2      0x02
#define DISPLAY_CHAR_3      0x03
#define DISPLAY_CHAR_4      0x04
#define DISPLAY_CHAR_5      0x05
#define DISPLAY_CHAR_6      0x06
#define DISPLAY_CHAR_7      0x07
#define DISPLAY_CHAR_8      0x08
#define DISPLAY_CHAR_9      0x09
#define DISPLAY_CHAR_A      0x0a
#define DISPLAY_CHAR_b      0x0b
#define DISPLAY_CHAR_C      0x0c
#define DISPLAY_CHAR_d      0x0d
#define DISPLAY_CHAR_E      0x0e
#define DISPLAY_CHAR_F      0x0f
#define DISPLAY_CHAR_BLANK  0x10
#define DISPLAY_CHAR_DASH   0x11
#define DISPLAY_CHAR_J      0x12
#define DISPLAY_CHAR_L      0x13
#define DISPLAY_CHAR_P      0x14
#define DISPLAY_CHAR_S      0x15
#define DISPLAY_CHAR_U      0x16
#define DISPLAY_CHAR_c      0x17
#define DISPLAY_CHAR_G1     0x18        // left & right vert segs
#define DISPLAY_CHAR_G2     0x19        // top & bottom segs
#define DISPLAY_CHAR_G4     0x1A        // inverted lower case c
#define DISPLAY_CHAR_G5     0x1B        // inverted C
#define DISPLAY_CHAR_G6     0x1C        // 3 horiz segs

// mixer selection values
#define MOD_SEL_VCO1        0x0001
#define MOD_SEL_VCO2        0x0002
#define MOD_SEL_NOISE       0x0004

// mixer selection values
#define MIXER_SEL_VCO1      0x0001
#define MIXER_SEL_VCO2      0x0002
#define MIXER_SEL_NOISE     0x0004
#define MIXER_SEL_LFO       0x0008

// key codes
#define KEY_NONE            0x00
#define KEY_0               0x10
#define KEY_1               0x11
#define KEY_2               0x12
#define KEY_3               0x13
#define KEY_4               0x14     
#define KEY_5               0x15
#define KEY_6               0x16
#define KEY_7               0x17
#define KEY_8               0x18
#define KEY_9               0x19
#define KEY_A               0x1A
#define KEY_B               0x1B
#define KEY_C               0x1C
#define KEY_D               0x1D
#define KEY_E               0x1E
#define KEY_F               0x1F


