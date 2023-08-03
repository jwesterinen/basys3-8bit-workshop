/*
 *  system16.v
 *
 *  Description:
 *      This module is a complete 16-bit computer.  It contains a 16-bit CPU, 4K words of synchronous ROM,
 *  4K words of synchronous RAM, 16 switches for input, and 16 LEDs for output.
 *
 *  The memory map for the system is as follows:
 *      0x0000-0x0fff: RAM
 *      0x2000-0x20ff: basic I/O
 *          0x2000: switch reg
 *          0x2002: button reg
 *          0x2010: LED reg
 *          0x2020: display reg 1
 *          0x2021: display reg 2
 *          0x2022: display reg 3
 *          0x2023: display reg 4
 *          0x2024: display control reg
 *      0xf000-0xffff: ROM
 *
 */
 
`ifdef SYNTHESIS
 `include "../../prescaler.v"
 `include "../../rom_sync.v"
 `include "../../ram_sync.v"
 `include "../../basic_io_16.v"
 `include "../cpu16.v"
`endif
 
 module system16(
    input  clk,         // 100MHz clock
    input [15:0] sw,    // switches
    input [4:0] btn,    // buttons - C (00001), U (00010), L (00100), R (01000), D (10000)
    output [15:0] led,  // LEDs
    output [6:0] seg,   // display segments
    output dp,          // decimal point
    output [3:0] an     // display anode
 );
    
    // for now just stub these
    reg hold = 0;
    wire busy;
    
    wire [15:0] cpu_addr;
    wire [15:0] cpu_din;
    wire [15:0] cpu_dout;
    wire we;
    
    wire [15:0] rom_dout;
    wire [15:0] ram_dout;
    wire [15:0] io_dout;
    
    // 25MHz clock
    //wire system_clk;
    //prescaler #(.N(2)) ps2(clk, system_clk);
    
    // system components
    ROM_sync #(.ADDR_WIDTH(12)) rom4k(clk, cpu_addr[11:0], rom_dout);
    RAM_sync #(.ADDR_WIDTH(12)) ram4k(clk, cpu_addr[11:0], cpu_dout, ram_dout, we);
    CPU16 cpu(clk, btn[0], hold, busy, cpu_addr, cpu_din, cpu_dout, we);
    basic_io_16 io(clk, cpu_addr[7:0], cpu_dout, io_dout, we, sw, btn, led, seg, dp, an);

    // memory data source selection
    assign cpu_din = (cpu_addr[15:12] == 4'b0000) ? ram_dout :
                     (cpu_addr[15:12] == 4'b0010) ? io_dout  :
                     (cpu_addr[15:12] == 4'b1111) ? rom_dout :
                     0;
    
endmodule

