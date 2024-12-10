// spicmd - command line interface to write and read an SPI device.
//
// Each line of hex numbers is a packet to an SPI device.
// CS is asseted before the first byte is sent and is
// deasserted after the last byte is sent.
// The program prompts with '>>' for sending bytes, and with
// '<<' for received bytes.
// Spaces in the command are ignored. (123456 == 12 34 56)
// Send bytes are collected and sent after the terminating newline.
// Example:
// >> 12 34 56
// << ff 98 76

#include <stdio.h>
#include <stdlib.h>
#include "../../include/avr_b3.h"
#include "../../include/avr_b3_stdio.h"
#include "../../include/avr_b3_console.h"
#include "../../include/ff.h"
#include "../../include/avr_b3_diskio.h"



/*************** DEFINES, GLOBALS, FORWARD REFERENCES *****************/
#define BUFSZ  514
char c;




// UART receive ISR
ISR(_VECTOR(3))
{
    int     val = 0;           // value of high or low nible
    static int lownibble = 0;  // ==1 if on low nibble, ==0 on high nibble
    static uint8_t byt[BUFSZ]; // Bytes to send/receive to/from SPI
    static int     bytinx = 0; // index into byt
    int     i;                 // generic loop counter
    uint8_t volatile *addrstatus = (volatile uint8_t *)(0xf501);

    // load the keyboard buffer with the char received by the UART
    c = UDR0;
    sei();

    UDR0 = c;         // echo input
    if (c == ' ') { 
        return;
    }
    else if ((c >= '0') && (c <= '9'))
        val = (int) c - (int) '0';
    else if ((c >= 'a') && (c <= '9')) 
        val = (int) c - (int) 'a';
    else if (c =='\r') {
        *addrstatus = (uint8_t)0x00;  // set cs (active low)
        *addrstatus = (uint8_t)0x02;  // clear cs
        // print return packet and prompt
        printf("<<");
        for (i = 0; i < bytinx; i++)
            printf(" %02x", byt[i]);
        printf("\n\r>> ");
        lownibble = 0;  // high nibble first
        bytinx = 0;   // new line of hex
        return;
    }
    else {
        printf("\n\rIllegal character in input.  Line terminated\n\r");
        lownibble = 0;  // high nibble first
        bytinx = 0;
        return;
    }

    // Got a valid hex character.  Nibble value is in val.
    //printf(" lownibble = %d, val = %d, bytinx = %d\n\r", lownibble, val, bytinx);
    if (lownibble == 1) {
        // low nibble
        byt[bytinx] += val;
        bytinx++;
        if (bytinx == BUFSZ) {
            printf("\n\rPacket exceeds 40 bytes. Line terminated\n\r");
            bytinx = 0;
        }
        lownibble = 0;  // high nibble first
    }
    else {
        // put high nibble in byt[]
        byt[bytinx] = val << 4;
        lownibble = 1;
    }
}


int main(void)
{
    static int     i = 0; // index into byt
    TCHAR     buff[4096];        /* Working buffer */
    TCHAR    *bufptr;            // points to buff
    FIL       testfp;            /* File object */
    /* FILINFO   Finfo; */
    FATFS     diskA;
    DIR       topdir;
    FILINFO   fno;
    FRESULT   retstat;
    int       count;

    // set UART baud rate to 115200, enable UART, enable interrupts
    // Baud Rate = (Oscillator Frequency / (16( UBRR Value +1))
    //UBRR0 = 13-1;  // 12.5 MHz system clock
    //UBRR0 = 14-1;  // 12.5 MHz system clock
    UBRR0 = 54-1;  // 50 MHz system clock
    UCSRB0 |= (1<<RXCIE);
    sei();
    stdout = &mystdout;
    printf("\n\rStarting sdcard test\n\r");

    // Initialize SD card
    printf("\n\rInitializing disk\n\r");
    retstat = disk_initialize(0);
    if (retstat != FR_OK) {
        printf("disk initialization failure\n\r");
        while (1) ;
    }

    // Mount FAT filesystem
    printf("\n\rMounting FAT filesystem\n\r");
    retstat = f_mount(&diskA, "/", 0);
    if (retstat != FR_OK) {
        printf("Failed to mount FAT filesystem\n\r");
        while (1) ;
    }

    // Open root directory
    printf("\n\rOpening root directory\n\r");
    retstat = f_opendir(&topdir, "");
    if (retstat != FR_OK) {
        printf("Failed to open root directory \n\r");
        while (1) ;
    }

    // List the files and directories in the root directory
    printf("\n\rListing root directory files ...\n\r");
    retstat = f_readdir(&topdir, &fno);
    printf("Name         --   Size -- Attributes\n\r");
    while ((retstat == FR_OK) && (fno.fname[0])) {
        fno.fname[12] = 0;
        printf("%s -- %6d -- %x\n\r", fno.fname, (int)fno.fsize, fno.fattrib);
        retstat = f_readdir(&topdir, &fno);
    }

    // Delete the file testfile.bas.  Ignore errors.
    printf("\n\rUnlinking testfile.bas\n\r");
    retstat = f_unlink("testfile.bas");
    if (retstat != FR_OK)
        printf("Unlink failed with err=%d\n\r", retstat);

    // Open/create the file testfile.bas
    printf("\n\rCreating testfile.bas\n\r");
    retstat = f_open(&testfp, "testfile.bas", (FA_CREATE_ALWAYS | FA_READ | FA_WRITE));
    if (retstat != FR_OK) {
        printf("Failed to open/create testfile.bas.  Err=%d\n\r", retstat);
        while (1) ;
    }

    // Write 1000 copies of a test string to testfile.bas
    printf("\n\rWriting 1000 lines of text to testfile.bas\n\r");
    for (i = 0; i < 1000; i++) {
        count = f_puts("0123456789abckefghijklmnopqrstuvwxyz\n\r", &testfp);
        if (count < 0) {
            printf("Failed to write to testfile.bas\n\r");
            while (1) ;
        }
    }

    // Flushing data to disk
    printf("\n\rFlushing testfile.bas data to disk\n\r");
    retstat = f_sync(&testfp);
    if (retstat != FR_OK) {
        printf("Failed to sync testfile.bas.  Err=%d\n\r", retstat);
        while (1) ;
    }

    // Close file
    printf("\n\rClosing testfile.bas\n\r");
    retstat = f_close(&testfp);
    if (retstat != FR_OK) {
        printf("Failed to close testfile.bas.  Err=%d\n\r", retstat);
        while (1) ;
    }

    // Open testfile.bas
    printf("\n\rOpening testfile.bas in read-only mode\n\r");
    retstat = f_open(&testfp, "testfile.bas", FA_READ);
    if (retstat != FR_OK) {
        printf("Failed to open testfile.bas for reading, err=%d\n\r", retstat);
        while (1) ;
    }

    // Read and print out the first ten lines of testfile.bas
    printf("\n\rPrinting the first 10 lines of testfile.bas ....\n\r");
    for (i = 0; i < 10; i++) {
        if (f_eof(&testfp) != 0)
            break;
        bufptr = f_gets(buff, 4096, &testfp);
        if (bufptr == 0) {
            printf("Failed to read from testfile.bas, err=%d\n\r", f_error(&testfp));
            //while (1) ;
        }
        printf("%s", bufptr);
    }

    // Close file
    printf("\n\rClosing testfile.bas\n\r");
    retstat = f_close(&testfp);
    if (retstat != FR_OK) {
        printf("Failed to close testfile.bas.  Err=%d\n\r", retstat);
        while (1) ;
    }

    // Unmount the file system
    printf("\n\rUnmounting FAT filesystem ....\n\r");
    retstat = f_unmount("/");
    if (retstat != FR_OK)
        printf("Unmount failed with err=%d\n\r", retstat);
    else
        printf("Unmount succeeded.  It is safe to remove the disk\n\r");

    // All done
    while (1) ;
}

