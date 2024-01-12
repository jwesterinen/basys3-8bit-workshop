`ifdef SYNTHESIS
 `include "avr_core.v"
 `include "avr_io_out.v"
 `include "flash.v"
 `include "ram.v"
`endif

 module system_avr(
    input  clk,         // 100MHz clock
//    input [15:0] sw,    // switches
    input [4:0] btn,    // buttons - C (00001), U (00010), L (00100), R (01000), D (10000)
    output [15:0] led  // LEDs
//    output [6:0] seg,   // display segments
//    output dp,          // decimal point
//    output [3:0] an,    // display anode
//    output JA7,         // Port JA on Basys3, on Pmod-Audio JA7 is left input (IL)
//	inout [7:0] JB      // Port JB on Basys3, on PmodKYPD, JB[7:4] is rows, JB[3:0] is Columns
 );
    parameter	pmem_width = 12;
    parameter	dmem_width = 10;

    wire			pmem_ce;
    wire [pmem_width-1:0]	pmem_a;
    wire [15:0]		pmem_d;

    wire			dmem_re;
    wire			dmem_we;
    wire [dmem_width-1:0] 	dmem_a;
    wire [7:0]		dmem_di;
    wire [7:0]		dmem_do;

    wire			io_re;
    wire			io_we;
    wire [5:0]		io_a;
    wire [7:0]      io_di;
    wire [7:0]		io_do;

    reg system_clk;
    
    initial begin
        system_clk = 0;
    end
    
    // clocks - memory (100MHz) is clocked 2x the system clock (50MHz)
    wire mem_clk;
    assign mem_clk = clk;
    always @(posedge mem_clk)
        system_clk = ~system_clk;
        
    // data memory, aka RAM (use ram module)        
    ram core0_ram ( mem_clk, dmem_re, dmem_we, dmem_a, dmem_di, dmem_do );
    defparam core0_ram.ram_width = dmem_width;

    // program memory, aka ROM (use flash module)
    flash core0_flash ( mem_clk, pmem_ce,pmem_a, pmem_d );
    defparam core0_flash.flash_file = "rom.bin";
    defparam core0_flash.flash_width = pmem_width;

    // I/O space, LEDs at I/O address 0x04
    wire port0_io_select = (io_a[5:0] == 6'b000100);		/* 000100: 4 */
    wire port0_io_re = (port0_io_select ? io_re : 1'b0);
    wire port0_io_we = (port0_io_select ? io_we : 1'b0);
    wire [7:0] port0_out;

    avr_io_out port0(clk, btn[0], port0_io_re, port0_io_we, io_di, io_do, port0_out);

    reg [7:0] led_msb = 0;
    assign led[15:8] = led_msb;
    assign led[7:0] = port0_out;

    // interrupt logic, for now unused
    wire iflag;
    wire [1:0] ivect;
    wire [1:0] ieack;
    wire [1:0] core0_mode;

    avr_core core0(
        system_clk, btn[0],
	    pmem_ce, pmem_a, pmem_d, 
	    dmem_re, dmem_we, dmem_a, dmem_di, dmem_do,
	    io_re, io_we, io_a, io_di, io_do,
	    iflag, ivect,
	    core0_mode,
	    ieack
     );

    defparam core0.pmem_width = pmem_width;
    defparam core0.dmem_width = dmem_width;
    defparam core0.interrupt  = 0;
    defparam core0.intr_width = 2;
    defparam core0.lsb_call = 0;

endmodule

