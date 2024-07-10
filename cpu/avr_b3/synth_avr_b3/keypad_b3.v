/*
 *  keypad_b3.v
 *
 *  Description:
 *      This module contains an 8-bit interface to Digilent's PmodKYPD keypad controller.
 *
 *  Registers:
 *      0x00: keypad data
 */

module keypad_b3(
    input system_clk,           // 50MHz clock
    output [7:0] data_out,      // keypad data output
    input re,                   // read enable
    output [3:0] col,           // Columns on KYPD
    input [3:0] row            // Rows on KYPD
);
	// output buffer
	wire [4:0] keypad_buf;

	// keypad controller module
	keypad kpd(system_clk, row, col, keypad_buf);
    
    // register decoding for reading from IO devices (note: there is only 1 register)
    assign data_out = (re) ? keypad_buf : 0;   // keypad keycodes
        
endmodule
        

