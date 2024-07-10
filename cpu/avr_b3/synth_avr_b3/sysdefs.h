/////////////////////////////////////////////////////////////////////////
//  File: sysdefs.h     The globally visible definitions and default
//         values used in DPCore.
//
/////////////////////////////////////////////////////////////////////////

// *********************************************************
// Copyright (c) 2022 Demand Peripherals, Inc.
// 
// This file is licensed separately for private and commercial
// use.  See LICENSE.txt which should have accompanied this file
// for details.  If LICENSE.txt is not available please contact
// support@demandperipherals.com to receive a copy.
// 
// In general, you may use, modify, redistribute this code, and
// use any associated patent(s) as long as
// 1) the above copyright is included in all redistributions,
// 2) this notice is included in all source redistributions, and
// 3) this code or resulting binary is not sold as part of a
//    commercial product.  See LICENSE.txt for definitions.
// 
// DPI PROVIDES THE SOFTWARE "AS IS," WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING
// WITHOUT LIMITATION ANY WARRANTIES OR CONDITIONS OF TITLE,
// NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR
// PURPOSE.  YOU ARE SOLELY RESPONSIBLE FOR DETERMINING THE
// APPROPRIATENESS OF USING OR REDISTRIBUTING THE SOFTWARE (WHERE
// ALLOWED), AND ASSUME ANY RISKS ASSOCIATED WITH YOUR EXERCISE OF
// PERMISSIONS UNDER THIS AGREEMENT.
// *********************************************************

/////////////////////////////////////////////////////////////////////////
//
//  The protocol to the host consists of a command byte, two bytes of
//  register address, a word transfer count, and, if applicable, write
//  data.
//     The command has an operation (read, write, write-read), a word
//  length, the same/increment flag, the register/FIFO flag, and a bit
//  that is echoed back to the host.  Two bits in the command are
//  reserved for future use.
//
`define CMD_OP_FIELD      8'h0C
`define CMD_OP_READ       8'h04
`define CMD_OP_WRITE      8'h08
`define CMD_OP_WRRD       8'h30 
`define CMD_SAME_FIELD    8'h02
`define CMD_SAME_REG      8'h00
`define CMD_SUCC_REG      8'h02


/////////////////////////////////////////////////////////////////////////
//
//  Single cycle clock pulses every decade from 100ns to 1 second
`define N10CLK            0
`define N100CLK           1
`define U1CLK             2
`define U10CLK            3
`define U100CLK           4
`define M1CLK             5
`define M10CLK            6
`define M100CLK           7
`define S1CLK             8
`define MXCLK             8


/////////////////////////////////////////////////////////////////////////
//
//  SPI states and configuration definitions.
`define IDLE         3'h0
`define GETBYTE      3'h1
`define LOWBYTE      3'h2
`define SNDBYTE      3'h3
`define SNDRPLY      3'h4
`define CS_MODE_AL   2'h0   // Active low chip select
`define CS_MODE_AH   2'h1   // Active high chip select
`define CS_MODE_FL   2'h2   // Forced low chip select
`define CS_MODE_FH   2'h3   // Forced high chip select
`define CLK_2M       2'h0   // 2 MHz
`define CLK_1M       2'h1   // 1 MHz
`define CLK_500K     2'h2   // 500 KHz
`define CLK_100K     2'h3   // 100 KHz


/////////////////////////////////////////////////////////////////////////
//
//  Baud rates for hostserial.  May be set in board Makefile
`define BAUD460800   2'b00
`define BAUD230400   2'b01
`define BAUD115200   2'b11


// Force error when implicit net has no type.
//`default_nettype none
