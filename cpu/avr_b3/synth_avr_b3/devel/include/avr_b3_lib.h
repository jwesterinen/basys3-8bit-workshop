#include <inttypes.h>
#include <stdbool.h>

void Display(uint16_t value, uint8_t displayQty);
void msleep(uint16_t msec);
void KeyBeep(uint8_t tone, uint16_t durationMs);
uint8_t ReadKeypad(bool beep, uint8_t tone, uint16_t durationMs);
uint8_t ReadButtons(bool beep, uint8_t tone, uint16_t durationMs);
int AppendKeyValue(int value, bool *pIsNewEntry, bool beep, uint8_t tone, uint16_t durationMs);

