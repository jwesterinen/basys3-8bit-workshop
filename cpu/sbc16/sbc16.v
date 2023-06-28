/**
 * Project: sbc16
 *
 * This is the top module that serves as an adaptor for projects in the 8 Bit Workshop
 * to be used on a Basys3 board.
 *
 */

`include "../prescaler.v"
`include "system16.v"

module sbc16 (
    input  clk,         // 100MHz clock
    input  btnC,        // reset button (center button on Basys3)
    input [15:0] sw,    // canonical I/O
    output [15:0] led
);

    // 25MHz clock
    wire CLK_25MHz;
    prescaler #(.N(2)) ps2(clk, CLK_25MHz);
    
    // test pattern object
    system16 sbc(CLK_25MHz, btnC, sw, led);    

endmodule
