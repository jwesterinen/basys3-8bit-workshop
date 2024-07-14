/*
    main.h
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
char *GetString(char *buffer);
void PutString(char *string);
uint8_t MemRead(uint16_t addr);
void MemWrite(uint16_t addr, uint8_t data);
void InitDisplay(void);


