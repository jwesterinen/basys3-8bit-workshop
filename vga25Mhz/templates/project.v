/**
 * Project: <project name>
 *
 * This is the top module that serves as an adaptor for projects in the 8 Bit Workshop
 * to be used on a Basys3 board.
 *
 */

`include "../prescaler.v"
`include "<project-specific top module>.v"

module <project name> (
    input  clk,             // 100MHz clock
    input  btnC,            // reset button (center button on Basys3)
    output [3:0] vgaRed,    // VGA display signals
    output [3:0] vgaBlue,
    output [3:0] vgaGreen,
    output Hsync,
    output Vsync
);

    // zero out the top VGA color bits
    assign vgaRed[2:0] = 0;
    assign vgaBlue[2:0] = 0;
    assign vgaGreen[2:0] = 0;

    // 25MHz clock
    wire CLK_25MHz;
    prescaler #(.N(2)) ps2(clk, CLK_25MHz);
    
    // test pattern object
    <project-specific top module> test(CLK_25MHz, btnC, Hsync, Vsync, {vgaBlue[3],vgaGreen[3],vgaRed[3]});    

endmodule
