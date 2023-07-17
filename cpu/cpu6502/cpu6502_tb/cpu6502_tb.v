// Force error when implicit net has no type.
`default_nettype none

// test module
module cpu6502_tb;

    // inputs
    reg clk = 1;
    reg reset = 0;
    reg [7:0] DI;       // data in, read bus
    reg IRQ = 0;        // interrupt request
    reg NMI = 0;        // non-maskable interrupt request
    reg RDY = 1;        // Ready signal. Pauses CPU when RDY=0 
    
    // outputs
    wire [15:0] AB;     // address bus
    wire [7:0] DO;      // data out, write bus
    wire WE;            // write enable
    
    cpu6502 cpu( clk, reset, AB, DI, DO, WE, IRQ, NMI, RDY );

    initial
        forever #1 clk = ~clk;

    // Main testbench code
    initial begin
        $monitor($time, ": clk = %b, reset = %b, WE = %b, AB = %x, DI = %x, DO = %x", 
            clk, reset, WE, AB, DI, DO);
        $dumpfile("cpu6502.vcd");
        $dumpvars(0, cpu6502_tb);

        // reset
        reset <= 1;
        
        // canonical test with NOP on the data input bus and free spinning clock
        
        // reset vector @0xfffc = 0xe000
        #2 reset <= 0;
        #8 DI <= 'b00000000;
        #2 DI <= 'b11100000;
        
        // nop
        #2 DI <= 'b11101010;
        #30

        $finish;
    end

  
endmodule

