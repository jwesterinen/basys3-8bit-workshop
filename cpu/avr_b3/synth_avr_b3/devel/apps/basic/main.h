/*
 *  main.h
 *   
 *  version:
 *      1.0     First complete Basic interpreter
 *      2.0     Added file system
 */

#include <inttypes.h>

//#define VERBOSE
#ifdef VERBOSE
#define MESSAGE(s) Message(s)
#else
#define MESSAGE(s)
#endif

extern char message[80];
void Console(const char *string);
void Message(const char *message);
void Panic(const char *message);
char *GetString(char *buffer);
void PutString(char *string);
uint8_t MemRead(uint16_t addr);
void MemWrite(uint16_t addr, uint8_t data);
void Tone(uint16_t freq, uint16_t duration);
void Delay(uint16_t duration);
void InitDisplay(void);
uint16_t Switches(void);
uint8_t Buttons(void);
void Leds(uint16_t value);
void Display7(uint16_t value, uint8_t displayQty);

// graphics
uint8_t GfxGetChar(uint8_t row, uint8_t col);
uint8_t GfxPutChar(uint8_t row, uint8_t col, uint8_t c);
void GfxClearScreen(void);
void GfxTextMode(uint8_t);
void GfxClearScreen(void);
void GfxSetFGColor(uint8_t color);
void GfxSetBGColor(uint8_t color);

// basic file system
bool SdMount(void);
bool SdUnmount(void);
bool SdList(void);
bool SdDelete(const char *filename);
bool SdLoad(const char *filename);
bool SdSave(const char *filename);

