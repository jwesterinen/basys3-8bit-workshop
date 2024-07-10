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
// This software may be covered by US patent #10,324,889. Rights
// to use these patents is included in the license agreements.
// See LICENSE.txt for more information.
// *********************************************************

//////////////////////////////////////////////////////////////////////////
//
//  File: vgaterm.v;   A terminal emulator with VGA output.
//
//  Addr  Register
//     0  Character FIFO on write, char under cursor on read
//     1  Set cursor col location on write, get location on read
//     2  Set cursor row location on write, get location on read
//     3  Display row offset.  Display this row after vsync
//     4  Cursor style. Bit0=block/underline, Bit1=invisible/visible
//     5  Foreground color applied to all subsequent characters rgb 222
//     6  Background color applied to all subsequent characters rgb 222
//     7  Attributes. Bit0=underline, Bit1=blink
//
//  Design Notes:
//  - The character RAM and font ROM each add one pixel clock
//    delay.  The signals vstart and hstart are 3 and 2 clock
//    cycles in advance of the start of VGA data.  This gives
//    the circuit a chance to get the character and font row
//    in time for the display.
//  - Literal constants are used throughout.  79 is one less
//    than the line length.  39 is one less than the row length.
//  - There is a bug in that the cursor is not clock adjusted
//    for the delay in the font ROM.  This could be fixed by
//    not using the font ROM and generating the cursor pixels
//    as they are needed.
//  - Rowoff allows for scrolling the text.  This needs help
//    from the host driver.  The Carriage Return and Line
//    Feed characters do not affect the cursor as you might
//    expect.  This could be fixed in the Verilog or on the
//    host.
/////////////////////////////////////////////////////////////////////////


module vgaterm(CLK_I,WE_I,TGA_I,STB_I,ADR_I,STALL_O,ACK_O,DAT_I,DAT_O,clocks,
                vgaconn);
    input  CLK_I;            // system clock
    input  WE_I;             // direction of this transfer. Read=0; Write=1
    input  TGA_I;            // ==1 if reg access, ==0 if poll
    input  STB_I;            // ==1 if this peri is being addressed
    input  [7:0] ADR_I;      // address of target register
    output STALL_O;          // ==1 if we need more clk cycles to complete
    output ACK_O;            // ==1 if we claim the above address
    input  [7:0] DAT_I;      // Data INto the peripheral;
    output [7:0] DAT_O;      // Data OUTput from the peripheral, = DAT_I if not us.
    input  [`MXCLK:0] clocks; // Array of clock pulses from 10ns to 1 second
    output [7:0] vgaconn;    // pins out to the VGA connector

    wire m100clk =  clocks[`M100CLK];    // utility 100.0 millisecond pulse
    wire m10clk  =  clocks[`M10CLK];     // utility 10.00 millisecond pulse
    wire m1clk   =  clocks[`M1CLK];      // utility 1.000 millisecond pulse
    wire u100clk =  clocks[`U100CLK];    // utility 100.0 microsecond pulse
    wire u10clk  =  clocks[`U10CLK];     // utility 10.00 microsecond pulse
    wire u1clk   =  clocks[`U1CLK];      // utility 1.000 microsecond pulse
    wire n100clk =  clocks[`N100CLK];    // utility 100.0 nanosecond pulse
    wire n10clk  =  clocks[`N10CLK];     // utility 100.0 nanosecond pulse

    // data from the host
    reg    underline;        // Underline characters
    reg    blink;            // blink characters
    reg    curvisible;       // Cursor visible if set
    reg    curblock;         // Cursor block if set, underline if not
    reg    [5:0] fg;         // Foreground color as 2/2/2 of r/g/b
    reg    [5:0] bg;         // Background color as 2/2/2 of r/g/b

    // wires to/from the vga display
    wire   vsync;            // vertical sync output
    wire   hsync;            // horizontal sync output
    wire   vstart;           // pulse three clocks before start of frame
    wire   hstart;           // pulse two clocks before start of line
    wire   enabled;          // video blanking signal
    wire   [1:0] red;        // red
    wire   [1:0] green;      // green
    wire   [1:0] blue;       // blue

    // character data
    reg    [21:0] curchar;   // char under cursor with attributes
    reg    [2:0] blinkcount; // divide 0.1 Hz signal to get blink
    reg    [5:0] rasterrow;  // Char buffer row for the VGA raster
    reg    [6:0] rastercol;  // Char buffer column for the VGA raster
    reg    [5:0] rowoff;     // Row offset to help with scrolling
    wire   [21:0] rddata;    // latched char data from display buffer (bg,fg,b,u,char)
    wire   [21:0] wrdata;    // char data written to the display buffer
    wire   [7:0] fontchar;   // char passed to the font ROM
    reg    [2:0] fcol;       // font column index (0-7)
    reg    [3:0] frow;       // font row index (0-11)
    reg    [5:0] cursorrow;  // Cursor location, where to write next char
    reg    [6:0] cursorcol;  // Cursor location
    wire   showcursor;       // true when raster is on cursor and it's visible
    wire   [7:0] bits;       // row of pixels from the font array
    wire   pixelclk;
    wire   myaddr;

    vgasyncs sync(n10clk, vsync, hsync, pixelclk, vstart, hstart, enabled);
    font437 font(pixelclk, fontchar, frow, bits);

    display_buffer charbuf(
        .wclk(CLK_I),        // write clock
        .rclk(pixelclk),     // read clock
        .we(TGA_I & myaddr & WE_I & (ADR_I[3:0] == 0)), // write strobe
        .wr(cursorrow),      // write row
        .wc(cursorcol),      // write col
        .wd(wrdata),         // write data
        .rr(rasterrow),      // read row
        .roff(rowoff),       // read row offset
        .rc(rastercol),      // read col
        .rd(rddata));        // read data
 

    // wires out to the VGA connector
    assign red = (~enabled) ? 2'h0 :
                 (rddata[9] && blinkcount[2]) ? 2'h0 :
                 (showcursor && blinkcount[2]) ? 2'h0 :
                 ((frow == 4'd10) && (rddata[8])) ? rddata[15:14] :
                 (bits[fcol]) ? rddata[15:14] : rddata[21:20];
    assign green = (~enabled) ? 2'h0 :
                 (rddata[9] && blinkcount[2]) ? 2'h0 :
                 (showcursor && blinkcount[2]) ? 2'h0 :
                 ((frow == 4'd10) && (rddata[8])) ? rddata[13:12] :
                 (bits[fcol]) ? rddata[13:12] : rddata[19:18];
    assign blue = (~enabled) ? 2'h0 :
                 (rddata[9] && blinkcount[2]) ? 2'h0 :
                 (showcursor && blinkcount[2]) ? 2'h0 :
                 ((frow == 4'd10) && (rddata[8])) ? rddata[11:10] :
                 (bits[fcol]) ? rddata[11:10] : rddata[17:16];
    assign vgaconn = {blue[0], green[0], red[0], blue[1], green[1], red[1], vsync, hsync};


    // Data to and from the display buffer is bg color (6), fg color (6), 
    // underline (1), blink (1), and character (8).
    assign wrdata = {bg[5:0], fg[5:0], blink, underline, DAT_I};

    // Char to the font is the RAM output or the cursor depending on whether
    // the cursor is visible and the type of cursor
    assign showcursor = (curvisible) && (rasterrow == cursorrow) && (rastercol == cursorcol);
    assign fontchar = ((showcursor) && (curblock)) ? 8'hdb :  // block cursor
                      ((showcursor) && (~curblock)) ? 8'h5f : // underline cursor
                      rddata[7:0];

    initial
    begin
        bg = 6'h0;
        fg = 6'h3f;
        underline = 0;
        blink = 0;
        curvisible = 1;
        curblock = 1;
        cursorrow = 0;
        cursorcol = 0;
        rasterrow = 0;
        rastercol = 0;
    end

    always @(posedge pixelclk)
    begin
        if (vstart)
        begin                  // reset all counters to top left of screen
            frow <= 0;
            rasterrow <= 0;
        end
        else if (hstart)
        begin                  // increment row for font and char buffer if needed
            fcol <= 2;         // two clock offset 
            rastercol <= 7'h7f;
            if (frow == 11)
            begin
                frow <= 0;
                rasterrow <= (rasterrow == 39) ? 0 : rasterrow + 5'h1;
            end
            else
                frow <= frow + 3'h1;
        end
        else
        begin
            fcol <= fcol - 3'h1;
            if (fcol == 2)      // two clock offset, one from buffer, one from font
                rastercol <= rastercol + 7'h1;
            // latch char under cursor in case the host asks for it.
            if ((rasterrow == cursorrow) && (rastercol == cursorcol))
                curchar <= rddata;
        end
    end

    always @(posedge CLK_I)
    begin
        if (TGA_I & myaddr & WE_I)  // latch data on a write
        begin
            if (ADR_I[3:0] == 0)
            begin
                // increment the cursor location
                cursorcol <= (cursorcol < 79) ? cursorcol + 1 : 0;
                if (cursorcol == 79)
                    cursorrow <= (cursorrow < 39) ? cursorrow + 1 : 0;
            end
            if (ADR_I[3:0] == 1)
                cursorcol <= DAT_I[6:0];
            if (ADR_I[3:0] == 2)
                cursorrow <= DAT_I[5:0];
            if (ADR_I[3:0] == 3)
                rowoff <= DAT_I[5:0];
            if (ADR_I[3:0] == 4)
                {curvisible,curblock} <= DAT_I[1:0];  // cursor
            if (ADR_I[3:0] == 5)
                fg <= DAT_I[5:0];       // fg r:g:b as 2:2:2
            if (ADR_I[3:0] == 6)
                bg <= DAT_I[5:0];       // bg r:g:b as 2:2:2
            if (ADR_I[3:0] == 7)
                {blink, underline} <= DAT_I[1:0]; // underline & blink
        end
        if (m100clk)
            blinkcount <= blinkcount + 3'h1;
    end
 
    assign myaddr = (STB_I) && (ADR_I[7:3] == 0);

    assign DAT_O = (~myaddr) ? DAT_I : 
                   (TGA_I && (ADR_I[2:0] == 0)) ? curchar[7:0] :          // char at cursor
                   (TGA_I && (ADR_I[2:0] == 1)) ? {1'h0,cursorcol} :      // current cursor column
                   (TGA_I && (ADR_I[2:0] == 2)) ? {2'h0,cursorrow} :      // current cursor row
                   (TGA_I && (ADR_I[2:0] == 3)) ? {2'h0,rowoff} :         // row offset
                   (TGA_I && (ADR_I[2:0] == 4)) ? {6'h0,curvisible,curblock} :  // block/visible
                   (TGA_I && (ADR_I[2:0] == 5)) ? {2'h0,curchar[15:10]} : // fg 00rrggbb
                   (TGA_I && (ADR_I[2:0] == 6)) ? {2'h0,curchar[21:16]} : // bg 00rrggbb
                   (TGA_I && (ADR_I[2:0] == 7)) ? {6'h0,curchar[9:8]} :   // blink, underline
                   8'h00;

    // Loop in-to-out where appropriate
    assign STALL_O = 0;
    assign ACK_O = myaddr;

endmodule


// This module generates the horizontal and vertical sync pulses
// for a 640x480 VGA interface.  Input is a 100 MHz clock
//
//  VGA timings for 640x480 at 60 Hz taken from
//  https://tinyvga.com
// Scanline part    Pixels  Time [µs]
// Back porch         48     1.9066534260179
// Visible area      640    25.422045680238
// Front porch        16     0.63555114200596
// Sync pulse         96     3.8133068520357
// Whole line        800    31.777557100298
// 
// Frame part       Lines   Time [ms]
// Back porch         33     1.0486593843098
// Visible area      480    15.253227408143
// Front porch        10     0.31777557100298
// Sync pulse          2     0.063555114200596
// Whole frame       525    16.683217477656
//
module vgasyncs(ck100, vsync, hsync, pixelclk, vstart, hstart, enabled);
    input   ck100;     // 100 MHz input clock
    output  vsync;     // Vertical sync at 72 Hz
    output  hsync;     // Horizontal sync at 48.077 KHz
    output  pixelclk;  // Pixel clock at 50 MHz
    output  vstart;    // pulse two clocks before start of frame
    output  hstart;    // pulse two clocks before start of line
    output  enabled;   // set during the data part of the frame

    reg   [1:0] clkdiv;   // 100 to 25 clock divider
    reg   [10:0] hcount;  // horizontal pixel counter
    reg   [9:0]  vcount;  // vertical line counter

    initial
    begin
        hcount = 0;
        vcount = 0;
        clkdiv = 0;
    end

    always @(posedge ck100)
    begin
        clkdiv <= clkdiv + 2'h1;

        if (clkdiv == 2'h3)
        begin
            if (hcount != 799)
                hcount <= hcount + 11'h1;
            else
            begin
                hcount <= 0;   // start of new line
                vcount <= (vcount == 524) ? 0 : vcount + 10'h1;
            end
        end
    end

    assign pixelclk = clkdiv[1];
    assign hsync = (hcount > 702);
    assign vsync = (vcount > 522);
    assign vstart = (vcount == 33) && (hcount == 44);
    assign hstart = (hcount == 45);
    assign enabled = (vcount > 32) && (vcount < 512) && (hcount > 47) && (hcount < 687);

endmodule


//  Module: display_buffer.v
//  This module provides a display buffer for the VGA terminal peripheral.  It is
//  implemented using a dual port RAM with synchronous read and contains 40 rows of
//  80 characters with 14 bits of per character attributes. (22 bits)
// 
 module display_buffer(wclk, rclk, we, wr, wc, wd, rr, roff, rc, rd);
    input   wclk;       // write clock
    input   rclk;       // read clock
    input   we;         // write strobe
    input   [5:0] wr;   // write row
    input   [6:0] wc;   // write col
    input   [21:0] wd;  // write data
    input   [5:0] rr;   // read row
    input   [5:0] roff; // read row offset
    input   [6:0] rc;   // read col
    output  [21:0] rd;  // read data
 
    reg [21:0] rdreg;            // RAM output register
    reg [21:0] ram[3199:0];      // 80x40
    wire [11:0] rdaddr;          // 12 bit read address
    wire [5:0]  realrrow;        // read row with offset
    wire [11:0] wraddr;          // 12 bit write address
    wire [5:0]  realwrow;        // write row with offset
    assign realrrow = ((rr + roff) < 7'd40) ? (rr + roff) : (rr + roff - 7'd40);
    assign realwrow = ((wr + roff) < 7'd40) ? (wr + roff) : (wr + roff - 7'd40);
    assign rdaddr = (realrrow << 6) + (realrrow << 4) + rc;
    assign wraddr = (realwrow << 6) + (realwrow << 4) + wc;

    integer index, row, col;   
    initial begin
        for (row = 0; row < 40; row=row+1)
        begin
            for (col = 0; col < 80; col=col+1) begin
                ram[(row * 80) + col] = 22'h00fc20; // white on black space
            end
        end
        rdreg  = 0;
    end

    always@(posedge wclk)
    begin
        if (we)
            ram[wraddr] <= wd;
    end

    always@(posedge rclk)
    begin
        rdreg <= ram[rdaddr];
    end

    assign rd = rdreg;

endmodule


