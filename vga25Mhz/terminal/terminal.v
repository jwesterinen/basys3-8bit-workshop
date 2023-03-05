/**
 * Project: terminal
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
    wire [9:0] hpos;
    wire [9:0] vpos;

    hvsync_generator hvsync_gen(
        .clk(CLK_25MHz),
        .reset(btnC),
        .hsync(Hsync),
        .vsync(Vsync),
        .display_on(display_on),
        .hpos(hpos),
        .vpos(vpos)
    );
  
    // video buffer contains 80x40 character codes
    reg [7:0] videoBuf[0:(80*40)-1];
    
    integer i,j;
  
    initial begin
        // init the video buffer
        videoBuf[0] = 72;   // 'H'
        videoBuf[1] = 101;  // 'e'
        videoBuf[2] = 108;  // 'l'
        videoBuf[3] = 108;  // 'l'
        videoBuf[4] = 111;  // 'o'
        videoBuf[5] = 44;   // ','
        videoBuf[6] = 0;    // ' '
        videoBuf[7] = 87;   // 'W'
        videoBuf[8] = 111;  // 'o'
        videoBuf[9] = 114;  // 'r'
        videoBuf[10] = 108; // 'l'
        videoBuf[11] = 100; // 'd'
        videoBuf[12] = 33;  // '!'
        for (i = 13; i <= 3200; i=i+1)
            videoBuf[i] = 0; 
    end

    // QUESTION: What type of data structure should the display buffer be, list or array??
    // display row = vpos[9:4] and display col = hpos[9:3]
    wire [255:0] char = videoBuf[vpos[9:4] * 80 + hpos[9:3]];
    // OR
    // wire [255:0] char = videoBuf[vpos[9:4]][hpos[9:3]];
    
    // index of the vertical slice of the char to be displayed
    wire [3:0] yofs = vpos[3:0];
    
    // index of the horizontal slice of the char to be displayed
    wire [2:0] xofs = hpos[2:0];
  
    // horizontal 8-bit slice of the char to be displayed
    wire [7:0] bits;
    
    font437_array numbers(
        .char(char),
        .yofs(yofs),
        .bits(bits)
    );

    // zero out the top VGA color bits
    assign vgaRed = 0;
    assign vgaGreen[2:0] = 0;
    assign vgaBlue = 0;

    // only display the defined bits of the chars by restricting the index into bits
    assign vgaGreen[3] = display_on && (((xofs ^ 3'b111) < 8) && (yofs < 12)) ? bits[xofs ^ 3'b111] : 0;

endmodule
