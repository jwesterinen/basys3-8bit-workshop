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
    input  btnC             // reset button (center button on Basys3)
);

    // 25MHz clock
    wire CLK_25MHz;
    prescaler #(.N(2)) ps2(clk, CLK_25MHz);
    
    // test pattern object
    <project-specific top module> test(CLK_25MHz, btnC);    

endmodule
