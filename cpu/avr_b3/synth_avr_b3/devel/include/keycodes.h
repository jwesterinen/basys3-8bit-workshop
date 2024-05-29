
// getps2() : Get available characters from the
// PS2 keyboard.  A value of zero indicates that
// no key presses were available.
//
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
uint16_t getps2(void);
void getkey(void);



#define KEY_MIN    		200
#define KEY_NUMLOCK		200
#define KEY_SCROLLLOCK		201
#define KEY_HOME		202
#define KEY_END			203
#define KEY_UP			204
#define KEY_LEFT		205
#define KEY_RIGHT		206
#define KEY_DOWN		207
#define KEY_PAGEUP		208
#define KEY_PAGEDOWN		209
#define KEY_INSERT		210
#define KEY_DELETE		211
#define KEY_F1			212
#define KEY_F2			213
#define KEY_F3			214
#define KEY_F4			215
#define KEY_F5			216
#define KEY_F6			217
#define KEY_F7			218
#define KEY_F8			219
#define KEY_F9			220
#define KEY_F10			221
#define KEY_F11			222
#define KEY_F12			223
#define KEY_WINL                224
#define KEY_WINR                225
#define KEY_WINMENU             226
#define KEY_ENTER               227
#define KEY_MAX			227
