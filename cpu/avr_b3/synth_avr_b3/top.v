/*
*   top.v for the avr_b3 project
*
*   The avr_b3 project is a general purpose computer based on an AVR core and 
*   specifically written for the Digilent Basys3 FPGA dev board (B3).  The 
*   peripheral set is configurable, but the current design contains the I/O
*   devices intrinsic to the B3 board, switches, buttons, LEDs, and 7-segment
*   displays, VGA terminal and PS2 keyboard.  It also contains a UART that is 
*   used as a console device.  Additionally, there are 2 external peripherals 
*   that are interfaced thru 2 of the B3's Pmod connectors, the Digilent PmodKYPD 
*   on JB and a pmod-audio v1.2 Pmod amp/speaker with volume control.
*
*   With respect to the AVR core, there are 32K words (64K bytes) of program memory,
*   and 0xf000 bytes of data memory; the last 0x1000 bytes of the 64K data memory
*   contains memory-mapped I/O registers, which allows for 16 peripherals, each 
*   with up to 256 registers.  The MMIO space begins at address 0xf000.
*
*   Memory map for AVR_B3:
*       Program space:  0x0000-0xffff in program memory
*       Data space:     0x0000-0xefff in data memory
*       MMIO space:     0xf000-0xffff in data memory
*/
 
`ifdef SYNTHESIS
 `include "prescaler.v"
 `include "flash.v"
 `include "ram.v"
 `include "mmio.v"
 `include "avr_core.v"
 `include "avr_io_uart.v"
`endif

`define MMIO_BASE 4'hf

module priority_encoder ( input [3:0] irq_lines , output iflag, output reg [1:0] ivect );

    always @(*) begin
	    if (irq_lines[0])       ivect = 0;
	    else if (irq_lines[1])  ivect = 1;
	    else if (irq_lines[2])  ivect = 2;
	    else if (irq_lines[3])  ivect = 3;
	    else                    ivect = 0;
    end

    assign	iflag = |irq_lines;

endmodule

module avr_b3(	
    input  clk,         // 100MHz clock
    input [15:0] sw,    // switches
    input [4:0] btn,    // buttons - C (00001), U (00010), L (00100), R (01000), D (10000)
    output [15:0] led,  // LEDs
    output [6:0] seg,   // display segments
    output dp,          // decimal point
    output [3:0] an,    // display anode
`ifdef SYNTHESIS        // (simulator doesn't like inout ports)
    inout [7:0] JB,     // Port JB on Basys3, on PmodKYPD, JB[7:4] is rows, JB[3:0] is Columns
`endif 	
    output JA7,         // Port JA on Basys3, on Pmod-Audio JA7 is left input (IL)
    output JC0,         // Port JC0-3 used for out4 peripheral
    output JC1,
    output JC2,
    output JC3,
    input RsRx,         // UART receive signal
    output RsTx,        // UART transmit signal
    inout PS2Clk,       // PS2 keyboard clock
    inout PS2Data,      // PS2 keyboard data
    output [3:0] vgaBlue,  // VGA signals
    output [3:0] vgaGreen,
    output [3:0] vgaRed,
    output Vsync,
    output Hsync
);

    wire			io_re;
    wire			io_we;
    wire [5:0]		io_a;
    wire [7:0]		io_do;
    wire [7:0]      io_di;

    wire system_rst;
    assign system_rst = btn[0];
    
    wire [7:0] uart0_io_dout;
    wire [7:0] basic_io_dout;
    wire [7:0] keypad_io_dout;
    wire [7:0] sound_io_dout;
    
// TODO: this should really be done with a PLL
    wire system_clk;
`ifdef SYNTHESIS    
    // scale the input clock to ~12.5MHz
    prescaler #(.N(3)) ps12(clk, system_clk);
`else
    // no scaling for simulator
    assign system_clk = clk;
`endif    
    
    // ROM
    parameter pmem_width = 15;      // 15-bit width = 32K program space, BUT the AVR pmem works in _words_, so it's actually 64K _bytes_
    wire			pmem_ce;
    wire [pmem_width-1:0]	pmem_a;
    wire [15:0]		pmem_d;
    flash core0_flash(system_clk, pmem_ce, pmem_a, pmem_d); // pmem addrs 0x000-0x7fff (words!)
    defparam core0_flash.flash_width = pmem_width;

    // CPU data bus for RAM and MMIO
    parameter dmem_width = 16;      // 64K
    wire   [dmem_width-1:0] dmem_a;  // Data bus address from the CPU
    wire   dmem_re;
    wire   dmem_we;
    wire   [7:0] dmem_do;
    wire   [7:0] dmem_di;

    // RAM
    wire   [dmem_width-1:0] rio_a;   // RAM/MMIO address (-2 to let msb select ram/mmio)
    wire   ram_re;
    wire   ram_we;
    wire   [7:0] ram_di;
    assign rio_a = dmem_a[dmem_width-1:0];   // ram/mmio address
    assign ram_re = dmem_re & (dmem_a[15:12] != `MMIO_BASE);
    assign ram_we = dmem_we & (dmem_a[15:12] != `MMIO_BASE);
    ram core0_ram(system_clk, ram_re, ram_we, rio_a, ram_di, dmem_do);
    defparam core0_ram.ram_width = dmem_width;

    // memory mapped IO (MMIO)
    wire   mmio_re;
    wire   mmio_we;
    wire   [7:0] mmio_di;
    wire   PS2irq;
    assign mmio_re = dmem_re & (dmem_a[15:12] == `MMIO_BASE);
    assign mmio_we = dmem_we & (dmem_a[15:12] == `MMIO_BASE);
    mmio core0_mmio
    (
        clk, mmio_re, mmio_we, rio_a[11:0], mmio_di, dmem_do, 
`ifdef SYNTHESIS
        sw, btn, led, seg, dp, an, JB, JA7, vgaBlue, vgaGreen, vgaRed,
        Vsync, Hsync, PS2Clk, PS2Data, PS2irq
`else
        sw, btn, led, seg, dp, an, JA7, vgaBlue, vgaGreen, vgaRed,
        Vsync, Hsync, PS2Clk, PS2Data, PS2irq
`endif        
    );

    // Select between RAM and MMIO and latch value on read of either
    reg    lastread;   // ==0 if from RAM, ==1 if from mmio
    always @(posedge clk)
    begin
        lastread <= (ram_re)  ? 0 :
                    (mmio_re) ? 1 :
                    lastread;
    end
    assign dmem_di = (ram_re || (lastread == 0))  ? ram_di  :
                     (mmio_re || (lastread == 1)) ? mmio_di :
                     0;  
                     
    wire iflag;
    wire [1:0] ivect;
    wire [1:0] ieack;
    wire [1:0] core0_mode;

    //assign systick0_ack = (ieack==2'b01);     // example of interrupt ack

    //priority_encoder irq0 ( { |uart0_irq[2:0], 1'b0, 1'b0, 1'b0 }, iflag, ivect );
    priority_encoder irq0 ( { |uart0_irq[2:0], PS2irq, 1'b0, 1'b0 }, iflag, ivect );

    avr_core core0 
    (	
        system_clk, system_rst,
	    pmem_ce, pmem_a, pmem_d, 
	    dmem_re, dmem_we, dmem_a, dmem_di, dmem_do,
	    io_re, io_we, io_a, io_di, io_do,
	    iflag, ivect,
	    core0_mode,
	    ieack
    );
    defparam core0.pmem_width = pmem_width;
    defparam core0.dmem_width = dmem_width;
    defparam core0.interrupt  = 1;
    defparam core0.intr_width = 2;
    defparam core0.lsb_call = 0;
    
    // uart
    wire uart0_io_select = (io_a[5:2] == 4'b0100);			// I/O addr range 1000xx, regs 0x10-0x13
    wire uart0_io_re = (uart0_io_select ? io_re : 1'b0);
    wire uart0_io_we = (uart0_io_select ? io_we : 1'b0);
    wire uart0_txd;
    wire uart0_rxd;
    wire [2:0] uart0_irq;

    assign RsTx = uart0_txd;
    assign uart0_rxd = RsRx; 

    avr_io_uart uart0 
    (	system_clk, system_rst, 
	    uart0_io_re, uart0_io_we, io_a[1:0], io_di, io_do,
	    uart0_txd, uart0_rxd,
	    uart0_irq
    );
endmodule

