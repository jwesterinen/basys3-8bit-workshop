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
//#include <stdlib.h>
#include "../include/ff.h"
#include "../include/avr_b3_diskio.h"
#include "../include/ffconf.h"
#include "../include/avr_b3.h"



/*************** DEFINES, GLOBALS, FORWARD REFERENCES *****************/
#define DEBUG 0
#define CD_CARDFOUND 0
#define BUFSZ  514
uint8_t buf[BUFSZ];
int     fd_dev;
uint8_t CRCTable[256];
int     diskinit = 0;
int     csstate = 1;
uint8_t sendbyt(uint8_t byt);
char c;

// Do a CCITT CRC16 calculation on one byte
void  bitcrc(uint16_t *crc, uint8_t c)
{
    int crcin[16];
    int crcout[16];
    int cbit[8];
    int x[8];
    uint16_t crctmp;

    crcin[ 0] = (*crc >>  0) & 0x0001;
    crcin[ 1] = (*crc >>  1) & 0x0001;
    crcin[ 2] = (*crc >>  2) & 0x0001;
    crcin[ 3] = (*crc >>  3) & 0x0001;
    crcin[ 4] = (*crc >>  4) & 0x0001;
    crcin[ 5] = (*crc >>  5) & 0x0001;
    crcin[ 6] = (*crc >>  6) & 0x0001;
    crcin[ 7] = (*crc >>  7) & 0x0001;
    crcin[ 8] = (*crc >>  8) & 0x0001;
    crcin[ 9] = (*crc >>  9) & 0x0001;
    crcin[10] = (*crc >> 10) & 0x0001;
    crcin[11] = (*crc >> 11) & 0x0001;
    crcin[12] = (*crc >> 12) & 0x0001;
    crcin[13] = (*crc >> 13) & 0x0001;
    crcin[14] = (*crc >> 14) & 0x0001;
    crcin[15] = (*crc >> 15) & 0x0001;

    cbit[0] = (c >> 0) & 0x01;
    cbit[1] = (c >> 1) & 0x01;
    cbit[2] = (c >> 2) & 0x01;
    cbit[3] = (c >> 3) & 0x01;
    cbit[4] = (c >> 4) & 0x01;
    cbit[5] = (c >> 5) & 0x01;
    cbit[6] = (c >> 6) & 0x01;
    cbit[7] = (c >> 7) & 0x01;

    //  x = (crc >> 8) ^ c;
    //  x ^= x >> 4;
    x[0] = (crcin[ 8] ^ cbit[0]) ^ (crcin[12] ^ cbit[4]);
    x[1] = (crcin[ 9] ^ cbit[1]) ^ (crcin[13] ^ cbit[5]);
    x[2] = (crcin[10] ^ cbit[2]) ^ (crcin[14] ^ cbit[6]);
    x[3] = (crcin[11] ^ cbit[3]) ^ (crcin[15] ^ cbit[7]);
    x[4] = crcin[12] ^ cbit[4];
    x[5] = crcin[13] ^ cbit[5];
    x[6] = crcin[14] ^ cbit[6];
    x[7] = crcin[15] ^ cbit[7];

    //     crc = (crc << 8) ^ ((uint16_t)x << 12) ^ ((uint16_t)x << 5) ^ ((uint16_t)x);
    crcout[ 0] = (     0  ) ^ (         0       ) ^ (          0     ) ^ (    x[0]   );
    crcout[ 1] = (     0  ) ^ (         0       ) ^ (          0     ) ^ (    x[1]   );
    crcout[ 2] = (     0  ) ^ (         0       ) ^ (          0     ) ^ (    x[2]   );
    crcout[ 3] = (     0  ) ^ (         0       ) ^ (          0     ) ^ (    x[3]   );
    crcout[ 4] = (     0  ) ^ (         0       ) ^ (          0     ) ^ (    x[4]   );
    crcout[ 5] = (     0  ) ^ (         0       ) ^ (       x[0]     ) ^ (    x[5]   );
    crcout[ 6] = (     0  ) ^ (         0       ) ^ (       x[1]     ) ^ (    x[6]   );
    crcout[ 7] = (     0  ) ^ (         0       ) ^ (       x[2]     ) ^ (    x[7]   );
    crcout[ 8] = (crcin[0]) ^ (         0       ) ^ (       x[3]     ) ^ (      0    );
    crcout[ 9] = (crcin[1]) ^ (         0       ) ^ (       x[4]     ) ^ (      0    );
    crcout[10] = (crcin[2]) ^ (         0       ) ^ (       x[5]     ) ^ (      0    );
    crcout[11] = (crcin[3]) ^ (         0       ) ^ (       x[6]     ) ^ (      0    );
    crcout[12] = (crcin[4]) ^ (      x[0]       ) ^ (       x[7]     ) ^ (      0    );
    crcout[13] = (crcin[5]) ^ (      x[1]       ) ^ (          0     ) ^ (      0    );
    crcout[14] = (crcin[6]) ^ (      x[2]       ) ^ (          0     ) ^ (      0    );
    crcout[15] = (crcin[7]) ^ (      x[3]       ) ^ (          0     ) ^ (      0    );

    crctmp = 0;
    crctmp += crcout[ 0] <<  0;
    crctmp += crcout[ 1] <<  1;
    crctmp += crcout[ 2] <<  2;
    crctmp += crcout[ 3] <<  3;
    crctmp += crcout[ 4] <<  4;
    crctmp += crcout[ 5] <<  5;
    crctmp += crcout[ 6] <<  6;
    crctmp += crcout[ 7] <<  7;
    crctmp += crcout[ 8] <<  8;
    crctmp += crcout[ 9] <<  9;
    crctmp += crcout[10] << 10;
    crctmp += crcout[11] << 11;
    crctmp += crcout[12] << 12;
    crctmp += crcout[13] << 13;
    crctmp += crcout[14] << 14;
    crctmp += crcout[15] << 15;

    *crc = crctmp;
}


// Calculate the CRC16 for a packet
uint16_t crc16(uint8_t *pkt, int length)
{
    // name    polynomial  initial val
    // XModem        1021         0000

    uint8_t   c;
    uint8_t   x;
    uint16_t  crc = 0x0;
    uint16_t  crcbits = 0x0;

    while (length --) {
        c = *pkt++;
        x = (crc >> 8) ^ c;
        x ^= x >> 4;
        crc = (crc << 8) ^ ((uint16_t)x << 12) ^ ((uint16_t)x << 5) ^ ((uint16_t)x);
        bitcrc(&crcbits, c);
    }
    return (crc);
}

/**********************************
 * sndbyt() : Send a byte to the SD card and return
 * the returned data.
 */
uint8_t sndbyt(uint8_t byte)
{
    SD_DATA = byte;
    while (SD_STATUS == 0) {
        continue;
    }

    return(SD_DATA);
}



/**********************************
 * setcs() : Set the chip select line on the SD card.
 * 0 to clear, non-zero to set.
 */
void setcs(uint8_t cs)
{
    if (cs == 0)
        SD_STATUS = 0x00;   // active low CS
    else
        SD_STATUS = 0x01;   // raise CS to end pkt

    csstate = cs;
    return;
}

// Build a CRC7 lookup table.  copyright Luke Peterson, http://www.dlpeterson.com/
void GenerateCRCTable(void)
{
  int i, j;
  uint8_t CRCPoly = 0x89;  // the value of our CRC-7 polynomial

  // generate a table value for all 256 possible byte values
  for (i = 0; i < 256; ++i) {
    CRCTable[i] = (i & 0x80) ? i ^ CRCPoly : i;
    for (j = 1; j < 8; ++j) {
        CRCTable[i] <<= 1;
        if (CRCTable[i] & 0x80)
            CRCTable[i] ^= CRCPoly;
    }
  }
}


// adds a message byte to the current CRC-7 to get a the new CRC-7
//   copyright Luke Peterson, http://www.dlpeterson.com/
uint8_t CRCAdd(uint8_t CRC, uint8_t message_byte)
{
    return CRCTable[(CRC << 1) ^ message_byte];
}

 
/**********************************
 * sdcmd() : Send a command to the SD card and return
 * the reply in the supplied buffer.
 */
void sdcmd(uint8_t cmd, int32_t arg, uint8_t *buf)
{
    uint8_t   crc;
    uint32_t  tmp;

    // Send FF and look for FF response to show card
    // is not busy.  (This should have a timeout!)
    while (sndbyt(0xff) != 0xff)
        ;

    buf[0] = cmd | 0x40;
    buf[4] = arg & 0xff;
    tmp    = arg >> 8;
    buf[3] = tmp & 0xff;
    tmp    = tmp >> 8;
    buf[2] = tmp & 0xff;
    tmp    = tmp >> 8;
    buf[1] = tmp & 0xff;
    crc = 0;
    crc = CRCAdd(crc, buf[0]);
    crc = CRCAdd(crc, buf[1]);
    crc = CRCAdd(crc, buf[2]);
    crc = CRCAdd(crc, buf[3]);
    crc = CRCAdd(crc, buf[4]);
    crc = (crc << 1) | 1;

#if DEBUG
    // Print the command to aid debugging
    printf(">>%d, %ld :  %x %x %x %x %x %x\n", cmd, arg,
            buf[0], buf[1], buf[2], buf[3], buf[4], crc);
#endif
    buf[0] = sndbyt(buf[0]);
    buf[1] = sndbyt(buf[1]);
    buf[2] = sndbyt(buf[2]);
    buf[3] = sndbyt(buf[3]);
    buf[4] = sndbyt(buf[4]);
    buf[5] = sndbyt(crc);
    buf[6] = sndbyt(0xff);
    buf[7] = sndbyt(0xff);
#if DEBUG
    printf("<< %02x %02x %02x %02x %02x %02x %02x %02x\n",
        buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
#endif
}


// R1 response
#define PARAM_ERROR(X)      X & 0b01000000
#define ADDR_ERROR(X)       X & 0b00100000
#define ERASE_SEQ_ERROR(X)  X & 0b00010000
#define CRC_ERROR(X)        X & 0b00001000
#define ILLEGAL_CMD(X)      X & 0b00000100
#define ERASE_RESET(X)      X & 0b00000010
#define IN_IDLE(X)          X & 0b00000001
// R7 response
#define CMD_VER(X)          ((X >> 4) & 0xF0)
#define VOL_ACC(X)          (X & 0x1F)
#define VOLTAGE_ACC_27_33   0b00000001
#define VOLTAGE_ACC_LOW     0b00000010
#define VOLTAGE_ACC_RES1    0b00000100
#define VOLTAGE_ACC_RES2    0b00001000
// R3 response
#define POWER_UP_STATUS(X)  X & 0x40
#define CCS_VAL(X)          X & 0x40
#define VDD_2728(X)         X & 0b10000000
#define VDD_2829(X)         X & 0b00000001
#define VDD_2930(X)         X & 0b00000010
#define VDD_3031(X)         X & 0b00000100
#define VDD_3132(X)         X & 0b00001000
#define VDD_3233(X)         X & 0b00010000
#define VDD_3334(X)         X & 0b00100000
#define VDD_3435(X)         X & 0b01000000
#define VDD_3536(X)         X & 0b10000000

// Debug routines
#if DEBUG

void SD_printR1(uint8_t res)
{
    if(res & 0b10000000)
        { printf("\tError: MSB = 1\n"); return; }
    if(res == 0)
        { printf("\tCard Ready\n"); return; }
    if(PARAM_ERROR(res))
        printf("\tParameter Error\n");
    if(ADDR_ERROR(res))
        printf("\tAddress Error\n");
    if(ERASE_SEQ_ERROR(res))
        printf("\tErase Sequence Error\n");
    if(CRC_ERROR(res))
        printf("\tCRC Error\n");
    if(ILLEGAL_CMD(res))
        printf("\tIllegal Command\n");
    if(ERASE_RESET(res))
        printf("\tErase Reset Error\n");
    if(IN_IDLE(res))
        printf("\tIn Idle State\n");
}


void SD_printR7(uint8_t *res)
{
    SD_printR1(res[0]);
    if(res[0] > 1)
        return;
    printf("\tCommand Version: %d\n", CMD_VER(res[1]));
    printf("\tVoltage Accepted: ");
    if(VOL_ACC(res[3]) == VOLTAGE_ACC_27_33)
        printf("2.7-3.6V\n");
    else if(VOL_ACC(res[3]) == VOLTAGE_ACC_LOW)
        printf("LOW VOLTAGE\n");
    else if(VOL_ACC(res[3]) == VOLTAGE_ACC_RES1)
        printf("RESERVED\n");
    else if(VOL_ACC(res[3]) == VOLTAGE_ACC_RES2)
        printf("RESERVED\n");
    else
        printf("NOT DEFINED\n");

    printf("\tEcho: %02x\n", res[4]);
}


void SD_printR3(uint8_t *res)
{
    SD_printR1(res[0]);
    if(res[0] > 1)
        return;
    printf("\tCard Power Up Status: ");
    if(POWER_UP_STATUS(res[1])) {
        printf("READY\n");
        printf("\tCCS Status: ");
        if(CCS_VAL(res[1])){ printf("1\n"); }
        else printf("0\n");
    } else {
        printf("BUSY\n");
    }
    printf("\tVDD Window: ");
    if(VDD_2728(res[3])) printf("2.7-2.8, ");
    if(VDD_2829(res[2])) printf("2.8-2.9, ");
    if(VDD_2930(res[2])) printf("2.9-3.0, ");
    if(VDD_3031(res[2])) printf("3.0-3.1, ");
    if(VDD_3132(res[2])) printf("3.1-3.2, ");
    if(VDD_3233(res[2])) printf("3.2-3.3, ");
    if(VDD_3334(res[2])) printf("3.3-3.4, ");
    if(VDD_3435(res[2])) printf("3.4-3.5, ");
    if(VDD_3536(res[2])) printf("3.5-3.6");
    printf("\n");
}
#endif


/*-----------------------------------------------------------------------*/
/* Get Drive Status, zero indicates a ready disk                      */
/*-----------------------------------------------------------------------*/
DSTATUS disk_status(
    BYTE pdrv)       /* Physical drive nmuber to identify the drive */
{
    uint8_t   cd;

    setcs(1);       // disable CS, read card detect, reenable CS
    cd = sndbyt(0xff);
    setcs(csstate);

    if (cd != CD_CARDFOUND)
        return(STA_NODISK);

    if (diskinit == 1)
        return(0);
    else
        return(STA_NOINIT);
}


/*-----------------------------------------------------------------------*/
/* Initialize a Drive                                                    */
/*-----------------------------------------------------------------------*/
DSTATUS disk_initialize(
    BYTE pdrv)               /* Physical drive number to identify the drive */
{
    int       i;
    uint8_t   cd;
    uint8_t   cmdbuf[15];
    int       newcard;       // set to 1 for a new version SD
    int       trycount;

    // Give leading clock pulses with CS high
    setcs(1);       // disable CS
    for (i = 0; i < 100; i++) {
        cd = sndbyt(0xff);
    }
    if (cd != CD_CARDFOUND)
        return(STA_NODISK);

    // generate the CRC7 table
    GenerateCRCTable();

    // Send sequence of init commands
    // Go idle
    trycount = 0;
    do {
        setcs(0);
        sdcmd(0x0, 0, cmdbuf); // cmd0, arg=0
        setcs(1);
        if (trycount++ == 10000)
            return(STA_NOINIT);
    } while (cmdbuf[7] != 0x01);
#if DEBUG
    printf("Going idle\n");
    SD_printR1(cmdbuf[7]);
#endif

    // Set interface
    trycount = 0;
    do {
        setcs(0);
        sdcmd(8, 0x01aa, cmdbuf); // cmd8, arg=1aa
        cmdbuf[8] = sndbyt(0xff);
        cmdbuf[9] = sndbyt(0xff);
        cmdbuf[10] = sndbyt(0xff);
        cmdbuf[11] = sndbyt(0xff);
        setcs(1);
        if (trycount++ == 1000)
            return(STA_NOINIT);
    } while (cmdbuf[11] != 0xaa);
#if DEBUG
    printf("Setting interface\n");
    SD_printR7(&cmdbuf[7]);
#endif

    // If new card then ACMD41 will work.
    // CMD55 return 0x05 if an old card
    newcard = 1;
    trycount = 0;
    do {
        // Go to APP mode
        setcs(0);
        sdcmd(55, 0, cmdbuf); // cmd55, arg=0
        setcs(1);
        if (cmdbuf[7] == 0x05)
            newcard = 0;

        // Get operating mode
        setcs(0);
        sdcmd(41, 0x40000000, cmdbuf); // cmd41
        setcs(1);
#if DEBUG
        printf("Going to APP mode, CMD55, newcard = %d\n", newcard);
        printf("Get op condition, ACMD41\n");
        SD_printR1(cmdbuf[7]);
#endif
        if (trycount++ == 1000)
            return(STA_NOINIT);
    } while (newcard && IN_IDLE(cmdbuf[7]));

    // Get CCS register
    setcs(0);
    sdcmd(58, 0, cmdbuf); // cmd58, arg=0
    cmdbuf[8] = sndbyt(0xff);
    setcs(1);
#if DEBUG
    printf("Get CCS register, cmd58\n");
    SD_printR3(&cmdbuf[7]);
#endif

    // At this point we have an initialized new card
    // or an old card
    if (newcard == 0) {
        trycount = 0;
        // Send CMD1 until card return 0x00 (READY)
        do {
            setcs(0);
            sdcmd(1, 0, cmdbuf); // cmd1, arg=0
            setcs(1);
#if DEBUG
            printf("Initializing old version card, CMD1\n");
            SD_printR1(cmdbuf[7]);
#endif
        if (trycount++ == 1000)
            return(STA_NOINIT);
        } while (cmdbuf[7] != 0);
    }

    // return value indicates no errors
    diskinit = 1;
    return(RES_OK);
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read(
    uint8_t   pdrv,        /* Physical drive nmuber to identify the drive */
    uint8_t  *buff,        /* Data buffer to store read data */
    LBA_t sector,    /* Start sector in LBA */
    UINT count)       /* Number of sectors to read */
{
    uint8_t  cmdbuf[15];
    int      seccount;
    int      crcdata;
    int      crcread;
    int      trycount;
    int      i;

    // Is disk ready?
    if (diskinit == 0)
        return(RES_NOTRDY);

    // Read count blocks into buff
    for (seccount = 0; seccount < count; seccount++) {
        trycount = 0;
        while (1) {
            while (1) {
                setcs(0);
                sdcmd(17, sector+seccount, cmdbuf); // cmd17, arg=0
#if DEBUG
                printf("Read block %ld %d\n", sector,seccount);
                SD_printR1(cmdbuf[7]);
#endif
                if (cmdbuf[7] == 0)   // wait for card ready
                    break;
                setcs(1);
                if (trycount++ == 10000) {
                    return(FR_DISK_ERR);
                }
            }
            // Cmd made it to the card, now watch for a reply
            do {
                cmdbuf[0] = sndbyt(0xff);
            } while (cmdbuf[0] == 0xff);
            // Start of data is prefaced with 0xfe
            if (cmdbuf[0] == 0xfe) {
                break;
            }
            setcs(1);
            printf("Failed to get FE sync on sector read\n");
        }

        // get sector
        for (i = 0; i < 512; i++) {
            buff[(seccount * 512) + i] = sndbyt(0xff);
        }
        // Next two bytes are CRC.  Get and check CRC
        crcread = (sndbyt(0xff) << 8) + sndbyt(0xff);
        setcs(1);
        crcdata = crc16(&buff[(seccount * 512)], 512);
        if (crcdata != crcread) {
            printf("Data CRC error\n");
            return(RES_ERROR);
        }
    }
    return(RES_OK);
}


/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/
DRESULT disk_write (
    BYTE pdrv,            /* Physical drive nmuber to identify the drive */
    const BYTE *buff,    /* Data to be written */
    LBA_t sector,        /* Start sector in LBA */
    UINT count)           /* Number of sectors to write */
{
    uint8_t   cmdbuf[15];
    int       block;
    int       crc;
    int       i;
    int       trycount;

    for (block = 0; block < count; block++) {
        trycount = 0;
        // send write command
        while (1) {
            setcs(0);
            sdcmd(24, block+sector, cmdbuf); // cmd24, arg=sector number
#if DEBUG
            printf("Writing block+sector %d %ld\n", block,sector);
            SD_printR1(cmdbuf[7]);
#endif
            if (cmdbuf[7] == 0) {  // wait for card ready (==0)
                break;
            }
            setcs(1);
            if (trycount++ == 10000) {
                return(FR_DISK_ERR);
            }
        }
        // Got card ready so write block
        // Start of data is prefaced with 0xfe
        (void) sndbyt(0xfe);
        for (i = 0; i < 512; i++) {
            (void) sndbyt(buff[(block * 512) + i]);
        }
        crc = crc16((uint8_t *) &buff[block * 512], 512);
        (void) sndbyt((crc >> 8) & 0xff);
        (void) sndbyt(crc & 0xff);
        setcs(1);
    }

    return RES_OK;
}


/*-----------------------------------------------------------------------*/
/* I/O Control                                                           */
/*-----------------------------------------------------------------------*/
DRESULT disk_ioctl (
    BYTE pdrv,        /* Physical drive nmuber (0..) */
    BYTE cmd,        /* Control code */
    void *buff)       /* Buffer to send/receive control data */
{
    if (diskinit == 0)
        return(RES_NOTRDY);
    else if (cmd == CTRL_SYNC)
        return(RES_OK);
    else if (cmd == GET_SECTOR_COUNT)
        return(RES_PARERR);    // not implemented
    else if (cmd == GET_SECTOR_SIZE) {
        *(WORD *)buff = (WORD) 512;
        return(RES_OK);
    }
    else if (cmd == GET_BLOCK_SIZE) {
        *(DWORD *)buff = (DWORD) 1;
        return(RES_OK);
    }
    else if (cmd == CTRL_TRIM) 
        return(RES_OK);

    // unknown command
    return(RES_PARERR);
}

// end of sdinit.c

