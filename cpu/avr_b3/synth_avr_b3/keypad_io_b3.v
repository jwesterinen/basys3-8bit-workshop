/*
 *  keypad_io16.v
 *
 *  Description:
 *      This module contains an 8-bit interface to Digilent's PmodKYPD keypad controller.
 *
 *  The local memory map for this module is as follows:
 *      0x3: keypad data
 */

`ifdef SYNTHESIS
 `include "PmodKYPD.v"
`endif

module keypad_io_b3(
    input clk,                  // 100MHz clock
    input [3:0] addr,           // register addresses
    output [7:0] data_out,      // keypad data output
    input re,                   // read enable
    input [3:0] row,            // Rows on KYPD
    output [3:0] col            // Columns on KYPD
);
	// output buffer
	wire [4:0] keypad_buf;

	// keypad controller module
	keypad kpd(clk, row, col, keypad_buf);
    
    // local address decoding for reading from IO devices
    assign data_out = 
        ((addr == 4'h3) && re) ? keypad_buf  :    // keypad keycodes
        0;
        
endmodule
        

