/*
 *  system16.v
 *
 *  Description:
 *      This module is a complete 16-bit computer.  It contains a 16-bit CPU, 4K words of synchronous ROM,
 *  4K words of synchronous RAM, 16 switches for input, and 16 LEDs for output.
 *
 *  The memory map for the system is as follows:
 *      0x0000-0x0fff: RAM
 *             0x2000: switch reg
 *             0x2001: LED reg
 *      0xf000-0xffff: ROM
 *
 */
 
 `include "../../prescaler.v"
 `include "../../rom_sync.v"
 `include "../../ram_sync.v"
 `include "../../basic_io.v"
 `include "../cpu16.v"
 
 module system16(
    input  clk,         // 100MHz clock
    input [15:0] sw,    // canonical I/O
    input [4:0] btn,    // buttons C (00001), U (00010), L (00100), R (01000), D (10000)
    output [15:0] led   // LEDs
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
    
    // canonical output
//    reg [15:0] led_reg;
    
    // 25MHz clock
    wire system_clk;
    prescaler #(.N(2)) ps2(clk, system_clk);
    
    // system components
    ROM_sync #(.ADDR_WIDTH(12)) rom4k(clk, cpu_addr[11:0], rom_dout);
    RAM_sync #(.ADDR_WIDTH(12)) ram4k(clk, cpu_addr[11:0], cpu_dout, ram_dout, we);
    CPU16 cpu(system_clk, btn[0], hold, busy, cpu_addr, cpu_din, cpu_dout, we);
    basic_io io(system_clk, cpu_addr[7:0], cpu_dout, io_dout, we, sw, btn, led);

    // memory data source selection
    assign cpu_din = (cpu_addr[15:12] == 4'b0000) ? ram_dout :
                     (cpu_addr[15:12] == 4'b0010) ? io_dout  :
                     (cpu_addr[15:12] == 4'b1111) ? rom_dout :
                     0;
/*    
    // memory data source selection
    assign cpu_din = (cpu_addr[15:12] == 16'b0000) ? ram_dout :
                     (cpu_addr == 16'h2000)        ? sw:
                     (cpu_addr[15:12] == 16'b1111) ? rom_dout :
                     0;
    
    // I/O memory decoding
    always @(*)
        if (cpu_addr == 16'h2001)
            led_reg <= cpu_dout;
            
    assign led = led_reg;
*/
    
endmodule

