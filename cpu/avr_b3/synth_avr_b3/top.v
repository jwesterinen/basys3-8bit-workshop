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

module top(	
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
    input RsRx,
    output RsTx
);

    parameter pmem_width = 12;    // 8K (0x2000)
    parameter dmem_width = 12;    // 8K (0x2000)

    wire			pmem_ce;
    wire [pmem_width-1:0]	pmem_a;
    wire [15:0]		pmem_d;

    wire			dmem_re;
    wire			dmem_we;
    wire [dmem_width-1:0] 	dmem_a;
    wire [7:0]		dmem_do;

    wire			io_re;
    wire			io_we;
    wire [5:0]		io_a;
    wire [7:0]		io_do;
    wor [7:0]       io_di;

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
    
    flash core0_flash( system_clk, pmem_ce,pmem_a, pmem_d );
    defparam core0_flash.flash_width = pmem_width;

`define IO_MAPPED_IO
//`define EXT_IO_MAPPED_IO

`ifdef IO_MAPPED_IO

    wire [7:0] dmem_di;

    ram core0_ram( system_clk, dmem_re, dmem_we, dmem_a, dmem_di, dmem_do );    // dmem addrs 0x000-0x7ff
    defparam core0_ram.ram_width = dmem_width-1;

    // basic I/O (switches, buttons, LEDs, and 7-segment displays
    wire basic_io_select = (io_a[5:4] == 4'b00);            // I/O addr range 00xxxx, regs 0x00-0x0f
    wire basic_io_re = basic_io_select & io_re;
    wire basic_io_we = basic_io_select & io_we;
    basic_io_b3 basic_io(
        clk, 
        io_a[3:0], io_di, io_do, basic_io_re, basic_io_we, 
        sw, btn, led, seg, dp, an
    );

    // Pmod keypad
    wire keypad_io_select = (io_a[5:4] == 4'b00);           // I/O addr range 00xxxx, reg 0x03
    wire keypad_io_re = keypad_io_select & io_re;
    wire keypad_io_we = keypad_io_select & io_we;
	keypad_io_b3 keypad(clk, io_a[3:0], io_di, keypad_io_re, JB[7:4], JB[3:0]);
    
    // sound generator w/Pmod amp/speaker
    wire sound_io_select = (io_a[5:4] == 4'b01);            // I/O addr range 01xxxx, regs 0x10-0x1f
    wire sound_io_re = sound_io_select & io_re;
    wire sound_io_we = sound_io_select & io_we;
    sound_io_b3 sound_io(clk_50MHz, btn[0], io_a[3:0], io_di, io_do, sound_io_re, sound_io_we, JA7);
        
`else

    wor [7:0] dmem_di;

    wire ram_select = (dmem_a >= 12'h100);                           // dmem addrs 0x100-0x7ff
    wire ram_re = ram_select & dmem_re;
    wire ram_we = ram_select & dmem_we;
    ram	core0_ram ( system_clk, ram_re, ram_we, dmem_a[dmem_width-2:0], dmem_di, dmem_do );
    defparam core0_ram.ram_width = dmem_width-1;

`ifdef EXT_IO_MAPPED_IO

    wire basic_io_select = (dmem_a >= 12'h060 && dmem_a < 12'h100);             // dmem addrs 0x060-0x0ff
    wire basic_io_re = basic_io_select & dmem_re;
    wire basic_io_we = basic_io_select & dmem_we;
    basic_io_b3 basic_io(
        clk, 
        dmem_a[3:0], dmem_di, dmem_do, basic_io_re, basic_io_we, 
        sw, btn, led, seg, dp, an, JB[7:4], JB[3:0]
    );
    
`else // memory mapped I/O

    wire basic_io_select = (dmem_a[dmem_width-1:4] == 8'b10000000);             // dmem addrs 0x800-0x8ff
    wire basic_io_re = basic_io_select & dmem_re;
    wire basic_io_we = basic_io_select & dmem_we;
    basic_io_b3 basic_io(
        clk, 
        dmem_a[3:0], dmem_di, dmem_do, basic_io_re, basic_io_we, 
        sw, btn, led, seg, dp, an, JB[7:4], JB[3:0]
    );
    
`endif   
`endif 

//`define TIMER0

`ifdef TIMER0
    wire timer0_io_select = (io_a[5:2] == 4'b0010);         // I/O addr range 0010xx: regs 0x08-0x0b
    wire timer0_io_re = timer0_io_select & io_re;
    wire timer0_io_we = timer0_io_select & io_we;
    wire timer0_irq;

    avr_io_timer timer0
     (      system_clk, 1'b0, 
            timer0_io_re, timer0_io_we, io_a[1:0], io_di, io_do,
	    timer0_irq
     );
`else
    wire timer0_irq = 0;
`endif

    wire uart0_io_select = (io_a[5:2] == 4'b1000);			// I/O addr range 1000xx: regs 0x20-0x23
    wire uart0_io_re = (uart0_io_select ? io_re : 1'b0);
    wire uart0_io_we = (uart0_io_select ? io_we : 1'b0);
    wire uart0_txd;
    wire uart0_rxd;
    wire [2:0] uart0_irq;

    assign RsTx = uart0_txd;
    assign uart0_rxd = RsRx; 

    avr_io_uart uart0 
     (	system_clk, 1'b0, 
	    uart0_io_re, uart0_io_we, io_a[1:0], io_di, io_do,
	    uart0_txd, uart0_rxd,
	    uart0_irq
     );

    wire iflag;
    wire [1:0] ivect;
    wire [1:0] ieack;
    wire [1:0] core0_mode;

    //assign systick0_ack = (ieack==2'b01);     // example of interrupt ack

    priority_encoder irq0 ( { |uart0_irq[2:0], 1'b0, systick0_irq, 1'b0 }, iflag, ivect );

    avr_core core0 
     (	system_clk, btn[0],
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

endmodule

