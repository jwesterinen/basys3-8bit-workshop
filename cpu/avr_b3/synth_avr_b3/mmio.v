/*
 *  mmio.v
 *
 *  Description:
 *      This module contains all of the peripherals with memory mapped IO registers.
 *
 *  Peripherals:
 *      basic IO:   reg addrs 0x8000-0x80ff
 *      keypad:     reg addrs 0x8100-0x81ff
 *      sound:      reg addrs 0x8200-0x82ff
 *      vgaterm:    reg addrs 0x8300-0x83ff
 */
 
`ifdef SYNTHESIS
 `include "basic_io_b3.v"
 `include "keypad_b3.v"
 `include "sound_b3.v"
 `include "sysdefs.h"
 `include "clocks.v"
 `include "vgaterm.v"
`endif

module mmio
(  
    input    clk,           // 100 MHz clock
    input    re,
    input    we,
    input    [14:0] addr,
    output   [7:0] data_read,
    input    [7:0] data_write,

    input   [15:0] sw,      // switches
    input   [4:0] btn,      // buttons
    output  [15:0] led,     // LEDs
    output  [6:0] seg,      // display segments
    output  dp,             // display decimal point
    output  [3:0] an,       // display select
`ifdef SYNTHESIS // (simulator doesn't like inout ports)
    inout [7:0] JB,         // PmodKYPD, JB[7:4] is rows, JB[3:0] is Columns
`endif 	
    output JA7,             // Pmod-Audio, JA7 is left input (IL)
    output [3:0] vgaBlue,   // VGA signals
    output [3:0] vgaGreen,
    output [3:0] vgaRed,
    output Vsync,
    output Hsync
);

    reg [7:0] out_buf;          // Latched output buffer for data_read
    
    wire [7:0] basic_io_dout;   // data from peripherals
    wire [7:0] keypad_dout;
    wire [7:0] sound_dout;
    wire [7:0] vgaterm_dout;

    wire clk_50MHz;
`ifdef SYNTHESIS    
    // scale the input clock to 50MHz
    prescaler #(.N(1)) ps50(clk, clk_50MHz);
`else
    // no scaling for simulator
    assign clk_50MHz = clk;
`endif    
    
    wire clk_12MHz;
`ifdef SYNTHESIS    
    // scale the input clock to ~12.5MHz
    prescaler #(.N(3)) ps12(clk, clk_12MHz);
`else
    // no scaling for simulator
    assign clk_12MHz = clk;
`endif    
    
    // basic I/O (switches, buttons, LEDs, and 7-segment displays
    wire basic_io_select = (addr[14:8] == 8'h00);            // addrs 0x8000-0x80ff
    wire basic_io_re = basic_io_select & re;
    wire basic_io_we = basic_io_select & we;
    basic_io_b3 basic_io
    (
        clk, 
        addr[7:0], basic_io_dout, data_write, basic_io_re, basic_io_we, 
        sw, btn, led, seg, dp, an
    );

`ifdef SYNTHESIS        // (simulator doesn't like inout ports)
    // Pmod keypad
    wire keypad_select = (addr[14:8] == 8'h01);
    wire keypad_re = keypad_select & re;
	keypad_b3 keypad
	(
	    clk, 
	    keypad_dout, keypad_re, 
	    JB[7:4], JB[3:0]
	);
`endif    

    // sound generator w/Pmod amp/speaker
    wire sound_select = (addr[14:8] == 8'h02);
    wire sound_re = sound_select & re;
    wire sound_we = sound_select & we;
    sound_b3 sound
    (
        clk_50MHz, 
        1'b0, addr[7:0], sound_dout, data_write, sound_re, sound_we, 
        JA7
    );
        
    // DP peripherals

    // DP clocks peripheral
    wire CLK_O;
    wire [`MXCLK:0] peri_clks;
    clocks clks(clk, CLK_O, peri_clks);

    // DP vgaterm
    wire vgaterm_select = (addr[14:8] == 8'h03);
    wire vgaterm_re = vgaterm_select & re;
    wire vgaterm_we = vgaterm_select & we;
    wire vgaterm_stall;
    wire vgaterm_ack;
    vgaterm vga
    (
        clk_12MHz, 
        vgaterm_we, 1'b1, vgaterm_select, addr[7:0], vgaterm_stall, vgaterm_ack, data_write, vgaterm_dout, peri_clks, 
        {vgaBlue[1], vgaGreen[1], vgaRed[1], vgaBlue[2], vgaGreen[2], vgaRed[2], Vsync, Hsync}
    );

    // latch peripheral output
    always @(posedge clk) begin
        out_buf <= (basic_io_select && re)  ? basic_io_dout :
`ifdef SYNTHESIS
                   (keypad_select && re)    ? keypad_dout   :
`endif                    
                   (sound_select && re)     ? sound_dout    :
                   (vgaterm_select && re)   ? vgaterm_dout  :
                   out_buf;
    end
    assign data_read = out_buf;

endmodule
