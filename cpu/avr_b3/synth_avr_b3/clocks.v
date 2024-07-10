// *********************************************************
// Copyright (c) 2020 Demand Peripherals, Inc.
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
//  File: clocks.v;   clock generators
//  Description:  This generates the clocks and other lines used by all
//     of the peripherals. 
// 
/////////////////////////////////////////////////////////////////////////
//  
//  CLOCKS:
//  - CLK_O: the basic system clock that drives the Wishbone bus
//  - n10-s1: these are clock that can be used as the basis for longer delays
//  
//
/////////////////////////////////////////////////////////////////////////
`include "sysdefs.h"

module clocks(ck100mhz, CLK_O, clocks);
    input ck100mhz;      // input clock from the board/oscillator
    output CLK_O;        // the global system clock
    output [`MXCLK:0] clocks; // clock pulses from 10ns to 1 second in steps of 10

    reg n20div;          // 10 nanosecond divider to 20 ns
    reg [2:0] n100div;   // 100 nanosecond divider
    reg [3:0] u1div;     // 1 microsecond divider
    reg [3:0] u10div;    // 10 microsecond divider
    reg [3:0] u100div;   // 100 microsecond divider
    reg [3:0] m1div;     // millisecond divider
    reg [3:0] m10div;    // 10 millisecond divider
    reg [3:0] m100div;   // 100 millisecond divider
    reg [3:0] s1div;     // 1 second divider
    reg n100pul;         // 100 nanosecond pulse
    reg u1pul;           // 1 microsecond pulse
    reg u10pul;          // 10 microsecond pulse
    reg u100pul;         // 100 microsecond pulse
    reg m1pul;           // millisecond pulse
    reg m10pul;          // 10 millisecond pulse
    reg m100pul;         // 100 millisecond pulse
    reg s1pul;           // 1 second pulse
    reg ck50;            // the 50 MHz clock
    wire n10;            // 100 MHz clock

    initial
    begin
        n20div = 1'h0;    // 10 nanosecond divider
        n100div = 2'h0;   // 100 nanosecond divider
        u1div = 4'h0;     // 1 microsecond divider
        u10div = 4'h0;    // 10 microsecond divider
        u100div = 4'h0;   // 100 microsecond divider
        m1div = 4'h0;     // millisecond divider
        m10div = 4'h0;    // 10 millisecond divider
        m100div = 4'h0;   // 100 millisecond divider
        s1div = 4'h0;     // 1 second divider
        n100pul = 1'b0;   // 100 nanosecond pulse
        u1pul = 1'b0;     // 1 microsecond pulse
        u10pul = 1'b0;    // 10 microsecond pulse
        u100pul = 1'b0;   // 100 microsecond pulse
        m1pul = 1'b0;     // millisecond pulse
        m10pul = 1'b0;    // 10 millisecond pulse
        m100pul = 1'b0;   // 100 millisecond pulse
        s1pul = 1'b0;     // 1 second pulse
    end

    assign n10 = ck100mhz;

    always @(posedge n10)
    begin
        n20div <= ~n20div;   // divide 100 MHz to get 50 MHz
        ck50 <= n20div;      // system clock is 50 MHz
    end

    always @(posedge ck50)
    begin
        if (n100div == 0)
        begin
            n100div <= 3'h4;
            n100pul <= 1'h1;
        end
        else
        begin
            n100div <= n100div - 3'h1;
            n100pul <= 1'h0;
        end

        if (n100pul)
        begin
            if (u1div == 4'h0)
            begin
                u1div <= 4'h9;
                u1pul <= 1'h1;
            end
            else
                u1div <= u1div - 4'h1;
        end
        else
            u1pul <= 1'h0;

        if (u1pul)
        begin
            if (u10div == 4'h0)
            begin
                u10div <= 4'h9;
                u10pul <= 1'h1;
            end
            else
                u10div <= u10div - 4'h1;
        end
        else
            u10pul <= 1'h0;

        if (u10pul)
        begin
            if (u100div == 4'h0)
            begin
                u100div <= 4'h9;
                u100pul <= 1'h1;
            end
            else
                u100div <= u100div - 4'h1;
        end
        else
            u100pul <= 1'h0;

        if (u100pul)
        begin
            if (m1div == 4'h0)
            begin
                m1div <= 4'h9;
                m1pul <= 1'h1;
            end
            else
                m1div <= m1div - 4'h1;
        end
        else
            m1pul <= 1'h0;

        if (m1pul)
        begin
            if (m10div == 4'h0)
            begin
                m10div <= 4'h9;
                m10pul <= 1'h1;
            end
            else
                m10div <= m10div - 4'h1;
        end
        else
            m10pul <= 1'h0;

        if (m10pul)
        begin
            if (m100div == 4'h0)
            begin
                m100div <= 4'h9;
                m100pul <= 1'h1;
            end
            else
                m100div <= m100div - 4'h1;
        end
        else
            m100pul <= 1'h0;

        if (m100pul)
        begin
            if (s1div == 4'h0)
            begin
                s1div <= 4'h9;
                s1pul <= 1'h1;
            end
            else
                s1div <= s1div - 4'h1;
        end
        else
            s1pul <= 1'h0;
    end

    // Put the system clock on a global clock line, 20 MHz
    assign clocks[`S1CLK]   =  s1pul;   // utility 1.000 second pulse on global clock line
    assign clocks[`M100CLK] =  m100pul; // utility 100.0 millisecond pulse on global clock line
    assign clocks[`M10CLK]  =  m10pul;  // utility 10.00 millisecond pulse on global clock line
    assign clocks[`M1CLK]   =  m1pul;   // utility 1.000 millisecond pulse on global clock line
    assign clocks[`U100CLK] =  u100pul; // utility 100.0 microsecond pulse on global clock line
    assign clocks[`U10CLK]  =  u10pul;  // utility 10.00 microsecond pulse on global clock line
    assign clocks[`U1CLK]   =  u1pul;   // utility 1.000 microsecond pulse on global clock line
    assign clocks[`N100CLK] =  n100pul; // utility 100.0 nanosecond pulse on global clock line
    assign clocks[`N10CLK]  =  n10;     // utility 10.0 nanosecond pulse on global clock line
    assign CLK_O = ck50;

endmodule



