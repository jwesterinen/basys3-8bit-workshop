/*
 *  keypad_b3.v
 *
 *  Description:
 *      This module contains an 8-bit interface to Digilent's PmodKYPD keypad controller.
 *
 *  Registers:
 *      0x00: keypad data
 */

`ifdef SYNTHESIS
 `include "PmodKYPD.v"
`endif

module keypad_b3(
    input clk,                  // 100MHz clock
    output [7:0] data_out,      // keypad data output
    input re,                   // read enable
    input [3:0] row,            // Rows on KYPD
    output [3:0] col            // Columns on KYPD
);
	// output buffer
	wire [4:0] keypad_buf;

	// keypad controller module
	keypad kpd(clk, row, col, keypad_buf);
    
    // register decoding for reading from IO devices (note: there is only 1 register)
    assign data_out = (re) ? keypad_buf : 0;   // keypad keycodes
        
endmodule
        

