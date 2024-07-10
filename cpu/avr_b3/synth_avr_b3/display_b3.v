/*
*   basys3_display.v
*
*   Description:
*       This module contains control logic for the Basys3 four 7-segment displays and
*   decimal point.  It contains a buffer for each of the four 7-segment displays, and 
*   a control register to specify the mode of display.  Character codes are written into
*   the display buffers which causes patterns to be shown on the displays. There are two
*   modes of display, raw, and pattern display.  In raw mode (status register = 1) the 
*   bits in the character code directly specify which segments to be illuminated, as follows:
* 
*             1                               Basys3 Display
*          -------           ----------------------------------------------
*         |       |         |           ---    ---    ---    ---           |
*       6 |       | 2       |          | | |  | | |  | | |  | | |          |
*         |   7   |         |           ---    ---    ---    ---           |
*           -----           |          | | |  | | |  | | |  | | |          |
*         |       |         |           --- .  --- .  --- .  --- .         |
*       5 |       | 3       |            4      3      2      1            |
*         |       |          ----------------------------------------------
*           -----   .  
*             4     DP 
*
*   The display register bits correspond to the one-based bit position, e.g. the LSB is 
*   bit 1 and corresponds to segment 1 and the MSB is bit 7 and correspoonds to segment 7.
*   Bit 8 is alway 0.  If the bit contains '0' the segment is on; '1' means off.  In pattern
*   mode (status register = 0x00), the character codes will be used as an index to a 
*   table of patterns, as follows:
*       0x00 - 0x0f:    '0', '1', ... 'E', 'F'
*       0x10:           <blank>
*       0x11 - 0x17:    '-', 'J', 'L', 'P', 'S', 'U', 'c'
        18:             2 leftmost and 2 rightmost segs
        19:             top & bottom segs
        1A:             inverted lower case c
        1B:             inverted C
        1C:             3 horiz segs
*
*/
 
module display_b3(
    input clk,                  // system clock
    input display_ctrl,         // display control reg
    input [7:0] display_buf0,   // display buffer 0
    input [7:0] display_buf1,   // display buffer 1
    input [7:0] display_buf2,   // display buffer 2
    input [7:0] display_buf3,   // display buffer 3
    input [3:0] dp_buf,         // decimal point buffer
    output [6:0] seg,           // display segments
    output dp,                  // display decimal point
    output [3:0] an             // display select
);
    wire [7:0] display_buf [0:3];
    assign display_buf[0] = display_buf0;
    assign display_buf[1] = display_buf1;
    assign display_buf[2] = display_buf2;
    assign display_buf[3] = display_buf3;
    
    // display patterns
    reg [7:0] display_pats[0:31];
        
    initial begin
        display_pats[5'h00] <= 7'b1000000;    // '0'
        display_pats[5'h01] <= 7'b1111001;    // '1'
        display_pats[5'h02] <= 7'b0100100;    // '2'
        display_pats[5'h03] <= 7'b0110000;    // '3'
        display_pats[5'h04] <= 7'b0011001;    // '4'
        display_pats[5'h05] <= 7'b0010010;    // '5'
        display_pats[5'h06] <= 7'b0000010;    // '6'
        display_pats[5'h07] <= 7'b1111000;    // '7'
        display_pats[5'h08] <= 7'b0000000;    // '8'
        display_pats[5'h09] <= 7'b0011000;    // '9'
        display_pats[5'h0a] <= 7'b0001000;    // 'A'
        display_pats[5'h0b] <= 7'b0000011;    // 'b'
        display_pats[5'h0c] <= 7'b1000110;    // 'C'
        display_pats[5'h0d] <= 7'b0100001;    // 'd'
        display_pats[5'h0e] <= 7'b0000110;    // 'E'
        display_pats[5'h0f] <= 7'b0001110;    // 'F'
        display_pats[5'h10] <= 7'b1111111;    // blank
        display_pats[5'h11] <= 7'b0111111;    // '-'
        display_pats[5'h12] <= 7'b1100001;    // 'J'
        display_pats[5'h13] <= 7'b1000111;    // 'L'
        display_pats[5'h14] <= 7'b0001100;    // 'P'
        display_pats[5'h15] <= 7'b0010010;    // 'S'
        display_pats[5'h16] <= 7'b1000001;    // 'U'
        display_pats[5'h17] <= 7'b0100111;    // 'c'
        display_pats[5'h18] <= 7'b1001001;    // <2 leftmost and 2 rightmost segs>
        display_pats[5'h19] <= 7'b1110110;    // <top & bottom segs>
        display_pats[5'h1A] <= 7'b0110011;    // <inverted lower case c>
        display_pats[5'h1B] <= 7'b1110000;    // <inverted C>
        display_pats[5'h1C] <= 7'b0110110;    // <3 horiz segs>
    end

    // display clock derivation
    // scale the input clock to ~500Hz (~2ms period)
    wire display_clk;
    prescaler #(.N(16)) ps16(clk, display_clk);

    // display selection   
    reg [1:0] display_index = 0;
    always @(posedge display_clk)
        display_index <= display_index + 1;
    
    // display segment value
    assign seg = (display_ctrl == 0) ? display_pats[display_buf[display_index]]:  // pattern display
                                       ~(display_buf[display_index]);             // raw display
                                       
    assign dp = ~(dp_buf[display_index]);                                       
                              
    // display selection                          
    assign an = 
        (display_index == 2'h0) ? 4'b0111 :  // leftmost display
        (display_index == 2'h1) ? 4'b1011 :  // next to leftmost display
        (display_index == 2'h2) ? 4'b1101 :  // next to rightmost display
                                  4'b1110 ;  // rightmost display

endmodule
