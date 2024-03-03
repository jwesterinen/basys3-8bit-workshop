`ifdef SYNTHESIS
 `include "avr_core.v"
 `include "basic_io_b3.v"
 `include "keypad_io_b3.v"
 `include "sound_io_b3.v"
 `include "avr_io_uart.v"
 `include "avr_io_timer.v"
 `include "flash.v"
 `include "ram.v"
 `include "prescaler.v"
 `include "sysdefs.h"
 `include "clocks.v"
 `include "out4.v"
`endif

/*****************************************************************************/

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
    output RsTx         // UART transmit signal
);

    parameter pmem_width = 12;    // 4K (0x1000)
    parameter dmem_width = 12;

    wire			pmem_ce;
    wire [pmem_width-1:0]	pmem_a;
    wire [15:0]		pmem_d;

    wire			dmem_re;
    wire			dmem_we;
    wire [dmem_width-1:0] 	dmem_a;
    wire [7:0]		dmem_do;
    wire [7:0]      dmem_di;

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
    wire [7:0] out4_io_dout;
    
// TODO: this should really be done with a PLL
    wire system_clk;
`ifdef SYNTHESIS    
    // scale the input clock to ~12.5MHz
    prescaler #(.N(3)) ps12(clk, system_clk);
`else
    // no scaling for simulator
    assign system_clk = clk;
`endif    
    
    wire clk_50MHz;
`ifdef SYNTHESIS    
    // scale the input clock to 50MHz
    prescaler #(.N(1)) ps50(clk, clk_50MHz);
`else
    // no scaling for simulator
    assign clk_50MHz = clk;
`endif    
    
    // ROM
    flash core0_flash(system_clk, pmem_ce, pmem_a, pmem_d);                 // pmem addrs 0x000-0xfff
    defparam core0_flash.flash_width = pmem_width;

    // RAM
    ram core0_ram(system_clk, dmem_re, dmem_we, dmem_a, dmem_di, dmem_do);  // dmem addrs 0x060-0xfff
    defparam core0_ram.ram_width = dmem_width;

    wire iflag;
    wire [1:0] ivect;
    wire [1:0] ieack;
    wire [1:0] core0_mode;

    //assign systick0_ack = (ieack==2'b01);     // example of interrupt ack

    priority_encoder irq0 ( { |uart0_irq[2:0], 1'b0, 1'b0, 1'b0 }, iflag, ivect );

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
    
//`define TIMER0

`ifdef TIMER0
    wire timer0_io_select = (io_a[5:2] == 4'b0010);         // I/O addr range 0010xx: regs 0x08-0x0b
    wire timer0_io_re = timer0_io_select & io_re;
    wire timer0_io_we = timer0_io_select & io_we;
    wire timer0_irq;

    avr_io_timer timer0
    (      
        system_clk, 1'b0, 
        timer0_io_re, timer0_io_we, io_a[1:0], io_di, io_do,
	    timer0_irq
    );
`else
    wire timer0_irq = 0;
`endif

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
	    uart0_io_re, uart0_io_we, io_a[1:0], uart0_io_dout, io_do,
	    uart0_txd, uart0_rxd,
	    uart0_irq
    );

    // basic I/O (switches, buttons, LEDs, and 7-segment displays
    wire basic_io_select = (io_a[5:4] == 4'b00);            // I/O addr range 00xxxx, regs 0x00-0x0f
    wire basic_io_re = basic_io_select & io_re;
    wire basic_io_we = basic_io_select & io_we;
    basic_io_b3 basic_io
    (
        clk, 
        io_a[3:0], basic_io_dout, io_do, basic_io_re, basic_io_we, 
        sw, btn, led, seg, dp, an
    );

`ifdef SYNTHESIS        // (simulator doesn't like inout ports)
    // Pmod keypad
    wire keypad_io_select = (io_a[5:0] == 5'b000011);       // I/O addr range 000011, reg 0x03
    wire keypad_io_re = keypad_io_select & io_re;
	keypad_io_b3 keypad
	(
	    clk, 
	    keypad_io_dout, keypad_io_re, 
	    JB[7:4], JB[3:0]
	);
`endif    
    
    // sound generator w/Pmod amp/speaker
    wire sound_io_select = (io_a[5:2] == 4'b0101);          // I/O addr range 0100xx, regs 0x14-0x17
    wire sound_io_re = sound_io_select & io_re;
    wire sound_io_we = sound_io_select & io_we;
    sound_io_b3 sound_io
    (
        clk_50MHz, 
        system_rst, io_a[1:0], sound_io_dout, io_do, sound_io_re, sound_io_we, 
        JA7
    );
        
    // DP peripherals

    // DP clocks peripheral
    wire CLK_O;
    wire [`MXCLK:0] peri_clks;
    clocks clks(clk, CLK_O, peri_clks);

    // DP out4
    wire out4_io_select = (io_a[5:0] == 5'b000111);           // I/O addr range 000111, reg 0x07
    wire out4_stall;
    wire out4_ack;
    out4 o4
    (
        clk, 
        io_we, 1'b1, out4_io_select, 8'h00, out4_stall, out4_ack, io_do, out4_io_dout, peri_clks, 
        {JC3,JC2,JC1,JC0}
    );

    // I/O data source selection
 
    assign io_di =  (out4_io_select)    ? out4_io_dout      :   // 000111, reg  0x07
`ifdef SYNTHESIS
                    (keypad_io_select)  ? keypad_io_dout    :   // 000011, reg  0x03
`endif                    
                    (basic_io_select)   ? basic_io_dout     :   // 00xxxx, regs 0x00-0x0f
                    (uart0_io_select)   ? uart0_io_dout     :   // 1000xx, regs 0x10-0x13
                    (sound_io_select)   ? sound_io_dout     :   // 0100xx, regs 0x14-0x17
                    0;

endmodule

