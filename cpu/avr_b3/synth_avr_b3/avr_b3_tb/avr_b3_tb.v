// Force error when implicit net has no type.
`default_nettype none

`include "../sysdefs.h"
 
module avr_b3_tb;

    // Inputs
    reg clk = 1;
    reg [15:0] switches;
    reg [4:0] buttons;
    reg RsRx;
    reg PS2Clk;
    reg PS2Data;

    // Output
    wire [15:0] leds;
    wire [6:0] segments;
    wire decimal_point;
    wire [3:0] anode;
    wire signal_out;
    wire [3:0] out4_pins;
    wire RsTx;
    wire [3:0] vgaBlue;
    wire [3:0] vgaGreen;
    wire [3:0] vgaRed;
    wire Vsync;
    wire Hsync;

    // Instantiate DUT (device under test)
    // FIXME: simulator doesn't like inout ports
    //system_avr system_avr_test(clk, switches, buttons, leds, segments, decimal_point, anode, {rows,cols}, signal_out);
    avr_b3 test_system
    (
        clk, 
        switches, buttons, leds, segments, decimal_point, anode, 
        signal_out, 
        out4_pins[0], out4_pins[1], out4_pins[2], out4_pins[3], 
        RsRx, RsTx,
        PS2Clk, PS2Data,
        vgaBlue, vgaGreen, vgaRed, Vsync, Hsync
    );

    initial
        forever #1 clk = ~clk;

    // Main testbench code
    initial begin
        //$monitor($time, ": clk = %b, switches = %x, leds = %x", clk, switches, leds);
        $monitor($time, ": leds = %x, out4_pins = %x", leds, out4_pins);
        $dumpfile("avr_b3.vcd");
        $dumpvars(0, avr_b3_tb);

        // reset (reset must last at least 2 clocks)
        buttons <= 5'b0001;
        #4 buttons <= 5'b0000; switches <= 16'hdbaa;

        // run the clock to allow the core to fetch the program from the ROM
        #5000

        $finish;
    end

endmodule
