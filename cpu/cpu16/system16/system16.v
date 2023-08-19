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
 *      0x3000-0x30ff: sound I/O
 *          0x3000: VCO1 freq
 *          0x3001: VCO2 freq
 *          0x3002: noise freq
 *          0x3003: LFO freq
 *          0x3005: LFO modulation depth
 *          0x3004: modulation select: {0.., noise, VCO2, VCO1}
 *          0x3006: mixer: {0.., LFO, noise, VCO2, VCO1}
 *      0xf000-0xffff: ROM
 *
 */
 
`ifdef SYNTHESIS
 `include "../../prescaler.v"
 `include "../../rom_sync.v"
 `include "../../ram_sync.v"
 `include "../../basic_io_16.v"
 `include "../../sound_io_16.v"
 `include "../../../vga25Mhz/sound_generator/SN76477.v"
 `include "../../../vga25Mhz/sound_generator/lfsr.v"
 `include "../cpu16.v"
`endif
 
 module system16(
    input  clk,         // 100MHz clock
    input [15:0] sw,    // switches
    input [4:0] btn,    // buttons - C (00001), U (00010), L (00100), R (01000), D (10000)
    output [15:0] led,  // LEDs
    output [6:0] seg,   // display segments
    output dp,          // decimal point
    output [3:0] an,    // display anode
    output JA7          // Pmod Audio left input (IL)
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
    wire [15:0] basic_io_dout;
    wire [15:0] sound_io_dout;
    
    initial begin
        system_clk = 0;
    end
    
    // clocks - memory (100MHz) is clocked 2x the system clock (50MHz)
    reg system_clk;
    wire mem_clk;
    assign mem_clk = clk;
    always @(posedge mem_clk)
        system_clk = ~system_clk;
        
    // system components
    ROM_sync #(.ADDR_WIDTH(12)) rom4k(mem_clk, cpu_addr[11:0], rom_dout);
    RAM_sync #(.ADDR_WIDTH(12)) ram4k(mem_clk, cpu_addr[11:0], cpu_dout, ram_dout, we);
    CPU16 #(.RAM_WAIT(0)) cpu(system_clk, btn[0], hold, busy, cpu_addr, cpu_din, cpu_dout, we);
    basic_io_16 #(.BASE_ADDR(16'h2000)) io(system_clk, cpu_addr, cpu_dout, basic_io_dout, we, sw, btn, led, seg, dp, an);
    sound_io_16 #(.BASE_ADDR(16'h3000)) sndgen(system_clk, btn[0], cpu_addr, cpu_dout, sound_io_dout, we, JA7);

    // memory data source selection
    assign cpu_din = (cpu_addr[15:12] == 4'b0000) ? ram_dout        :   // 0x0???
                     (cpu_addr[15:12] == 4'b0010) ? basic_io_dout   :   // 0x20??
                     (cpu_addr[15:12] == 4'b0011) ? sound_io_dout   :   // 0x30??
                     (cpu_addr[15:12] == 4'b1111) ? rom_dout        :   // 0xf000
                     0;
    
endmodule

