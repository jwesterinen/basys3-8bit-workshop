// Force error when implicit net has no type.
`default_nettype none

module cpu16_tb;

    // Inputs
    reg clk = 1;
    reg reset = 0;
    reg hold = 0;
    reg [15:0] dataIn;

    // Output
    wire busy;
    wire [15:0] address;
    wire [15:0] dataOut;
    wire write;

    // Instantiate DUT (device under test)
    CPU16 cpu_test(clk, reset, hold, busy, address, dataIn, dataOut, write);

    initial
        forever #1 clk = ~clk;

    // Main testbench code
    initial begin
        $monitor($time, ": clk = %b, busy = %b, write = %b, address = %x, dataIn = %x, dataOut = %x", 
            clk, busy, write, address, dataIn, dataOut);
        $dumpfile("cpu16.vcd");
        $dumpvars(0, cpu16_tb);

        // reset
        reset <= 1;
        
        // canonical test with NOP on the data input bus and free spinning clock
        // nop
        #2 reset <= 0; dataIn <= 0xea;
        
        #300

        $finish;
    end

endmodule

