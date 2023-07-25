/*
 *  system6502.v
 *
 *  Description:
 *      This module is a complete 16-bit computer.  It contains a 6502 CPU, 4K words of synchronous ROM,
 *  4K words of synchronous RAM, 16 switches for input, and 16 LEDs for output.
 *
 *  The memory map for the system is as follows:
 *      0x0000-0x0fff: RAM
 *                  0x0000-0x00ff: RAM zero page
 *                  0x0100-0x01ff: stack
 *                  0x0200-0x0ff0: general purpose RAM
 *                  0x0ff0-0x0ff7: interrupt jump table
 *      0x2000-0x20ff; basic I/O
 *                  0x2000: switch reg LSB
 *                  0x2001: switch reg MSB
 *                  0x2002: button reg
 *                  0x2010: LED reg LSB
 *                  0x2011: LED reg MSB
 *                  0x2020: display reg 1
 *                  0x2021: display reg 2
 *                  0x2022: display reg 3
 *                  0x2023: display reg 4
 *                  0x2024: display control reg
 *      0xe000-0xefff: ROM
 *      0xfffa-0xffff: vector table
 *                  0xfffa-0xfffb: NMI vector (0x0ffa)
 *                  0xfffc-0xfffd: reset_ vector (0xe000)
 *                  0xfffc-0xffff: IRQ vector (0x0ffd)
 *
 */

`ifdef SYNTHESIS
`include "../../prescaler.v"
`include "../../rom_sync.v"
`include "../../ram_sync.v"
`include "../../basic_io_8.v"
`include "../ag_6502.v"
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
    
    // inactive inputs
    reg irq;
    reg nmi;
    reg ready;
    reg so;
    
    // unused outputs
    wire clk01;
    wire clk02;
    wire sync;

    // clocks
    reg system_clk;    
    wire mem_clk;
        
    // CPU busses
    wire [15:0] cpu_addr;
    wire [7:0] cpu_din;
    wire [7:0] cpu_dout;
    wire rdwr_;
    
    // module connection signals
    wire [7:0] rom_dout;
    wire [7:0] ram_dout;
    wire [7:0] io_dout;
    wire we;
    
    // reset_ circuitry
    wire reset_;
    reg hold_reset;     // hold reset == true
    reg [2:0] por;      // init power-on-reset counter to 8 (clocks)
    
    // vector table
    reg [7:0] vector_tbl[0:5];
    
    initial begin
        system_clk <= 0;
        
        irq <= 0;       // no interrupts
        nmi <= 0;
        ready <= 1;     // always ready
        so <= 1;        // no external overflow
        
        hold_reset <=1;
        por <= 8;
        
        vector_tbl[0] <= 8'hf0;     // NMI vector: 0x0ff0
        vector_tbl[1] <= 8'h0f;
        vector_tbl[2] <= 8'h00;     // reset vector: 0xe000
        vector_tbl[3] <= 8'he0;
        vector_tbl[4] <= 8'hf4;     // IRQ vector: 0x0ff4
        vector_tbl[5] <= 8'h0f;
    end
    
    // clocks - memory (100MHz) is clocked 2x the system clock (50MHz)
    assign mem_clk = clk;
    always @(posedge mem_clk)
        system_clk = ~system_clk;
        
    // conversion of 6502 rdwr_ signal to we for common components
    assign we = ~rdwr_;
    
    // system components
    ROM_sync #(.ADDR_WIDTH(12), .DATA_WIDTH(8)) rom4k(mem_clk, cpu_addr[11:0], rom_dout);
    RAM_sync #(.ADDR_WIDTH(12), .DATA_WIDTH(8)) ram4k(mem_clk, cpu_addr[11:0], cpu_dout, ram_dout, we);
    ag6502 cpu( system_clk, clk01, clk02, cpu_addr, rdwr_, cpu_din, cpu_dout, ready, reset_, irq, nmi, so, sync );
    basic_io_8 io(system_clk, cpu_addr[7:0], cpu_dout, io_dout, we, sw, btn, led, seg, dp, an);
    
    // POR: hold the cpu in reset for 8 clocks at power up
    always @(posedge system_clk) begin
        if (hold_reset == 1 && por != 0)
            por <= por - 1;
        else
            hold_reset <= 0;    // false
    end
    assign reset_ = (hold_reset) ? 0 : ~btn[0];
    
    // memory data source selection
    assign cpu_din = 
        (cpu_addr[15:12] == 4'h0)   ? ram_dout      :    // 0x0??? - RAM
        (cpu_addr[15:8]  == 8'h20)  ? io_dout       :    // 0x20?? - I/O
        (cpu_addr[15:12] == 4'he)   ? rom_dout      :    // 0xe??? - ROM
        (cpu_addr == 16'hfffa)      ? vector_tbl[0] :    // NMI vector LSB
        (cpu_addr == 16'hfffb)      ? vector_tbl[1] :    // NMI vector MSB
        (cpu_addr == 16'hfffc)      ? vector_tbl[2] :    // reset_ vector LSB
        (cpu_addr == 16'hfffd)      ? vector_tbl[3] :    // reset_ vector MSB
        (cpu_addr == 16'hfffe)      ? vector_tbl[4] :    // IRQ vector LSB
        (cpu_addr == 16'hffff)      ? vector_tbl[5] :    // IRQ vector MSB
        0;

endmodule

