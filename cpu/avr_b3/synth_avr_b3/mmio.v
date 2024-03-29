/*
 *  mmio.v
 *
 *  Description:
 *      This module contains all of the peripherals with memory mapped IO registers.
 *
 *  Peripherals:
 *      basic IO:   reg addrs 0x8000-0x80ff
 *      keypad:     reg addrs 0x8001-0x81ff
 *      sound:      reg addrs 0x8002-0x82ff
 */
 
`ifdef SYNTHESIS
 `include "basic_io_b3.v"
 `include "keypad_b3.v"
 `include "sound_b3.v"
`endif

module mmio
 (  input    clk,           // 100 MHz clock
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
    output JA7              // Pmod-Audio, JA7 is left input (IL)
 );

    reg [7:0] out_buf;          // Latched output buffer for data_read
    
    wire [7:0] basic_io_dout;   // data from peripherals
    wire [7:0] keypad_dout;
    wire [7:0] sound_dout;

    wire clk_50MHz;
`ifdef SYNTHESIS    
    // scale the input clock to 50MHz
    prescaler #(.N(1)) ps50(clk, clk_50MHz);
`else
    // no scaling for simulator
    assign clk_50MHz = clk;
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
        
    // latch peripheral output
    always @(posedge clk) begin
        out_buf <= (basic_io_select && re)  ? basic_io_dout  :
`ifdef SYNTHESIS
                   (keypad_select && re) ? keypad_dout :
`endif                    
                   (sound_select && re)  ? sound_dout  :
                   out_buf;
    end
    assign data_read = out_buf;

endmodule
