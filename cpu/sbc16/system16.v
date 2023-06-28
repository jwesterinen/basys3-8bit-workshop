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
 
 `include "rom_sync.v"
 `include "ram_sync.v"
 `include "cpu16.v"
 
 module system16(
    input clk,
    input reset,
    //input hold,
    input [15:0] switches,
    //output busy,
    output [15:0] leds
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
    
    // canonical output
    reg [15:0] led_reg;
    
    // system components
    ROM_sync #(.ADDR_WIDTH(12)) rom4k(clk, cpu_addr[11:0], rom_dout);
    RAM_sync #(.ADDR_WIDTH(12)) ram4k(clk, cpu_addr[11:0], cpu_dout, ram_dout, we);
    CPU16 cpu(clk, reset, hold, busy, cpu_addr, cpu_din, cpu_dout, we);
    
    // memory data source selection
    assign cpu_din = (cpu_addr[15:12] == 16'b0000) ? ram_dout :
                     (cpu_addr == 16'h2000)        ? switches:
                     (cpu_addr[15:12] == 16'b1111) ? rom_dout :
                     0;
    
    // I/O memory decoding
    always @(*)
        if (cpu_addr == 16'h2001)
            led_reg <= cpu_dout;
            
    assign leds = led_reg;
    
  // example ROM program code
`ifdef EXT_INLINE_ASM
    initial begin
        program_rom = '{
      __asm
.arch femto16
.org 0xF000
.len 32768
Loop:
      zero  ax
      zero  ax
      zero  ax
      jmp   Loop
      __endasm
    };
    end
`endif

endmodule

