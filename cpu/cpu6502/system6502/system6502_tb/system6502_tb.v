// Force error when implicit net has no type.
`default_nettype none

module system6502_tb;

    // Inputs
    reg clk = 1;
    reg [15:0] switches;
    reg [4:0] buttons;

    // Output
    wire [15:0] leds;
    wire [6:0] segments;
    wire decimal_point;
    wire [3:0] anode;

    // Instantiate DUT (device under test)
    system6502 system6502_test(clk, switches, buttons, leds, segments, decimal_point, anode);

    initial
        forever #1 clk = ~clk;

    // Main testbench code
    initial begin
        $monitor($time, ": clk = %b, switches = %x, leds = %x", clk, switches, leds);
        $dumpfile("system6502.vcd");
        $dumpvars(0, system6502_tb);

        // reset
        buttons[0] <= 1; switches <= 'h1234;

        #2 buttons[0] <= 0;
        #150

        $finish;
    end

endmodule
