/**
 * Project: Bitmapped Digits
 *
 * This module is the proof of concept of a Demand Peripherals "terminal" peripheral.
 *
 */

`include "../prescaler.v"
`include "../hvsync_generator.v"
`include "font437array.v"

module terminal (
    input  clk,             // 25MHz clock
    input  btnC,            // reset button (center button on Basys3)
    output [3:0] vgaRed,    // VGA display signals
    output [3:0] vgaBlue,
    output [3:0] vgaGreen,
    output Hsync,
    output Vsync
);

    // 25MHz clock
    wire CLK_25MHz;
    prescaler #(.N(2)) ps2(clk, CLK_25MHz);
    
    wire display_on;
    wire [8:0] hpos;
    wire [8:0] vpos;

    hvsync_generator hvsync_gen(
        .clk(CLK_25MHz),
        .reset(btnC),
        .hsync(Hsync),
        .vsync(Vsync),
        .display_on(display_on),
        .hpos(hpos),
        .vpos(vpos)
    );
  
    wire [255:0] digit = hpos[8:3];
    wire [2:0] xofs = hpos[2:0];
    wire [3:0] yofs = vpos[3:0];
    wire [7:0] bits;
  
    font437_array numbers(
        .char(digit),
        .yofs(yofs),
        .bits(bits)
    );

    // zero out the top VGA color bits
    assign vgaRed = 0;
    assign vgaGreen[2:0] = 0;
    assign vgaBlue = 0;

    // only display the defined bits of the digits by restricting the index into bits
    assign vgaGreen[3] = display_on && (((xofs ^ 3'b111) < 8) && (yofs < 12)) ? bits[xofs ^ 3'b111] : 0;

endmodule
