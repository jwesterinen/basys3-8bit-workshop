// Force error when implicit net has no type.
`default_nettype none

//`include "../system16.v"

module system16_tb;

    // Inputs
    reg clk = 1;
    reg reset = 0;
    //reg hold = 0;
    reg [15:0] switches;

    // Output
    //wire busy;
    wire [15:0] leds;

    // Instantiate DUT (device under test)
    //system16 system16_test(clk, reset, hold, switches, busy, leds);
    system16 system16_test(clk, reset, switches, leds);

    initial
        forever #1 clk = ~clk;

    // Main testbench code
    initial begin
        $monitor($time, ": clk = %b, switches = %x, leds = %x", clk, switches, leds);
        $dumpfile("system16.vcd");
        $dumpvars(0, system16_tb);

        // reset
        reset <= 1; switches <= 'h1234;

        #2 reset <= 0;
        #200

        $finish;
    end

endmodule
