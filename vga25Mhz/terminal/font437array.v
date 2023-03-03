/**
 *
 * This module contains the array that holds the codepage 437 font
 *
 */

module font437_array(char, yofs, bits);
  
    input [255:0] char; // char 0-255
    input [3:0] yofs;   // vertical offset (0-11)
    output [7:0] bits;  // output (8 bits)

    // font array (256 x 8 x 12 bits)
    reg [7:0] font437[255:0][11:0];

    initial begin
        `include "font437.v"
    end

    assign bits = font437[char][yofs];
  
endmodule


