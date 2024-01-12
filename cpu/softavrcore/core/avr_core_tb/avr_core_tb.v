// Force error when implicit net has no type.
`default_nettype none

module avr_core_tb;

    reg  clk = 1;
    reg  rst = 0;

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
    
    // Instantiate DUT (device under test)
    avr_core #(.interrupt(0)) avr_core_test(clk, rst, pmem_ce, pmem_a, pmem_d, dmem_re, dmem_we, dmem_a, dmem_di, dmem_do, io_re, io_we, io_a, io_di, io_do, in_iflag, in_ivect, mode, in_ieack);

    initial
        forever #1 clk = ~clk;

    // Main testbench code
    initial begin
        $monitor($time, ": clk = %b, pmem_ce = %b, pmem_a = %x, pmem_d = %x", clk, pmem_ce, pmem_a, pmem_d);
        $dumpfile("avr.vcd");
        $dumpvars(0, avr_core_tb);

        // canonical test with NOP on the data input bus and free spinning clock
        
        // reset
        rst <= 1; pmem_d <= 16'h0000;
        #2 rst <= 0;

        // free run the clock        
        #300

        $finish;
    end

endmodule

