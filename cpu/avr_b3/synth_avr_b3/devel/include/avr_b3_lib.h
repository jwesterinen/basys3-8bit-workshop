#include <inttypes.h>
#include <stdbool.h>

enum VGA_CUR_DIR {CUR_UP, CUR_DOWN, CUR_LEFT, CUR_RIGHT};
typedef uint8_t VGA_DISPLAY_BUFFER[VGA_ROW_MAX+1][VGA_COL_MAX+1];
#define VGA_BLANK_CHAR 0x20

void Display(uint16_t value, uint8_t displayQty);
void msleep(uint16_t msec);
void KeyBeep(uint8_t tone, uint16_t durationMs);
uint8_t ReadKeypad(bool beep, uint8_t tone, uint16_t durationMs);
uint8_t ReadButtons(bool beep, uint8_t tone, uint16_t durationMs);
int AppendKeypadValue(int value, bool *pIsNewEntry, bool beep, uint8_t tone, uint16_t durationMs);
uint8_t PrintKeypadCode(uint8_t keypadCode);
void MoveVgaCursor(enum VGA_CUR_DIR dir);
uint8_t GetVgaChar(int col, int row);
uint8_t PutVgaChar(int col, int row, uint8_t c);
void FillVgaDisplay(uint8_t c);
void Newline(void);
void PrintStr(char *str);

#define ClearVgaDisplay() FillVgaDisplay(VGA_BLANK_CHAR)

