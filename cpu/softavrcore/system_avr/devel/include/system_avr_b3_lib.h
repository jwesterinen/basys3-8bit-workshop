#include <inttypes.h>
#include <stdbool.h>

void Display(uint16_t value, uint8_t displayQty);
void msleep(uint16_t msec);
void KeyBeep(uint8_t beepTone, uint16_t mSec);
uint8_t ReadKeypad(bool beep);
uint8_t ReadButtons(bool beep);
int AppendKeyValue(int value, bool *pIsNewEntry, bool beep);

