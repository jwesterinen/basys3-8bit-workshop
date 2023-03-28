/**
 *
 * This module contains the codepage 437 font ROM.
 *
 */

module font437_rom(clk, char, yofs, bits);

    input clk;          // clock  
    input [7:0] char;   // char 0-255
    input [3:0] yofs;   // vertical offset (0-11)
    output [7:0] bits;  // output (8 bits)

    // font array (256 x 12 x 8 bits)
    reg [7:0] font437[255:0][11:0];
    reg [7:0] bitsReg;
    
    initial begin
        `include "font437.v"
    end

    always @ (posedge clk) begin
        bitsReg <= font437[char][yofs];
    end
    
    assign bits = bitsReg;   
    //assign bits = font437[char][yofs];
  
endmodule


