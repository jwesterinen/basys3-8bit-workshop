/*
 *  keypad_io16.v
 *
 *  Description:
 *      This module contains a 16-bit interface to Digilent's PmodKYPD keypad controller.
 *
 *  The local memory map for this module is as follows:
 *      BASE_ADDR + 0x00: keypad data
 */
 
module keypad_io_16(
    input clk,                  // 100MHz onboard clock
    input [15:0] addr,          // register addresses
    input [3:0] row,            // Rows on KYPD
    output [3:0] col,           // Columns on KYPD
    output [15:0] data_out      // keypad data output
);
	parameter BASE_ADDR = 0;    // base address for registers
	
	// output buffer
	wire [4:0] keypad_buf;

	
	// keypad controller module
	keypad C0(clk, row, col, keypad_buf);
    
    // local address decoding for reading from IO devices
    assign data_out = 
        (addr == {BASE_ADDR[15:8],8'h00}) ? {11'h0,keypad_buf}  :    // keypad keycodes
        0;
        
endmodule
        

