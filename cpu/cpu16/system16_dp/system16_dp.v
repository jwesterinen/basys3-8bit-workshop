/*
 *  system16_dp.v
 *
 *  Description:
 *      This module is a complete 16-bit computer.  It contains a 16-bit CPU, 4K words of synchronous ROM,
 *  4K words of synchronous RAM, and the basys3.v DP wishbone peripheral for basic board I/O.
 *
 *  The memory map for the system is as follows:
 *      0x0000-0x0fff:  RAM
 *      0x2000:         switches 0-7 (read only)
 *      0x2001:         switches 8-15 (read only)
 *      0x2002:         buttons (read only)
 *      0x2003:         unused
 *      0x2004:         segments for left display
 *      0x2005:         segments
 *      0x2006:         segments
 *      0x2007:         segments for right display
 *      0xf000-0xffff:  ROM
 *
 */
 
`include "../../rom_sync.v"
`include "../../ram_sync.v"
`include "../cpu16.v"
`include "basys3.v"
`include "brddefs.h" 
 
module system16_dp(BRDIO, PCPIN);

    inout [`BRD_MX_IO:0] BRDIO;     // Board IO 
    inout [`MX_PCPIN:0] PCPIN;      // Peripheral Controller Pins (for Pmods)
    
    // for now just stub these
    reg hold = 0;
    wire busy;
    
    // stub wires for the basys3 module
    wire system_clk;
    wire stall;
    wire ack;
    wire [`MXCLK:0] clocks;    
    wire [15:0] cpu_addr;
    wire [15:0] cpu_din;
    wire [15:0] cpu_dout;
    wire we;
    
    wire [15:0] rom_dout;
    wire [15:0] ram_dout;
    wire [7:0] io_dout;
    
    // canonical output
    reg [15:0] led_reg;
    
    // system components
    ROM_sync #(.ADDR_WIDTH(12)) rom4k(system_clk, cpu_addr[11:0], rom_dout);
    RAM_sync #(.ADDR_WIDTH(12)) ram4k(system_clk, cpu_addr[11:0], cpu_dout, ram_dout, we);
    CPU16 cpu(system_clk, reset, hold, busy, cpu_addr, cpu_din, cpu_dout, we);
    basys3 io(system_clk, we, 1, 1, cpu_addr[7:0], stall, ack, cpu_dout[7:0], io_dout, clocks, BRDIO, PCPIN);
    
    // memory map decoder
    assign cpu_din = (cpu_addr[15:12] == 4'h0)  ? ram_dout :            // 0x0???
                     (cpu_addr[15:8]  == 8'h20) ? {8'h00,io_dout} :     // 0x20??
                     (cpu_addr[15:12] == 4'hf)  ? rom_dout :            // 0xf???
                     0;
                     
endmodule

