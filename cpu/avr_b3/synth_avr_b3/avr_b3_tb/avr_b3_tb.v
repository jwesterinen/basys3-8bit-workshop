// This tests the keypad.  Change the prescaler
// divide in PmodKYPD.v from 20 to 5 before running
// the simulation.  

`default_nettype none
`include "../sysdefs.h"
`timescale 1ns/1ns
 
module avr_b3_tb;

    // Inputs and outputs
    reg clk_in = 1;
    reg [15:0] sw;
    reg [4:0] btn;
    wire [15:0] led;
    wire [6:0] seg;
    wire dp;
    wire [3:0] an;
    wire [3:0] JBU;
    reg  [3:0] JBL;
    wire JA7;
    wire JC0;
    wire JC1;
    wire JC2;
    wire JC3;
    reg  RsRx;
    wire RsTx;
    wire PS2Clkout;
    reg  PS2Clkin;
    wire PS2Dataout;
    reg  PS2Datain;
    wire [3:0] vgaBlue;
    wire [3:0] vgaGreen;
    wire [3:0] vgaRed;
    wire Vsync;
    wire Hsync;
    integer i;
 
    avr_b3 test_system
    (
        clk_in, // 100MHz input clock
        sw, // switches
        btn, // buttons - C (00001), U (00010), L (00100), R (01000), D (10000)
        led,// LEDs
        seg, // display segments
        dp,        // decimal point
        an,  // display anode
        JBU,   // PmodKYPD, JB[7:4] is rows, JB[3:0] is Columns
        JBL,   // PmodKYPD, JB[7:4] is rows, JB[3:0] is Columns
        JA7,       // Port JA on Basys3, on Pmod-Audio JA7 is left input (IL)
        JC0,       // Port JC0-3 used for out4 peripheral
        JC1,
        JC2,
        JC3,
        RsRx,      // UART receive signal
        RsTx,      // UART transmit signal
        PS2Clkout,    // PS2 keyboard clock
        PS2Dataout,   // PS2 keyboard data
        vgaBlue,  // VGA signals
        vgaGreen,
        vgaRed,
        Vsync,
        Hsync
    );


    always #5 clk_in = ~clk_in;

    // Main testbench code
    initial begin
        //$monitor($time, ": clk = %b, switches = %x, leds = %x", clk, sw, led);
        $monitor($time, ": leds = %x", led);
        $dumpvars(0, avr_b3_tb);
        $dumpfile("avr_b3.vcd");

        // reset (reset must last at least 2 clocks, 20 ns)
        JBL <= 4'hf;
        btn <= 5'h0;
        #20
        btn <= 5'h1f;
        #20
        btn <= 5'h0;
        #20
        sw <= 16'haa55;
        #1520

        #400000

        $finish;
    end

endmodule
