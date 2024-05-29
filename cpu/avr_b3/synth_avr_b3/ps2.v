// *********************************************************
// Copyright (c) 2023 Demand Peripherals, Inc.
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
// 
// *********************************************************

//////////////////////////////////////////////////////////////////////////
//
//  File: ps2.v;   PS/2 keyboard receiver / transmitter
//
//  Registers: (8 bit)
//      Reg 0:  PS/2 data bit in bit 0
//      Reg 1:  PS/2 data bit in bit 1
//      Reg 2:  PS/2 data bit in bit 2
//      Reg 3:  PS/2 data bit in bit 3
//      :::::::::::::::::::::::::::::::::::::::::
//      Reg 42: PS/2 data bit in bit 42 (if four bytes)
//      Reg 43: PS/2 data bit in bit 43
//
//      There are up to four bytes of PS/2 data.
//
//
//  HOW THIS WORKS : Receiver
//      Input data is store bit sequentially in a 1x44 bit RAM block.  Each
//  negative edge of the input clock latches the data bit and increments the
//  RAM pointer to the next bit.  Lack of a clock edge for 700 us sets the 
//  'data_ready' line and triggers an auto send of the valid data bits.
//
//  HOW THIS WORKS : Transmitter 
//      If the receiver is idle the circuit can accept a command byte from
//  the host.  This is stored in the RAM block.  When the last bit is received
//  from the host the 'xmitstate' is set to 1 and the clock line is forced to
//  zero.  At the u100 clock the state is set to 2 and the data line is forced
//  to zero.  At the next u100 clock the state is set to 3 and the clock line
//  is set to an input.
//      The keyboard sends clock pulses to ready the contents of RAM.  At 
//  RAM address equal 0x0a (the stop bit), the data line is set to be an
//  input and the xmitstate is set back to 0 (idle).
//
/////////////////////////////////////////////////////////////////////////
module ps2(CLK_I,WE_I,TGA_I,STB_I,ADR_I,STALL_O,ACK_O,DAT_I,DAT_O,IRQ_O,clocks,pins);
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
    inout  [3:0] pins;       // FPGA I/O pins

    wire u100clk  =  clocks[`U100CLK];   // utility 10.00 microsecond pulse
    wire ps2din;             // data in on line 1
    wire ps2dout;            // data out on line 2 (low or tristate)
    wire ps2cin;             // clock in on line 1
    wire ps2cout;            // clock out on line 2 (low or tristate)
    wire ps2clockedge;       // true on either edge of PS2 clock

    // PS/2 bit counter and idle timer
    reg    [5:0] bitidx;     // Bit index into RAM
    reg    [2:0] timer;      // Idle timer
    reg    data_ready;       // ==1 if a packet is ready for the host
    reg    ps2clk_1;         // bring PS/2 clock into our domain
    reg    ps2clk_2;         // bring PS/2 clock into our domain
    reg    ps2clk_3;         // bring PS/2 clock into our domain
    reg    [1:0] xmitstate;  // sequencer for sending command bytes
    `define PS2_XMITIDLE 0
    `define PS2_SETCLK   1
    `define PS2_SETDATA  2
    `define PS2_SENDDATA 3
    

    // Addressing, bus interface, and spare I/O lines and registers
    wire   myaddr;           // ==1 if a correct read/write on our address

    // Registers for Rx and Tx data
    wire   rxout;            // Rx RAM output line
    wire   [5:0] rxaddr;     // Rx RAM address lines
    wire   rxin;             // Rx RAM input lines
    wire   rxwen;            // Rx RAM write enable
    ps2ram44x1 ps2rx(rxout,rxaddr,rxin,CLK_I,wen);

    initial
    begin
        bitidx = 0;
        timer = 0;
        data_ready = 0;
        xmitstate = 0;
    end


    always @(posedge CLK_I)
    begin
        // Bring PS2 clock into our clock domain
        ps2clk_3 <= ps2clk_2;
        ps2clk_2 <= ps2clk_1;
        ps2clk_1 <= ps2cin; 

        // Handle reads and writes from the host
        if (TGA_I && myaddr)
        begin
            if (~WE_I)
            begin
                data_ready <= 0;     // Clear data ready on a read
                bitidx <= 6'h0;
                timer <= 3'h0;
            end
            // send byte if the receiver is idle (6'ha is addr of stop bit)
            else if ((ADR_I == 6'ha) && (bitidx == 0))
            begin
                xmitstate <= `PS2_SETCLK;
            end
        end

        // Do xmit state machine on u100 clocks
        else if (u100clk & (xmitstate == `PS2_SETCLK))
        begin
            xmitstate <= `PS2_SETDATA;
        end
        else if (u100clk & (xmitstate == `PS2_SETDATA))
        begin
            xmitstate <= `PS2_SENDDATA;
            bitidx <= 0;      // there's an extra clk edge when xmit
        end

        // Do all receiver processing on negative edge of PS2 clock
        else if (ps2clockedge)
        begin
            bitidx <= bitidx + 6'h1;
            timer <= 3'h0;
            // xmitstate goes idle if sending the stop bit of a command
            if ((xmitstate == `PS2_SENDDATA) && (bitidx == 6'ha))
            begin
                xmitstate <= `PS2_XMITIDLE;
            end
        end

        // Increment idle timer every 100 us, check for timeout
        else if (u100clk)
        begin
            timer <= timer + 3'h1;
            if ((timer == 3'h7) && (bitidx != 0))
                data_ready <= 1;
        end
    end


    // Detect positive and negative PS/2 clock edges
    assign ps2clockedge = (ps2clk_3 == 1) && (ps2clk_2 == 1) && (ps2clk_1 == 0);

    // Route the RAM and output lines
    assign rxaddr = (TGA_I && myaddr) ? ADR_I[5:0] : bitidx[5:0] ;
    assign rxin = (TGA_I && myaddr && WE_I && (bitidx == 0)) ? DAT_I[0] : ps2din;
    // latch data if receiving PS/2 data or if getting a command byte from the host
    assign wen  = (ps2clockedge & (xmitstate == `PS2_XMITIDLE)) | (TGA_I & myaddr & WE_I);

    // Assign the outputs.
    assign myaddr = (STB_I) && (ADR_I[7:5] == 0);
    assign DAT_O = (~myaddr) ? DAT_I :
                    (~TGA_I && myaddr && data_ready) ? {2'h0, bitidx} : // send valid bits
                    (TGA_I) ? {7'h0,rxout} :   // host read of one data bit
                    8'h00 ; 
    assign IRQ_O = data_ready;

    // Loop in-to-out where appropriate
    assign STALL_O = 0;
    assign ACK_O = myaddr;

    assign pins[0] = (xmitstate == `PS2_SETDATA) ? 1'b0 :
                     (xmitstate == `PS2_SENDDATA) ? rxout :
                     1'bz;
    assign pins[2] = ((xmitstate == `PS2_SETDATA) || (xmitstate == `PS2_SETCLK)) ? 1'b0 : 1'bz;
    assign ps2din  = pins[0];      // data in
    assign ps2dout = pins[1];      // data- (unused)
    assign ps2cin  = pins[2];      // clock in
    assign ps2cout = pins[3];      // clock- (unused)

endmodule


module ps2ram44x1(dout,addr,din,wclk,wen);
    output   dout;
    input    [5:0] addr;
    input    din;
    input    wclk;
    input    wen;

    reg      ram [43:0];

    always@(posedge wclk)
    begin
        if (wen)
            ram[addr] <= din;
    end

    assign dout = ram[addr];

endmodule

