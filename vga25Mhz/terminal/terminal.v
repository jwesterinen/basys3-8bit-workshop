/**
 *  Module: terminal
 *
 *  This project is the proof of concept of a Demand Peripherals "terminal" peripheral.
 *
 *  This module is the top, test module of the project.
 *
 */

`include "vgachar.v"

module terminal (
    input  clk,             // 100MHz clock
    output [3:0] vgaRed,    // VGA display signals
    output [3:0] vgaBlue,
    output [3:0] vgaGreen,
    output Hsync,
    output Vsync
);
    reg [7:0] data = 0;
    reg dataStrobe = 0;
    reg dataType = 0;
    reg cursorVisible = 1;
    reg cursorBlock = 1;
    reg [11:0] fgColor = 12'hFFF; // white
    reg [11:0] bgColor = 12'h000; // black
    reg underline = 0;
    
    wire [5:0] curRow;
    wire [6:0] curCol;
    wire [7:0] charUnderCursor;
    wire [24:0] attribUnderCursor;
    
    // test pattern object
    vgachar test(clk, data, dataStrobe, dataType, cursorVisible, cursorBlock, fgColor, bgColor, underline,
                 curRow, curCol, charUnderCursor, attribUnderCursor, Hsync, Vsync, vgaRed, vgaGreen, vgaBlue);

endmodule
