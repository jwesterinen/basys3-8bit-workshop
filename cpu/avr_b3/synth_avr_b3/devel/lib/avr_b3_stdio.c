/*
*   avr_b3_stdio.c
*
*   This file contains utilities to access avr_b3 peripherals.
*
*/

#define F_CPU 50000000UL
#include <stdbool.h>
#include <util/delay.h>
#include "../include/avr_b3.h"
#include "../include/avr_b3_stdio.h"
//#include "avr_b3_ps2.c"

#define TABSIZE 4

void msleep(uint16_t msec)
{
    while (msec)
    {	
        _delay_loop_2((uint32_t)F_CPU/4000UL);
        msec--;
    }
}

void Display(uint16_t value, uint8_t displayQty)
{
    if (displayQty > 2)
    {
        DISPLAY0 = (value & 0xf000) >> 12;
        DISPLAY1 = (value & 0x0f00) >> 8;
    }
    DISPLAY2 = (value & 0x00f0) >> 4;
    DISPLAY3 = (value & 0x000f);
}

void KeyBeep(uint8_t tone, uint16_t durationMs)
{
    MIXER = VCO1;
    VCO1_FREQ = tone;
    msleep(durationMs);
    VCO1_FREQ = 0;
}

uint8_t ReadKeypad(bool beep, uint8_t tone, uint16_t durationMs)
{
    uint8_t keyCode = KEYPAD;
    
    if (keyCode)
    {
        if (beep)
            KeyBeep(tone, durationMs);
        while (KEYPAD)
        ;
    }
    
    return keyCode;
}

uint8_t ReadButtons(bool beep, uint8_t tone, uint16_t durationMs)
{
    uint8_t ButtonCode = BUTTONS;
    
    if (ButtonCode)
    {
        if (beep)
            KeyBeep(tone, durationMs);
        while (BUTTONS)
        ;
    }
    
    return ButtonCode;
}

int AppendKeypadValue(int value, bool *pIsNewEntry, bool beep, uint8_t tone, uint16_t durationMs)
{
    uint8_t keycode = ReadKeypad(beep, tone, durationMs);
    
    if (keycode)
    {
        if (*pIsNewEntry)
        {
            value = 0;
            *pIsNewEntry = false;
        }
        value = (value << 4) | (keycode & 0x000f);
    }
    
    return value;
}

uint8_t VgaPrintKeypadCode(uint8_t keypadCode)
{
    uint8_t asciiVal;
    
    // decode the keypad code, convert it to ASCII, then print it
    keypadCode &= 0x0f;
    asciiVal = keypadCode + ((keypadCode <= 9) ? 0x30 : 0x37);
    VGA_CHAR = asciiVal;
    
    return asciiVal;
}

void VgaMoveCursor(enum VGA_CUR_DIR dir)
{
    switch (dir)
    {
        // cursor up
        case CUR_UP:
            // move the cursor up
            VGA_CUR_ROW -= (VGA_CUR_ROW > VGA_ROW_MIN) ? 1 : 0;
            break;
            
        // cursor down
        case CUR_DOWN:
            // move the cursor down
            VGA_CUR_ROW += (VGA_CUR_ROW < VGA_ROW_MAX) ? 1 : 0;
            break;
            
        // cursor left
        case CUR_LEFT:
            // move the cursor left
            VGA_CUR_COL -= (VGA_CUR_COL > VGA_COL_MIN) ? 1 : 0;
            break;
            
        // cursor right
        case CUR_RIGHT:
            // move the cursor right
            VGA_CUR_COL += (VGA_CUR_COL < VGA_COL_MAX) ? 1 : 0;
            break;
    }
}

char VgaGetChar(int row, int col)
{
    if ((VGA_ROW_MIN <= row && row <= VGA_ROW_MAX) && (VGA_COL_MIN <= col && col <= VGA_COL_MAX))
    {
        VGA_CUR_ROW = row;
        VGA_CUR_COL = col;
        return VGA_CHAR;    
    }
    return 0;
}

char VgaPutChar(int row, int col, char c)
{
    if ((VGA_ROW_MIN <= row && row <= VGA_ROW_MAX) && (VGA_COL_MIN <= col && col <= VGA_COL_MAX))
    {
        VGA_CUR_ROW = row;
        VGA_CUR_COL = col;
        VGA_CHAR = c;   
        return c;    
    } 
    return 0;
}

void VgaFillFrameBuffer(char c)
{
    VGA_CUR_ROW = VGA_ROW_MIN;
    VGA_CUR_COL = VGA_COL_MIN;
    for (int i = 0; i < (VGA_ROW_MAX+1)*(VGA_COL_MAX+1); i++)
        VGA_CHAR = c;
}

void VgaLoadFrameBuffer(VGA_DISPLAY_BUFFER srcBuf)
{
    for (int row = 0; row <= VGA_ROW_MAX; row++)
        for (int col = 0; col <= VGA_COL_MAX; col++)
            VgaPutChar(VGA_ROW_MIN + row, VGA_COL_MIN + col, srcBuf[row][col]);
}

void VgaFillDisplayBuffer(VGA_DISPLAY_BUFFER buffer, char c)
{
    for (int row = 0; row <= VGA_ROW_MAX; row++)
        for (int col = 0; col <= VGA_COL_MAX; col++)
            buffer[row][col] = c;
}

void VgaLoadDisplayBuffer(VGA_DISPLAY_BUFFER destBuf, VGA_DISPLAY_BUFFER srcBuf)
{
    for (int row = 0; row <= VGA_ROW_MAX; row++)
        for (int col = 0; col <= VGA_COL_MAX; col++)
            destBuf[row][col] = srcBuf[row][col];
}

void VgaReset(void)
{
    VGA_CUR_ROW = 0;
    VGA_CUR_COL = 0;
    VGA_ROW_OFFSET = 0;
    VgaClearFrameBuffer();
}

void VgaNewline(void)
{
    // line feed
    if (VGA_CUR_ROW < VGA_ROW_MAX)
    {
        // move the cursor to the next row down
        VGA_CUR_ROW++;
    }
    else
    {
        // scroll the display up one row if the cursor is on the last row
        VGA_ROW_OFFSET = (VGA_ROW_OFFSET == 39) ? 0 : VGA_ROW_OFFSET+1;
        
        // clear the line
        for (int i = VGA_COL_MIN; i < VGA_COL_MAX; i++)
        {
            VGA_CUR_COL = i;
            VGA_CHAR = 0x20;
        }
    }
    
    // carriage return
    VGA_CUR_COL = VGA_COL_MIN;
}

void VgaPrintStr(const char *str)
{
    int i = 0;
    
    while (str[i])
    {
        switch (str[i])
        {
            case '\n':
                VgaNewline();
                i++;
                break;
            
            case '\t':
                for (int j = 0; j < TABSIZE; j++)
                    VGA_CHAR = ' ';
                i++;
                break;
            
            default:
                VGA_CHAR = str[i++];
                break;
        }
    }
}


/************** PS2 keyboard routines **************/


/************** Defines and global variables   ************/
        /* Scan codes for modifier keys */
#define MOD_CAPLCK    0x58
#define MOD_SHIFTL    0x12
#define MOD_SHIFTR    0x59
#define MOD_ALTL      0x11
#define MOD_ALTR      0x11   /* in E0 character set */
#define MOD_CTRLL     0x14
#define MOD_CTRLR     0x14   /* in E0 character set */
#define EVT_UP        0xF0
#define EXTENDED      0xE0

/**************** Static storage allocation ***************/
#define   PS2BUFSZ    8
uint8_t   ps2_wridx = 0;
uint8_t   ps2_rdidx = 0;
uint16_t  ps2_keycode[PS2BUFSZ];


// Return PS2 keycode if a new key is available.
// Otherwise return zero
uint16_t getps2(void)
{
    uint16_t  key = 0;

    if (ps2_wridx == ps2_rdidx)  // no key presses if equal
        return(0);

    key = ps2_keycode[ps2_rdidx];
    ps2_rdidx = (ps2_rdidx == PS2BUFSZ -1) ? 0 : ps2_rdidx +1;

    return(key);
}


/* Scancode to ASCII conversion for normal and shifted char */
typedef struct  {
    uint8_t   scancode;
    uint8_t   shifascii;
    uint8_t   normascii;
} SCASCII;

/* one byte scan code to ASCII */
SCASCII basickeys[] = {
    { 0x01, KEY_F9, KEY_F9 },
    { 0x03, KEY_F5, KEY_F5 },
    { 0x04, KEY_F3, KEY_F3 },
    { 0x05, KEY_F1, KEY_F1 },
    { 0x06, KEY_F2, KEY_F2 },
    { 0x07, KEY_F12, KEY_F12 },
    { 0x09, KEY_F10, KEY_F10 },
    { 0x0A, KEY_F8, KEY_F8 },
    { 0x0B, KEY_F6, KEY_F6 },
    { 0x0C, KEY_F4, KEY_F4 },
    { 0x0D, '\t', '\t' }, // tab
    { 0x0E, '~', '`' },
    { 0x15, 'Q', 'q' },
    { 0x16, '!', '1' },
    { 0x1A, 'Z', 'z' },
    { 0x1B, 'S', 's' },
    { 0x1C, 'A', 'a' },
    { 0x1D, 'W', 'w' },
    { 0x1E, '@', '2' },
    { 0x21, 'C', 'c' },
    { 0x22, 'X', 'x' },
    { 0x23, 'D', 'd' },
    { 0x24, 'E', 'e' },
    { 0x25, '$', '4' },
    { 0x26, '#', '3' },
    { 0x29, ' ', ' ' },  // space
    { 0x2A, 'V', 'v' },
    { 0x2B, 'F', 'f' },
    { 0x2C, 'T', 't' },
    { 0x2D, 'R', 'r' },
    { 0x2E, '%', '5' },
    { 0x31, 'N', 'n' },
    { 0x32, 'B', 'b' },
    { 0x33, 'H', 'h' },
    { 0x34, 'G', 'g' },
    { 0x35, 'Y', 'y' },
    { 0x36, '^', '6' },
    { 0x3A, 'M', 'm' },
    { 0x3B, 'J', 'j' },
    { 0x3C, 'U', 'u' },
    { 0x3D, '&', '7' },
    { 0x3E, '*', '8' },
    { 0x41, '<', ',' },
    { 0x42, 'K', 'k' },
    { 0x43, 'I', 'i' },
    { 0x44, 'O', 'o' },
    { 0x45, ')', '0' },
    { 0x46, '(', '9' },
    { 0x49, '>', '.' },
    { 0x4A, '?', '/' },
    { 0x4B, 'L', 'l' },
    { 0x4C, ':', ';' },
    { 0x4D, 'P', 'p' },
    { 0x4E, '_', '-' },
    { 0x52, '"', '\'' },
    { 0x54, '{', '[' },
    { 0x55, '+', '=' },
    { 0x5a, '\n', '\n' },  // enter (carriage return)
    { 0x5B, '}', ']' },
    { 0x5D, '|', '\\' },
    { 0x66, 0x08, 0x08 },  // backspace
    { 0x76, 0x1b, 0x1b },  // escape
    { 0x78, KEY_F11, KEY_F11 },
    { 0x83, KEY_F7, KEY_F7 }
};
#define NBASIC (sizeof(basickeys) / sizeof(SCASCII))


/* Scancode to KEY_ conversion for E0 extended character set */
typedef struct  {
    uint8_t   scancode;
    uint8_t   keycode;
} SCKEY;

/* one byte scan code to ASCII */
SCKEY E0keys[] = {
    { 0x1F, KEY_WINL },
    { 0x27, KEY_WINR },
    { 0x2F, KEY_WINMENU },
    { 0x5A, KEY_ENTER },
    { 0x69, KEY_END },
    { 0x6B, KEY_LEFT },
    { 0x6C, KEY_HOME },
    { 0x70, KEY_INSERT },
    { 0x71, KEY_DELETE },
    { 0x72, KEY_DOWN },
    { 0x74, KEY_RIGHT },
    { 0x75, KEY_UP },
    { 0x7A, KEY_PAGEDOWN },
    { 0x7D, KEY_PAGEUP }
};
#define NE0 (sizeof(E0keys) / sizeof(SCKEY))
 

// Return the byte in the 11 bit scancode at addr.
// Ignore start, stop, and parity bits.  FIX_ME(?)
int getscancode(uint8_t volatile *addr)
{
    int temp;

    addr++;                   // skip over start bit
    temp = (int)*(addr++);    // LSB first
    temp = (*addr++ == 1) ? temp + 2 : temp;
    temp = (*addr++ == 1) ? temp + 4 : temp;
    temp = (*addr++ == 1) ? temp + 8 : temp;
    temp = (*addr++ == 1) ? temp + 16 : temp;
    temp = (*addr++ == 1) ? temp + 32 : temp;
    temp = (*addr++ == 1) ? temp + 64 : temp;
    temp = (*addr++ == 1) ? temp + 128 : temp;
    return(temp);
}

void setcapled(int capled)
{
    __attribute__((unused)) uint8_t key;

    key = PS2_START_BIT;
    PS2_START_BIT = 0;      // start
    PS2_DATA0_BIT = 1;      // ED is 11101101, reversed is 10110111
    PS2_DATA1_BIT = 0;
    PS2_DATA2_BIT = 1;
    PS2_DATA3_BIT = 1;
    PS2_DATA4_BIT = 0;
    PS2_DATA5_BIT = 1;
    PS2_DATA6_BIT = 1;
    PS2_DATA7_BIT = 1;
    PS2_PARITY_BIT = 1;     // odd parity
    PS2_STOP_BIT = 1;       // stop
    msleep(10);
    key = PS2_START_BIT;
    PS2_START_BIT = 0;      // start
    PS2_DATA0_BIT = 0;      // LEDs 00000111, reversed 11100000
    PS2_DATA1_BIT = 0;
    PS2_DATA2_BIT = capled & 0x01;
    PS2_DATA3_BIT = 0;
    PS2_DATA4_BIT = 0;
    PS2_DATA5_BIT = 0;
    PS2_DATA6_BIT = 0;
    PS2_DATA7_BIT = 0;
    PS2_PARITY_BIT = (capled == 0) ? 1 : 0;     // odd parity
    PS2_STOP_BIT = 1;       // stop
    msleep(10);
    key = PS2_START_BIT;
}


//  getkey() -- // Convert a series of scan code bytes into
//  key events.
//
//  Scan codes can have up to 8 bytes although
//  the most common codes have up to 3 bytes.
//  This code deals with the most common codes.
//
//  The returned key codes are 16 bits in lenght.
//  If the upper 8 bits are zero, the lower 8 bits
//  are an ASCII character.  Shift and shift lock
//  are handled internally.  The Ctrl and Alt set
//  bit in the upper byte of the key code.  The
//  key up event also sets a bit in the upper byte.
//  If you just want key down and ASCII then you
//  can quietly ignore key codes with set bits in
//  the upper byte.

//  State information includes:
//  - Caps lock on or off
//  - Shift left down or up
//  - Shift right down or up
//  - Alt left down or up
//  - Alt right down or up
//  - Ctrl left down or up
//  - Ctrl right down or up
//  - Last scancode byte was E0
//  - Last scancode was F0 (may be preceded by E0)

// Output is a 16 bit number with the character
// (or KEY_CODE) in the low 8 bits and with the
// modifier information in the high 8 bits.  The
// modifier information is all zeros for normal
// characters and key codes.  
// the modifier information as:
//  - bit6 : Caps lock (zero if not a KEY_CODE)
//  - bit5 : Shift left (zero if not a KEY_CODE)
//  - bit4 : Shift right (zero if not a KEY_CODE)
//  - bit3 : Alt left 
//  - bit2 : Alt right
//  - bit1 : Ctrl left
//  - bit0 : Ctrl right

void getkey(void)
{
    uint8_t  volatile *addr;
    __attribute__((unused)) uint8_t  key;
    int      i = 0;
    uint8_t  scancode[4];
    int      cidx;          // index into scancode
    int      byte_count;
    uint16_t outchar = 0;

    /* State variables */
    static uint8_t  mod_caplck = 0;
    static uint8_t  mod_shiftl = 0;
    static uint8_t  mod_shiftr = 0;
    static uint8_t  mod_altl = 0;
    static uint8_t  mod_altr = 0;
    static uint8_t  mod_ctrll = 0;
    static uint8_t  mod_ctrlr = 0;
    static uint8_t  evt_up = 0;
    static uint8_t  extended = 0;


    // return if no scancode present
    if (PS2_READY_COUNT == 0)
        return;

    // All scancode words have 11 bits. (start, 8 data, parity, stop)
    // Quietly ignore mal-formed scancodes
    if (PS2_READY_COUNT % 11 != 0) {
        key = PS2_START_BIT;     // clear data ready
        return;
    }

    // get scancodes in buffer (usually one) and process each
    // NOTE: this is an unconventional way to access the peripheral regs, 
    //       by address instead of contents-of-address, but it's a shorthand
    //       to prevent the definition of 44 register aliases
    byte_count = PS2_READY_COUNT / 11;
    addr = PS2_START_BIT_ADDR;
    for (cidx = 0; cidx < byte_count; cidx++) {
        scancode[cidx] = getscancode(addr + (cidx * 11));
    }

    // Process each scancode byte
    for (cidx = 0; cidx < byte_count; cidx++) {
        outchar = 0;
        //printf("_%02x_ %d _%1d%1d%1d%1d%1d%1d%1d%1d \n", scancode[cidx], byte_count,
        //extended, evt_up, mod_caplck, mod_shiftl,mod_shiftr, mod_ctrll, mod_altl, mod_altr);

        // Handle the modifier keys
        switch (scancode[cidx]) {
            case EVT_UP:
                evt_up = 1;
                continue;
                break;
            case EXTENDED :
                extended = 1;
                continue;
                break;
            case MOD_CAPLCK : // toggle caplock on key down event
                mod_caplck = (evt_up == 0) ? mod_caplck ^ 1 : mod_caplck;
                evt_up = 0;
                setcapled(mod_caplck);
                continue;
                break;
            case MOD_SHIFTL :
                mod_shiftl = (evt_up) ? 0 : 1;
                evt_up = 0;
                continue;
                break;
            case MOD_SHIFTR :
                mod_shiftr = (evt_up) ? 0 : 1;
                evt_up = 0;
                continue;
                break;
            case MOD_ALTL :
                mod_altl = (evt_up) ? 0 : 1;
                evt_up = 0;
                continue;
                break;
            case MOD_CTRLL :
                mod_ctrll = (evt_up) ? 0 : 1;
                evt_up = 0;
                continue;
                break;
        }

        // We have a new scancode and it is not one of the modifiers.
        // Search the basic table for a match.
        if (extended == 0) {
            for (i = 0; i < NBASIC; i++) {
                if (basickeys[i].scancode == scancode[cidx]) {
                    if(evt_up == 0) {
                        // If capslock but not shift, print caps for A-Z
                        if (mod_caplck && (basickeys[i].normascii >= 'a') &&
                            (basickeys[i].normascii <= 'z')) {
                            if (mod_shiftr || mod_shiftl)
                                outchar = basickeys[i].normascii;
                            else
                                outchar = basickeys[i].shifascii;
                        }
                        else {
                            // Handle function keys
                            if (mod_shiftr || mod_shiftl)
                                outchar = basickeys[i].shifascii;
                            else
                                outchar = basickeys[i].normascii;
                        }
                    }
                    break;
                }
            }
            if (i == NBASIC)   // unknown key
                continue;      // quietly ignored
        }
        else {
            // Search the E0 extended scan codes
            // Handle the modifier keys
            switch (scancode[cidx]) {
                case MOD_ALTR :
                    mod_altr = (evt_up) ? 0 : 1;
                    evt_up = 0;
                    continue;
                    break;
                case MOD_CTRLR :
                    mod_ctrlr = (evt_up) ? 0 : 1;
                    evt_up = 0;
                    continue;
                    break;
            }
            for (i = 0; i < NE0; i++) {
                if (E0keys[i].scancode == scancode[cidx]) {
                    if(evt_up == 0) {
                        // Handle function keys
                        if (E0keys[i].keycode >= KEY_MIN)
                            outchar =  E0keys[i].keycode;
                    }
                    break;
                }
            }
            if (scancode[cidx] != EVT_UP)
                extended = 0;
            if (i == NE0)   // unknown key
                continue;   // quietly ignored
        }
        if (evt_up) {
            evt_up = 0;
            continue;
        }

        outchar += (mod_altl) ? (1 << 11) : 0;
        outchar += (mod_altr) ? (1 << 10) : 0;
        outchar += (mod_ctrll) ? (1 << 9) : 0;
        outchar += (mod_ctrlr) ? (1 << 8) : 0;

        if (outchar >= KEY_MIN) {
            outchar += (mod_caplck) ? (1 << 14) : 0;
            outchar += (mod_shiftl) ? (1 << 13) : 0;
            outchar += (mod_shiftr) ? (1 << 12) : 0;
        }
    }

    // Save keycode to the user buffer is empty.  Discards
    // previous keycodes on buffer overrun.  Don't post
    // if outchar is zero
    if (0xff & outchar) {
        ps2_keycode[ps2_wridx] = outchar;
        ps2_wridx = (ps2_wridx == PS2BUFSZ -1) ? 0 : ps2_wridx +1;
    }

    return;
}
// end of avr_b3_stdio.c

