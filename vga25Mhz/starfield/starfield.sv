/**
 * Project: starfield
 *
 * This is the top module for the project.
 *
 */

`include "../prescaler.v"
`include "../hvsync_generator.v"
`include "lfsr.sv"

module starfield (
    input  clk,             // 25MHz clock
    input  btnC,            // reset button (center button on Basys3)
    output [3:0] vgaRed,    // VGA display signals
    output [3:0] vgaBlue,
    output [3:0] vgaGreen,
    output Hsync,
    output Vsync
);

    // zero out the top VGA color bits
    /*
    assign vgaRed[2:0] = 0;
    assign vgaGreen[2:0] = 0;
    assign vgaBlue[2:0] = 0;
    */

    // 25MHz clock
    wire CLK_25MHz;
    prescaler #(.N(2)) ps2(clk, CLK_25MHz);
      
    wire display_on;
    wire [9:0] hpos;
    wire [9:0] vpos;
    wire [15:0] lfsr;

    hvsync_generator hvsync_gen(
        .clk(CLK_25MHz),
        .reset(btnC),
        .hsync(Hsync),
        .vsync(Vsync),
        .display_on(display_on),
        .hpos(hpos),
        .vpos(vpos)
    );

    // enable LFSR only in 512x512 area
    wire star_enable = !hpos[9] & !vpos[9];

    // LFSR with period = 2^16-1 = 256*256-1
    LFSR #(16'b1000000001011,0) lfsr_gen(
        .clk(CLK_25MHz),
        .reset(btnC),
        .enable(star_enable),
        .lfsr(lfsr)
    );

    wire star_on = &lfsr[15:9]; // all 7 bits must be set
    //assign rgb = display_on && star_on ? lfsr[2:0] : 0;
    assign vgaRed[0]    = display_on && star_on ? lfsr[2] : 0;
    assign vgaRed[1]    = display_on && star_on ? lfsr[2] : 0;
    assign vgaRed[2]    = display_on && star_on ? lfsr[2] : 0;
    assign vgaRed[3]    = display_on && star_on ? lfsr[2] : 0;
    assign vgaGreen[0]  = display_on && star_on ? lfsr[1] : 0;
    assign vgaGreen[1]  = display_on && star_on ? lfsr[1] : 0;
    assign vgaGreen[2]  = display_on && star_on ? lfsr[1] : 0;
    assign vgaGreen[3]  = display_on && star_on ? lfsr[1] : 0;
    assign vgaBlue[0]   = display_on && star_on ? lfsr[0] : 0;
    assign vgaBlue[1]   = display_on && star_on ? lfsr[0] : 0;
    assign vgaBlue[2]   = display_on && star_on ? lfsr[0] : 0;
    assign vgaBlue[3]   = display_on && star_on ? lfsr[0] : 0;

endmodule

