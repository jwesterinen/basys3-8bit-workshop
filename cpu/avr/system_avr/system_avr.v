/*
 *  system_avr.v
 *
 *  Description:
 *      This module is an AVR-based computer.  It contains an AVR core with 
 *  4K words of program memory (ROM) and 4K words of data memory (RAM), 
 *  16 switches and a keypad for input and 16 LEDs and four 7-segment displays 
 *  for output, and a sound generator.
 *
 *  The memory map for the system is as follows:
 
 *      0x0000-0x0fff: program memory (ROM or Flash)
 *      0x0000-0x0fff: data memory (RAM)
 *      
 *  The I/O addresses for the system are as follows:
 *      0x0000-0x00ff: basic I/O
 *          0x0000: switch reg
 *          0x0002: button reg
 *          0x0010: LED reg
 *          0x0020: display reg 1
 *          0x0021: display reg 2
 *          0x0022: display reg 3
 *          0x0023: display reg 4
 *          0x0024: display control reg
 
 *      0x0100-0x01ff: sound I/O
 *          0x0100: VCO1 freq
 *          0x0101: VCO2 freq
 *          0x0102: noise freq
 *          0x0103: LFO freq
 *          0x0105: LFO modulation depth
 *          0x0104: modulation select: {0.., noise, VCO2, VCO1}
 *          0x0106: mixer: {0.., LFO, noise, VCO2, VCO1}
 
 *      0x0200: keypad data reg
 *
 */
 
`ifdef SYNTHESIS
 `include "../../prescaler.v"
 `include "../../basic_io_16.v"
 `include "../../sound_io_16.v"
 `include "../../../vga25Mhz/sound_generator/SN76477.v"
 `include "../../../vga25Mhz/sound_generator/lfsr.v"
 `include "../../keypad_io_16.v"
 `include "../../PmodKYPD.v"
 `include "../avr_core.v"
`endif
 
 module system_avr(
    input  clk,         // 100MHz clock
    input [15:0] sw,    // switches
    input [4:0] btn,    // buttons - C (00001), U (00010), L (00100), R (01000), D (10000)
    output [15:0] led,  // LEDs
    output [6:0] seg,   // display segments
    output dp,          // decimal point
    output [3:0] an,    // display anode
    output JA7,         // Port JA on Basys3, on Pmod-Audio JA7 is left input (IL)
	inout [7:0] JB      // Port JB on Basys3, on PmodKYPD, JB[7:4] is rows, JB[3:0] is Columns
 );
    
    wire pmem_ce;
    wire [8:0] pmem_a;
    reg	 [15:0] pmem_d;

    wire dmem_re;
    wire dmem_we;
    wire [8:0] dmem_a;
    reg  [7:0] dmem_di;
    wire [7:0] dmem_do;

    wire io_re;
    wire io_we;
    wire [5:0] io_a;
    reg  [7:0] io_di;
    wire [7:0] io_do;

    reg  in_iflag;
    reg  [1:0] in_ivect;

    wire [1:0]	mode;
    wire [1:0] in_ieack;
    
    wire [15:0] basic_io_dout;
    wire [15:0] sound_io_dout;
    wire [15:0] keypad_io_dout;
    
    reg system_clk;
    
    initial begin
        system_clk = 0;
    end
    
    // clocks - memory (100MHz) is clocked 2x the system clock (50MHz)
    wire mem_clk;
    assign mem_clk = clk;
    always @(posedge mem_clk)
        system_clk = ~system_clk;
        
    // system components
    //CPU16 #(.RAM_WAIT(0)) cpu(system_clk, btn[0], hold, busy, cpu_addr, cpu_din, cpu_dout, we);
    avr_core #(.interrupt(0)) avr_test(
        clk, btn[0], 
        pmem_ce, pmem_a, pmem_d,                        // program memory (ROM)
        dmem_re, dmem_we, dmem_a, dmem_di, dmem_do,     // data memory (RAM)
        io_re, io_we, io_a, io_di, io_do,               // I/O space
        in_iflag, in_ivect, mode, in_ieack              // interrupt control
    );
    ROM_sync #(.ADDR_WIDTH(12)) rom4k(mem_clk, pmem_a, pmem_d, pmem_ce);
    RAM_sync #(.ADDR_WIDTH(12)) ram4k(mem_clk, dmem_a, dmem_di, dmem_do, dmem_re, dmem_we);
    basic_io_16 #(.BASE_ADDR(16'h2000)) io(system_clk, io_a, cpu_dout, basic_io_dout, we, sw, btn, led, seg, dp, an);
    sound_io_16 #(.BASE_ADDR(16'h3000)) sndgen(system_clk, btn[0], io_a, cpu_dout, sound_io_dout, we, JA7);
	keypad_io_16 #(.BASE_ADDR(16'h4000)) keypad(system_clk, io_a, JB[7:4], JB[3:0], keypad_io_dout);

    // memory device chip selection
    assign ram_cs = (cpu_addr[15:12] ==  4'h0) ? 1 : 0; // 0x0???
    
    // memory data source selection
    assign cpu_din = (cpu_addr[15:12] ==  4'h0)    ? ram_dout        :   // 0x0???
                     (cpu_addr[15:8]  ==  8'h20)   ? basic_io_dout   :   // 0x20??
                     (cpu_addr[15:8]  ==  8'h30)   ? sound_io_dout   :   // 0x30??
                     (cpu_addr        == 16'h4000) ? keypad_io_dout  :   // 0x4000
                     (cpu_addr[15:12] ==  4'hf)    ? rom_dout        :   // 0xf???
                     0;
    
endmodule

