/**
 * Module: vgachar
 *
 * This module is the guts of the terminal peripheral.
 *
 */

`include "../prescaler.v"
`include "../hvsync_generator.v"
`include "font437array.v"

module vgachar(ck100, dstb, rstb, cstb, data, curvis, curblk, fgclr, bgclr, underln,
                currow, curcol, charrc, attrrc, hsync, vsync, red, green, blue);
    input  ck100;           // 100 MHz clock
    input  dstb;            // strobe to latch /data/ char and attr on rising edge
    input  rstb;            // Set cursor row to /data/ on rising edge of rstb
    input  cstb;            // Set cursor column to /data/ on rising edge of cstb
    input  [7:0] data;      // character data, X or Y location input
    input  curvis;          // cursor visible if ==1
    input  curblk;          // cursor block if ==1, else underline
    input  [11:0] fgclr;    // foreground color 4/4/4 for r/g/b
    input  [11:0] bgclr;    // background color 4/4/4 for r/g/b
    input  underln;         // underline characters added if ==1
    output [4:0] currow;    // current row location of cursor (30 rows)
    output [6:0] curcol;    // current column location of cursor (80 cols)
    output [7:0] charrc;    // character under cursor
    output [24:0] attrrc;   // attributes of character under cursor
    output hsync;           // VGA horizontal sync
    output vsync;           // VGA vertical sync
    output [3:0] red;       // VGA red value
    output [3:0] green;     // VGA green value
    output [3:0] blue;      // VGA blue value

    // 25MHz clock
    wire ck25;
    prescaler #(.N(2)) ps2(ck100, ck25);
    
    wire display_on;
    wire [9:0] hpos;
    wire [9:0] vpos;

    hvsync_generator hvsync_gen(
        .clk(ck25),
        .reset(0),
        .hsync(hsync),
        .vsync(vsync),
        .display_on(display_on),
        .hpos(hpos),
        .vpos(vpos)
    );
  
    // current row and column values
    reg [4:0] cursorRow;
    reg [6:0] cursorCol;
    initial begin
        cursorRow = 0;
        cursorCol = 0;
    end
    
    // latch cursor row and column
    always @(posedge rstb) begin
        cursorRow <= data[4:0];
    end
    always @(posedge cstb) begin
        cursorCol <= data[6:0];
    end

    // video buffer contains 30x80 character codes and attributes: {underline, fgColor, bgColor, charCode}
    reg [33:0] videoBuf[29:0][79:0];    
    integer row, col;  
    initial begin
        // initialize the display buffer to all blank except for the cursor
        videoBuf[0][0] = 34'h0FFFFFFDB;  // {0, 0000/1000/0000, 0000/0000/0000, <block char>}
        for (col = 1; col < 80; col=col+1)
            videoBuf[0][col] = 0; 
        for (row = 1; row < 30; row=row+1)
            for (col = 0; col < 80; col=col+1)
                videoBuf[row][col] = 0; 
    end

    // latch character code and attributes into video buffer:
    //   [32]    - underline
    //   [31:20] - fg color: [31:28] = red, [27:24] = green, [23:20] = blue
    //   [19:8]  - bg color: [19:16] = red, [15:12] = green, [11:8] = blue
    //   [7:0]   - character code
    always @(posedge dstb) begin
        // set the character code and attributes into the video buffer at the cursor position then increment the cursor
        videoBuf[cursorRow][cursorCol] <= {underln, fgclr, bgclr, data};        
        cursorCol <= (cursorCol < 79) ? cursorCol + 1 : 0;
        if (cursorCol == 79) begin
            cursorRow <= (cursorRow < 29) ? cursorRow + 1 : 0;
        end
    end

    // the char to be displayed is taken from the lower 8 bits of the indexed video buffer element
    wire [7:0] char = videoBuf[vpos[9:4]][hpos[9:3]][7:0];
    
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

    // the font is defined as a bitmap
    //  - if a bit is 1 in the slice, output the fg color
    //  - if a bit is 0 in the slice, output the bg color
    // only display the defined bits of the chars by restricting the index into the bit slice
    assign red   = display_on && (((xofs ^ 3'b111) < 8) && (yofs < 12)) ? (bits[xofs ^ 3'b111] ? videoBuf[cursorRow][cursorCol][31:28] : videoBuf[cursorRow][cursorCol][19:16]) : 0;
    assign green = display_on && (((xofs ^ 3'b111) < 8) && (yofs < 12)) ? (bits[xofs ^ 3'b111] ? videoBuf[cursorRow][cursorCol][27:24] : videoBuf[cursorRow][cursorCol][15:12]) : 0;
    assign blue  = display_on && (((xofs ^ 3'b111) < 8) && (yofs < 12)) ? (bits[xofs ^ 3'b111] ? videoBuf[cursorRow][cursorCol][23:20] : videoBuf[cursorRow][cursorCol][11:8]) : 0;

    // outputs
    assign currow = cursorRow;
    assign curcol = cursorCol;
    assign charrc = videoBuf[cursorRow][cursorCol][7:0];
    assign attrrc = videoBuf[cursorRow][cursorCol][32:8];

endmodule
