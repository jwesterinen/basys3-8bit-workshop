// *********************************************************
// Copyright (c) 2024 Bob Smith
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
// SOFTWARE IS  PROVIDED "AS IS," WITHOUT WARRANTIES OR
// CONDITIONS OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING
// WITHOUT LIMITATION ANY WARRANTIES OR CONDITIONS OF TITLE,
// NON-INFRINGEMENT, MERCHANTABILITY, OR FITNESS FOR A PARTICULAR
// PURPOSE.  YOU ARE SOLELY RESPONSIBLE FOR DETERMINING THE
// APPROPRIATENESS OF USING OR REDISTRIBUTING THE SOFTWARE (WHERE
// ALLOWED), AND ASSUME ANY RISKS ASSOCIATED WITH YOUR EXERCISE OF
// PERMISSIONS UNDER THIS AGREEMENT.
// 
// *********************************************************

//////////////////////////////////////////////////////////////////////////
//
//  File: sd.v;   SD card interface
//             Note that the data line is set to MISO when CS is low
//             and set to CardDetect when CS is high.  This lets the
//             host read card detect without an extra pin.
//
//  Registers: (8 bit)
//      Reg 0:  Transmit data on write, receive data on read
//      Reg 1:  Configuration on write, buffer status on read
//
//
//
/////////////////////////////////////////////////////////////////////////
module sd(CLK_I,WE_I,TGA_I,STB_I,ADR_I,STALL_O,ACK_O,DAT_I,DAT_O,IRQ_O,
           clocks, sclk, miso, mosi, cs, carddetect);
    input  CLK_I;            // system clock
    input  WE_I;             // direction of this transfer. Read=0; Write=1
    input  TGA_I;            // ==1 if reg access, ==0 if poll
    input  STB_I;            // ==1 if this peri is being addressed
    input  [7:0] ADR_I;      // address of target register
    output STALL_O;          // ==1 if we need more clk cycles to complete
    output ACK_O;            // ==1 if we claim the above address
    input  [7:0] DAT_I;      // Data INto the peripheral;
    output [7:0] DAT_O;      // Data OUTput from the peripheral, = DAT_I if not us.
    output IRQ_O;            // Peripheral is requesting service from the host
    input  [`MXCLK:0] clocks; // Array of clock pulses from 10ns to 1 second
    output sclk;             // SPI clock
    input  miso;             // SPI Master In / Slave Out
    output mosi;             // SPI Master Out / Slave In
    output cs;               // SPI chip select
    input  carddetect;       // 

// States include idle and xmit.
// falling edge of sclk to go to the xmit state.  This gives the
// xmit state a full cycle of sclk
`define SPI_IDLE      1'b0
`define SPI_XMIT      1'b1


    wire u1clk  = clocks[`U1CLK];   // utility 1.00 microsecond pulse

    // SPI data register, config register, and bit counter
    reg    [7:0] xmitdata;   // Byte to send
    reg    [7:0] recvdata;   // Received byte
    reg    xmitstate;        // ==1 if transmitting
    reg    data_ready;       // ==1 if byte is ready to send to host
    reg    [8:0] xmitcount;  // Bit index is high 3 bits
    reg    csdata;           // CS as set by the host

    // Addressing, bus interface, and spare I/O lines and registers
    wire   myaddr;           // ==1 if a correct read/write on our address
    wire   misocdin;         // MISO or CardDetect input

    initial
    begin
        xmitstate = 0;
        data_ready = 0;
        xmitcount = 0;
        csdata = 1;          // Assume active-low CS
    end


    always @(posedge CLK_I)
    begin
        // Status reads and writes from the host are asychronous to
        // the xmit state machine.
        if (TGA_I && myaddr && WE_I && (ADR_I[0] == 1'h1))
        begin
            csdata <= DAT_I[0];
        end

        // Handle data writes from the host.  Writes are sychronous to
        // the xmit state machine since both set the state register.
        if (TGA_I && myaddr && WE_I && (ADR_I[0] == 1'h0))  // data register
        begin
            xmitdata <= DAT_I;
            xmitstate <= `SPI_XMIT;
            xmitcount <= 9'h0;
        end
        // clear data_ready on a read of the data register
        else if (TGA_I && myaddr && ~WE_I && (ADR_I[0] == 1'h0))  // data register
        begin
            data_ready <= 0;
        end
        // Do xmit state machine rising edge of (50 MHz) u2clk.
        else if (xmitstate == `SPI_XMIT)
        begin
            xmitcount <= xmitcount + 9'h1;
            // shift MISO into receiver on rising edge of SCK (NOT SPI standard!)
            if (xmitcount[5:0] == 6'h1f)
                recvdata <= {recvdata[6:0], misocdin};
            if (xmitcount == 9'h1ff)  // count = 511*u2 = 1022 uS
            begin
                xmitstate <= `SPI_IDLE;
                data_ready <= 1;
            end
        end

    end


    // Assign the outputs.
    assign myaddr = (STB_I) && (ADR_I[7:1] == 0);
    assign DAT_O = (~myaddr) ? DAT_I :
                    (TGA_I && (ADR_I[0] == 0)) ? recvdata :
                    (TGA_I && (ADR_I[0] == 1)) ? {csdata,6'h0, data_ready} :
                    8'h00 ; 
    assign IRQ_O = data_ready;

    // Loop in-to-out where appropriate
    assign STALL_O = 0;
    assign ACK_O = myaddr;

    assign cs    = csdata;
    assign sclk  = ((xmitstate == `SPI_XMIT) && xmitcount[5]);
    assign mosi = (xmitcount[8:6] == 0) ? xmitdata[7] :   // MSB first
                  (xmitcount[8:6] == 1) ? xmitdata[6] :
                  (xmitcount[8:6] == 2) ? xmitdata[5] :
                  (xmitcount[8:6] == 3) ? xmitdata[4] :
                  (xmitcount[8:6] == 4) ? xmitdata[3] :
                  (xmitcount[8:6] == 5) ? xmitdata[2] :
                  (xmitcount[8:6] == 6) ? xmitdata[1] : xmitdata[0];
    assign misocdin = (cs == 0) ? miso : carddetect;

endmodule



