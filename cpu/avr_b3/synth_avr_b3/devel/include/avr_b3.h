#include <inttypes.h>
#include <avr/interrupt.h>

// I/O mapped I/O

#define __IOR(x)    (*(volatile uint8_t *)(0x20+(x)))
#define __IOW(x)    (*(volatile uint16_t *)(0x20+(x)))

#define IO_BASE_UART0       0x10

// uart.h

#define UDR0    __IOR(IO_BASE_UART0+0x00)
#define UCSRA0  __IOR(IO_BASE_UART0+0x01)
#define UCSRB0  __IOR(IO_BASE_UART0+0x02)
#define UBRR0   __IOR(IO_BASE_UART0+0x03)

// UCSRA
#define RXB8    0
#define PE		2
#define DOR		3
#define FE		4
#define UDRE    5
#define TXC		6
#define RXC		7

// UCSRB
#define TXB8    0
#define UCSZ    1
#define UPM0    2
#define UPM1    3
#define USBS    4
#define UDRIE   5
#define TXCIE   6
#define RXCIE   7	


// memory mapped I/O

#define __MMIOR(x) (*(volatile uint8_t *)(0x8000+(x)))
#define __MMIOW(x) (*(volatile uint16_t *)(0x8000+(x)))

#define MMIO_BASE_BASIC_IO  0x0000  // reg addrs 0x8000-0x80ff
#define MMIO_BASE_KEYPAD    0x0100  // reg addrs 0x8100-0x81ff
#define MMIO_BASE_SOUND     0x0200  // reg addrs 0x8200-0x82ff
#define MMIO_BASE_VGATERM   0x0300  // reg addrs 0x8300-0x83ff
#define MMIO_BASE_KEYBOARD  0x0400  // reg addrs 0x8400-0x84ff

// basic I/O
#define SW          __MMIOW(MMIO_BASE_BASIC_IO+0x00)    // all switches
#define SW_LSB      __MMIOR(MMIO_BASE_BASIC_IO+0x00)    // switch LSB
#define SW_MSB      __MMIOR(MMIO_BASE_BASIC_IO+0x01)    // switch MSB
#define BUTTONS     __MMIOR(MMIO_BASE_BASIC_IO+0x02)    // buttons
#define LED         __MMIOW(MMIO_BASE_BASIC_IO+0x04)    // all LEDs
#define LED_LSB     __MMIOR(MMIO_BASE_BASIC_IO+0x04)    // LED LSB
#define LED_MSB     __MMIOR(MMIO_BASE_BASIC_IO+0x05)    // LED MSB
#define DP          __MMIOR(MMIO_BASE_BASIC_IO+0x06)    // decimal points
#define DISPCTRL    __MMIOR(MMIO_BASE_BASIC_IO+0x0b)    // display control, pattern mode = 0, raw mode = !0
#define DISPLAY0    __MMIOR(MMIO_BASE_BASIC_IO+0x0c)    // 7-segment display0
#define DISPLAY1    __MMIOR(MMIO_BASE_BASIC_IO+0x0d)    // 7-segment display1
#define DISPLAY2    __MMIOR(MMIO_BASE_BASIC_IO+0x0e)    // 7-segment display2
#define DISPLAY3    __MMIOR(MMIO_BASE_BASIC_IO+0x0f)    // 7-segment display3

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

// decimal points
#define DP_NONE 0
#define DP0     0
#define DP1     1
#define DP2     2
#define DP3     3

// button codes
#define BUTTON_NONE 0x00
#define BUTTON_C    0x01
#define BUTTON_U    0x02 
#define BUTTON_L    0x04 
#define BUTTON_R    0x08 
#define BUTTON_D    0x10

// keypad
#define KEYPAD  __MMIOR(MMIO_BASE_KEYPAD+0x00)   // key code

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

// sound generator
#define VCO1_FREQ   __MMIOW(MMIO_BASE_SOUND+0x00)   // VCO 1 freq
#define VCO2_FREQ   __MMIOW(MMIO_BASE_SOUND+0x02)   // VCO 2 freq
#define NOISE_FREQ  __MMIOW(MMIO_BASE_SOUND+0x04)   // NOISE freq
#define LFO_FREQ    __MMIOW(MMIO_BASE_SOUND+0x06)   // LFO freq
#define MOD_DEPTH   __MMIOR(MMIO_BASE_SOUND+0x08)   // modulation depth
#define MOD_SEL     __MMIOR(MMIO_BASE_SOUND+0x09)   // modulation select {noise, VCO2, VCO1}
#define MIXER       __MMIOR(MMIO_BASE_SOUND+0x0a)   // mixer select {LFO, noise, VCO2, VCO1}

// sound generator oscillators
#define VCO1    0x01
#define VCO2    0x02
#define NOISE   0x04
#define LFO     0x08

// VGA terminal
#define VGA_CHAR        __MMIOR(MMIO_BASE_VGATERM+0x00) // Character FIFO on write, char under cursor on read
#define VGA_CUR_COL     __MMIOR(MMIO_BASE_VGATERM+0x01) // Set cursor col location on write, get location on read
#define VGA_CUR_ROW     __MMIOR(MMIO_BASE_VGATERM+0x02) // Set cursor row location on write, get location on read
#define VGA_ROW_OFFSET  __MMIOR(MMIO_BASE_VGATERM+0x03) // Display row offset.  Display this row after vsync
#define VGA_CUR_STYLE   __MMIOR(MMIO_BASE_VGATERM+0x04) // Cursor style. Bit0=underline/block, Bit1=invisible/visible
#define VGA_FG_COLOR    __MMIOR(MMIO_BASE_VGATERM+0x05) // Foreground color applied to all subsequent characters rgb 222
#define VGA_BG_COLOR    __MMIOR(MMIO_BASE_VGATERM+0x06) // Background color applied to all subsequent characters rgb 222
#define VGA_ATTR        __MMIOR(MMIO_BASE_VGATERM+0x07) // Attributes. Bit0=underline, Bit1=blink

// VGA constants
#define VGA_CUR_UNDERLINE   0x00
#define VGA_CUR_BLOCK       0x01
#define VGA_CUR_INVISIBLE   0x00
#define VGA_CUR_VISIBLE     0x02
#define VGA_ATTR_UNDERLINE  0x01
#define VGA_ATTR_BLINK      0x02
#define VGA_ROW_MIN         0
#define VGA_ROW_MAX         39
#define VGA_COL_MIN         0
#define VGA_COL_MAX         79

// PS2 keyboard
#define KEYBOARD    __MMIOR(MMIO_BASE_KEYBOARD+0x00)    // key code

