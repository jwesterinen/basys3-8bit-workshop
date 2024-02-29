#include <avr/cpufunc.h>
#include <avr/interrupt.h>
#include <inttypes.h>
#include <stdio.h>

#define __IOR(x)    (*(volatile uint8_t *)(0x20+(x)))
#define __IOW(x)    (*(volatile uint16_t *)(0x20+(x)))

#define IO_BASE_BASIC_IO	0x00
#define IO_BASE_SOUND_IO	0x10
#define IO_BASE_UART0       0x20
#define IO_BASE_TIMER0      0x08

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


// timer.h

#define TCNT0   __IOW(IO_BASE_TIMER0+0x00)
#define TCR0    __IOR(IO_BASE_TIMER0+0x02)
#define TSR0    __IOR(IO_BASE_TIMER0+0x03)

#define TOF     7	// timer overflow
#define TOFIE   7	// timer overflow interrupt enable
#define TPRESC0 0	// timer prescaler bit 0
#define TPRESC1 1	// timer prescaler bit 1


// basic I/O

#define SW          __IOW(IO_BASE_BASIC_IO+0x00)    // all switches
#define SW_LSB      __IOR(IO_BASE_BASIC_IO+0x00)    // switch LSB
#define SW_MSB      __IOR(IO_BASE_BASIC_IO+0x01)    // switch MSB
#define BUTTONS     __IOR(IO_BASE_BASIC_IO+0x02)    // buttons
#define KEYPAD      __IOR(IO_BASE_BASIC_IO+0x03)    // keypad
#define LED         __IOW(IO_BASE_BASIC_IO+0x04)    // all LEDs
#define LED_LSB     __IOR(IO_BASE_BASIC_IO+0x04)    // LED LSB
#define LED_MSB     __IOR(IO_BASE_BASIC_IO+0x05)    // LED MSB
#define DP          __IOW(IO_BASE_BASIC_IO+0x06)    // decimal points
#define OUT4        __IOR(IO_BASE_BASIC_IO+0x07)    // 4-bit output port data on JC[3:0]
#define DISPCTRL    __IOR(IO_BASE_BASIC_IO+0x0b)    // display control, pattern mode = 0, raw mode = !0
#define DISPLAY0    __IOR(IO_BASE_BASIC_IO+0x0c)    // 7-segment display0
#define DISPLAY1    __IOR(IO_BASE_BASIC_IO+0x0d)    // 7-segment display1
#define DISPLAY2    __IOR(IO_BASE_BASIC_IO+0x0e)    // 7-segment display2
#define DISPLAY3    __IOR(IO_BASE_BASIC_IO+0x0f)    // 7-segment display3

// button codes
#define BUTTON_NONE         0x00
#define BUTTON_C            0x01
#define BUTTON_U            0x02 
#define BUTTON_L            0x04 
#define BUTTON_R            0x08 
#define BUTTON_D            0x10

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

// decimal points
#define DP_NONE             0
#define DP0                 0
#define DP1                 1
#define DP2                 2
#define DP3                 3

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

// sound generator

#define VCO1_FREQ_LO    __IOR(IO_BASE_SOUND_IO+0x00)    // VCO1 freq (12 bits)
#define VCO1_FREQ_HI    __IOR(IO_BASE_SOUND_IO+0x01)
#define VCO2_FREQ_LO    __IOR(IO_BASE_SOUND_IO+0x02)    // VCO2 freq (12 bits)
#define VCO2_FREQ_HI    __IOR(IO_BASE_SOUND_IO+0x03)
#define NOISE_FREQ_LO   __IOR(IO_BASE_SOUND_IO+0x04)    // noise freq (12 bits)
#define NOISE_FREQ_HI   __IOR(IO_BASE_SOUND_IO+0x05)
#define LFO_FREQ_LO     __IOR(IO_BASE_SOUND_IO+0x06)    // LFO freq (10 bits)
#define LFO_FREQ_HI     __IOR(IO_BASE_SOUND_IO+0x07)
#define LFO_MOD_DEPTH   __IOR(IO_BASE_SOUND_IO+0x08)    // LFO modulation depth/shift (3 bits)
#define MOD_SEL         __IOR(IO_BASE_SOUND_IO+0x09)    // modulation select: which oscillators are modulated {Noise, VCO2, VCO1}    
#define MIXER_SEL       __IOR(IO_BASE_SOUND_IO+0x0a)    // mixer select: which oscillators are enabled {LFO, Noise, VCO2, VCO1}

// modulation selection values
#define MOD_SEL_NONE        0x0000
#define MOD_SEL_VCO1        0x0001
#define MOD_SEL_VCO2        0x0002
#define MOD_SEL_NOISE       0x0004

// mixer selection values
#define MIXER_SEL_NONE      0x0000
#define MIXER_SEL_VCO1      0x0001
#define MIXER_SEL_VCO2      0x0002
#define MIXER_SEL_NOISE     0x0004
#define MIXER_SEL_LFO       0x0008


