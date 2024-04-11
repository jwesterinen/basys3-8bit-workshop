#include <inttypes.h>
#include <stdbool.h>

enum VGA_CUR_DIR {CUR_UP, CUR_DOWN, CUR_LEFT, CUR_RIGHT};
typedef char VGA_DISPLAY_BUFFER[VGA_ROW_QTY][VGA_COL_QTY];

void Display(uint16_t value, uint8_t displayQty);
void msleep(uint16_t msec);
void KeyBeep(uint8_t tone, uint16_t durationMs);
uint8_t ReadKeypad(bool beep, uint8_t tone, uint16_t durationMs);
uint8_t ReadButtons(bool beep, uint8_t tone, uint16_t durationMs);
int AppendKeypadValue(int value, bool *pIsNewEntry, bool beep, uint8_t tone, uint16_t durationMs);
uint8_t VgaPrintKeypadCode(uint8_t keypadCode);
void VgaMoveCursor(enum VGA_CUR_DIR dir);
char VgaGetChar(int row, int col);
char VgaPutChar(int row, int col, char c);
void VgaFillFrameBuffer(char c);
void VgaLoadFrameBuffer(VGA_DISPLAY_BUFFER srcBuf);
void VgaFillDisplayBuffer(VGA_DISPLAY_BUFFER buffer, char c);
void VgaLoadDisplayBuffer(VGA_DISPLAY_BUFFER destBuf, VGA_DISPLAY_BUFFER srcBuf);
void VgaNewline(void);
void VgaPrintStr(char *str);

#define VGA_BLANK_CHAR ' '
#define VgaClearFrameBuffer() VgaFillFrameBuffer(VGA_BLANK_CHAR)
#define VgaClearDisplayBuffer(b) VgaFillDisplayBuffer((b), VGA_BLANK_CHAR)

