/*
 *  system6502.v
 *
 *  Description:
 *      This module is a complete 16-bit computer.  It contains a 6502 CPU, 4K words of synchronous ROM,
 *  4K words of synchronous RAM, 16 switches for input, and 16 LEDs for output.
 *
 *  The memory map for the system is as follows:
 *      0x0000-0x0fff: ROM
 *             0x2000: switch reg LSB
 *             0x2001: switch reg MSB
 *             0x2002: button reg
 *             0x2010: LED reg LSB
 *             0x2011: LED reg MSB
 *             0x2020: display reg 1
 *             0x2021: display reg 2
 *             0x2022: display reg 3
 *             0x2023: display reg 4
 *             0x2024: display control reg
 *      0xe000-0xefff: RAM
 *      0xfffc-0xffff: reset vector
 *
 */

`ifdef SYNTHESIS
`include "../../prescaler.v"
`include "../../rom_sync.v"
`include "../../ram_sync.v"
`include "../../basic_io_8.v"
`include "../cpu6502.v"
`endif
 
 module system6502(
    input  clk,         // 100MHz clock
    input [15:0] sw,    // switches
    input [4:0] btn,    // buttons - C (00001), U (00010), L (00100), R (01000), D (10000)
    output [15:0] led,
    output [6:0] seg,
    output dp,
    output [3:0] an
 );
    
    // for now just stub these
    reg irq = 0;
    reg nmi = 0;
    reg ready = 1;
    
    wire [15:0] cpu_addr;
    wire [7:0] cpu_din;
    wire [7:0] cpu_dout;
    wire rdwr_;
    
    wire [7:0] rom_dout;
    wire [7:0] ram_dout;
    wire [7:0] io_dout;
    wire we;
    
    // vector table
    reg [7:0] vector_tbl[0:5];
    
    initial begin
        system_clk = 0;
        
        // TODO: for now all vectors are the same
        vector_tbl[0] <= 8'h00;     // reset vector
        vector_tbl[1] <= 8'he0;
        vector_tbl[2] <= 8'h00;     // NMI vector
        vector_tbl[3] <= 8'he0;
        vector_tbl[4] <= 8'h00;     // IRQ vector
        vector_tbl[5] <= 8'he0;
    end
    
    // clocks - memory (100MHz) is clocked 2x the system clock (50MHz)
    reg system_clk;
    wire mem_clk;
    assign mem_clk = clk;
    always @(posedge mem_clk)
        system_clk = ~system_clk;
        
    // conversion of 6502 rdwr_ signal to we for common components
    assign we = ~rdwr_;
    
    // system components
    ROM_sync #(.ADDR_WIDTH(12), .DATA_WIDTH(8)) rom4k(mem_clk, cpu_addr[11:0], rom_dout);
    RAM_sync #(.ADDR_WIDTH(12), .DATA_WIDTH(8)) ram4k(mem_clk, cpu_addr[11:0], cpu_dout, ram_dout, we);
    cpu6502 cpu(system_clk, btn[0], cpu_addr, cpu_din, cpu_dout, rdwr_, irq, nmi, ready);
    basic_io_8 io(system_clk, cpu_addr[7:0], cpu_dout, io_dout, we, sw, btn, led, seg, dp, an);
    
    // memory data source selection
    assign cpu_din = 
        (cpu_addr[15:12] == 4'h0)   ? ram_dout      :    // 0x0??? - RAM
        (cpu_addr[15:8]  == 8'h20)  ? io_dout       :    // 0x20?? - I/O
        (cpu_addr[15:12] == 4'he)   ? rom_dout      :    // 0xe??? - ROM
        (cpu_addr == 16'hfffc)      ? vector_tbl[0] :    // reset vector LSB
        (cpu_addr == 16'hfffd)      ? vector_tbl[1] :    // reset vector MSB
        0;

endmodule

