// Force error when implicit net has no type.
`default_nettype none

//`include "../system_avr.v"

module system_avr_tb;

    // Inputs
    reg clk = 1;
//    reg [15:0] switches;
    reg [4:0] buttons;

    // Output
    wire [7:0] leds;
//    wire [6:0] segments;
//    wire decimal_point;
//    wire [3:0] anode;
//    wire signal_out;

    // Instantiate DUT (device under test)
    //system_avr system_avr_test(clk, switches, buttons, leds, segments, decimal_point, anode, signal_out);
    system_avr system_avr_test(clk, buttons, leds);

    initial
        forever #1 clk = ~clk;

    // Main testbench code
    initial begin
        //$monitor($time, ": clk = %b, switches = %x, leds = %x", clk, switches, leds);
        $monitor($time, ": clk = %b, leds = %x", clk, leds);
        $dumpfile("system_avr.vcd");
        $dumpvars(0, system_avr_tb);

        // reset
        buttons <= 5'b0001;
        #2 buttons <= 5'b0000;

        // run the clock to allow the core to fetch the program from the ROM
        #20

        $finish;
    end

endmodule
