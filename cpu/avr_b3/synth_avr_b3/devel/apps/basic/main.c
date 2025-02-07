/*
*   This is the main module for a Basic interpreter retro computer.
*/

#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_ps2.h"
#include "../../include/avr_b3_console.h"
#include "../../include/ff.h"
#include "../../include/avr_b3_diskio.h"
#include "symtab.h"
#include "expr.h"
#include "parser.h"
#include "runtime.h"

// display buffer used for animation
VGA_DISPLAY_BUFFER dispBuf;

// default is PS2 keyboard
#define USE_CONSOLE_KB

#ifdef USE_CONSOLE_KB
    #define CR      '\r'    // newlines are returned as carriage return (CR) by terminal emulators
    #define BS      0x7f    // backspaces are returned as delete (DEL) by terminal emulators

    char keycode = 0;
    #define GetKey() keycode    
#else
    #define CR      '\n'
    #define BS      '\b'

    uint16_t keycode;
    char GetKey(void)
    {
        return (keycode = getps2());
    }    
#endif


// UART receive ISR
ISR(_VECTOR(3))
{
    // load the keyboard buffer with the char received by the UART
    keycode = UDR0;
    sei();
}

#ifndef USE_CONSOLE_KB
// PS2 receive ISR
ISR(_VECTOR(2))
{
    // PS2 HW API to get a ps2_keycode when there is a keypress
    getkey();
    sei();
}
#endif

char message[80];
char *versionStr = "v4.0";
char *promptStr = "> ";
    
extern bool ready;

// print a message to the console device
void Console(const char *string)
{
    stdout = &mystdout;
    printf(string);
}

// print out messages during runtime
void Message(const char *message)
{
    VgaPrintStr(message);
}

// print out system error messages unconditionally during runtime
void Panic(const char *message)
{
    VgaPrintStr(message);
}

void PutString(char *string)
{
    if (textMode)
    {
        VgaPrintStr(string);
    }
}

// returns the next CR-terminated string from the input device
char *GetString(char *buffer)
{
    unsigned i = 0;
        
    while (1)
    {
        if (GetKey())
        {
            //sprintf(message, "keycode = %d\r\n", keycode);
            //Console(message);
            
            // Enter - terminate the command string and return
            if (keycode == CR)
            {
                buffer[i++] = keycode;
                buffer[i] = (uint8_t)'\0';
                VgaNewline();
                keycode = 0x00;
                return buffer;
            }

            // Backspace - move the cursor back on place stopping at the prompt
            else if (keycode == BS)
            {
                if (VGA_CUR_COL > VGA_COL_MIN + strlen(promptStr))
                {          
                    VGA_CUR_COL--;
                    VGA_CHAR = ' ';
                    VGA_CUR_COL--;
                    i--;
                }
            }

            // display only printable characters
            else if (0x20 <= keycode && keycode <= 0x7f)
            {
                buffer[i++] = keycode;
                VGA_CHAR = keycode;
            }
            
            // clear the old keycode -- really only necessary for the console
            keycode = 0x00;
        }
        msleep(50);
    }
}

uint8_t MemRead(uint16_t addr)
{
    return (*(volatile uint8_t *)(addr));
}

void MemWrite(uint16_t addr, uint8_t data)
{
    (*(volatile uint8_t *)(addr)) = data;
}

void Tone(uint16_t freq, uint16_t duration)
{
    KeyBeep(freq, duration);
}

void InitDisplay(void)
{
    VgaReset();
    PutString("AVR_B3 Basic Interpreter ");
    PutString(versionStr);
    PutString("\n\n");
}

uint16_t Switches(void)
{
    return SW;
}

uint8_t Buttons(void)
{
    return BUTTONS;
}

void Leds(uint16_t value)
{
    LED = value;
}

void Display7(uint16_t value, uint8_t displayQty)
{
    Display(value, displayQty);
}

void Delay(uint16_t duration)
{
    msleep(duration);
}

uint8_t GfxPutChar(uint8_t row, uint8_t col, uint8_t c)
{
    return VgaPutChar(row, col, c);
}

uint8_t GfxGetChar(uint8_t row, uint8_t col)
{
    return VgaGetChar(row, col);
}

uint8_t GfxPutDB(uint8_t row, uint8_t col, uint8_t c)
{
    dispBuf[row][col] = c;
    return dispBuf[row][col];
}

uint8_t GfxGetDB(uint8_t row, uint8_t col)
{
    return dispBuf[row][col];
}

void GfxLoadFB(void)
{
    VgaLoadFrameBuffer(dispBuf);
}

void GfxClearScreen(void)
{
    VgaClearFrameBuffer();
}

void GfxClearDB(void)
{
    VgaClearDisplayBuffer(dispBuf);
}

void GfxTextMode(uint8_t mode)
{
    VGA_CUR_STYLE = (mode) ? VGA_CUR_VISIBLE : VGA_CUR_INVISIBLE;
}

// globals required for SD file system
FIL       fp;
FATFS     diskA;
DIR       topdir;
FRESULT   retstat;

char fileBuffer[MAX_PROGRAM_LEN][MAX_CMDLINE_LEN];

bool SdMount(void)
{
    // initialize SD card, mount FAT filesystem, and open root directory
    retstat = disk_initialize(0);
    if (retstat != FR_OK) 
    {
        sprintf(errorStr, "mount failed (error %d)\n", retstat);
        return false;
    }
    retstat = f_mount(&diskA, "/", 0);
    if (retstat != FR_OK) 
    {
        sprintf(errorStr, "mount failed (error %d)\n", retstat);
        return false;
    }
    retstat = f_opendir(&topdir, "");
    if (retstat != FR_OK) 
    {
        sprintf(errorStr, "mount failed (error %d)\n", retstat);
        return false;
    }

    return true;
}

bool SdUnmount(void)
{
    retstat = f_unmount("/");
    if (retstat != FR_OK)
    {
        sprintf(errorStr, "unmount failed (err %d)\n", retstat);
        return false;
    }
    PutString("...it is safe to remove the disk\n");

    return true;
}

bool SdList(void)
{
    FILINFO fno;
    
    retstat = f_readdir(&topdir, &fno);
    while ((retstat == FR_OK) && (fno.fname[0])) 
    {
        fno.fname[12] = 0;
        sprintf(message, "%s\n", fno.fname);
        PutString(message);
        retstat = f_readdir(&topdir, &fno);
    }
    if (retstat != FR_OK)
    {
        sprintf(errorStr, "file listing failed (err %d)\n", retstat);
        return false;
    }
    retstat = f_rewinddir(&topdir);
    if (retstat != FR_OK)
    {
        sprintf(errorStr, "file listing failed (err %d)\n", retstat);
        return false;
    }

    return true;
}

bool SdDelete(const char *filename)
{
    if (filename != NULL)
    {
        retstat = f_unlink(filename);
        if (retstat != FR_OK)
        {
            sprintf(errorStr, "file delete failed (err=%d)\n", retstat);
            return false;
        }
    }
    else
    {
        strcpy(errorStr, "missing filename");
        return false;
    }

    return true;
}

bool SdLoad(const char *filename)
{
    TCHAR *bufptr;
    int i;
    char *nextChar;
    
    if (filename != NULL)
    {
        // open the file
        retstat = f_open(&fp, filename, FA_READ);
        if (retstat != FR_OK) 
        {
            sprintf(errorStr, "load failed (err %d)\n", retstat);
            return false;
        }

        // read the file (until EOF) into the file buffer
        for (i = 0; i < MAX_PROGRAM_LEN; i++) 
        {
            if (f_eof(&fp) != 0)
            {
                // stop reading at EOF
                break;
            }
            bufptr = f_gets(fileBuffer[i], MAX_CMDLINE_LEN, &fp);
            if (bufptr == 0) 
            {
                sprintf(errorStr, "load failed (err %d)\n", f_error(&fp));
                return false;
            }
        }
        strcpy(fileBuffer[i], "");

        // close the file
        retstat = f_close(&fp);
        if (retstat != FR_OK) 
        {
            sprintf(errorStr, "load failed (err %d)\n", retstat);
            return false;
        }
        
        // process each line of the file buffer
        for (i = 0; fileBuffer[i][0] != '\0'; i++)
        {
            // remove any line endings
            for (nextChar = fileBuffer[i]; *nextChar != '\0'; nextChar++)
            {
                if (*nextChar == '\n' || *nextChar == '\r')
                {
                    *nextChar = '\0';
                    break;
                }
            }
            
            if (!ProcessCommand(fileBuffer[i]))
            {
                PutString(errorStr);
                PutString("\n");
            }
        }
    }
    else
    {
        strcpy(errorStr, "missing filename");
        return false;
    }

    return true;
}

// save the file buffer to a new file
bool SdSave(const char *filename)
{
    int i;
    
    if (filename != NULL)
    {
        // create a new file
        retstat = f_open(&fp, filename, (FA_CREATE_ALWAYS | FA_READ | FA_WRITE));
        if (retstat != FR_OK) 
        {
            sprintf(errorStr, "save failed (err %d)\n", retstat);
            return false;
        }

        // fill the file buffer with the program commands
        for (i = 0; i < programSize; i++)
        {
            strcpy(fileBuffer[i], Program[i].commandStr);
            strcat(fileBuffer[i], "\n");
        }
        strcpy(fileBuffer[i], "");
        
        // write the buffer contents to the currently open file
        for (int line = 0; fileBuffer[line][0] != '\0'; line++)
        {
            if (f_puts(fileBuffer[line], &fp) < 0) 
            {
                sprintf(errorStr, "save failed at line %d\n", line);
                return false;
            }
        }

        // flushing data to disk
        retstat = f_sync(&fp);
        if (retstat != FR_OK) 
        {
            sprintf(errorStr, "save failed (err %d)\n", retstat);
            return false;
        }

        // close the currently open file
        retstat = f_close(&fp);
        if (retstat != FR_OK) 
        {
            sprintf(errorStr, "save failed (err %d)\n", retstat);
            return false;
        }
    }
    else
    {
        strcpy(errorStr, "missing filename");
        return false;
    }

    return true;
}

int main(void)
{
    // set UART baud rate to 115200
    //UBRR0 = 13-1;
    UBRR0 = 54-1;

    // enable UART receiver interrupts
    UCSRB0 |= (1<<RXCIE);

    // enable global interrupts
    sei();

    Console("starting basic interpreter...\r\n");
    
    char command[80];
    
    InstallBuiltinFcts();
    InitDisplay();
    SdMount();
    while (1)
    {
        if (ready)
        {
            PutString("ready\n");
        }
        PutString(promptStr);        
        GetString(command);
        command[strlen(command)-1] = '\0';
        if (!ProcessCommand(command))
        {
            PutString(errorStr);
            PutString("\n");
        }
    }
}

// end of main.c

