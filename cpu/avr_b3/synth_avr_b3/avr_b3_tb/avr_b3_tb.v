// Force error when implicit net has no type.
`default_nettype none

module avr_b3_tb;

    // Inputs
    reg clk = 1;
    reg [15:0] switches;
    reg [4:0] buttons;
    reg [3:0] rows;             // rows on KYPD
    reg RsRx;

    // Output
    wire [15:0] leds;
    wire [6:0] segments;
    wire decimal_point;
    wire [3:0] anode;
    wire signal_out;
    wire [3:0] cols;            // columns on KYPD
    wire RsTx;

    // Instantiate DUT (device under test)
    // FIXME: simulator doesn't like inout ports
    //system_avr system_avr_test(clk, switches, buttons, leds, segments, decimal_point, anode, {rows,cols}, signal_out);
    top avr_b3(clk, switches, buttons, leds, segments, decimal_point, anode, RsRx, RsTx);

    initial
        forever #1 clk = ~clk;

    // Main testbench code
    initial begin
        //$monitor($time, ": clk = %b, switches = %x, leds = %x", clk, switches, leds);
        $monitor($time, ": clk = %b, leds = %x", clk, leds);
        $dumpfile("avr_b3.vcd");
        $dumpvars(0, avr_b3_tb);

        // reset (reset must last at least 2 clocks)
        buttons <= 5'b0001;
        #4 buttons <= 5'b0000; switches <= 16'h3456;

        // run the clock to allow the core to fetch the program from the ROM
        #200

        $finish;
    end

endmodule
