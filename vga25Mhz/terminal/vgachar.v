/**
 * Module: vgachar
 *
 * This module is the guts of the terminal peripheral.
 *
 */

`include "prescaler.v"
`include "hvsync_generator.v"
`include "font437array.v"

module vgachar(ck100, data, dstrobe, dtype, curvis, curblk, fgclr, bgclr, underln,
                currow, curcol, charrc, attrrc, hsync, vsync, red, green, blue);
    input  ck100;           // 100 MHz clock
    input  [7:0] data;      // data to latch
    input  dstrobe;         // positive edge will latch in data
    input  [1:0] dtype;     // data type: 0 = char, 1 = column, and 2 = row
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
        cursorRow = 15; //0;
        cursorCol = 40; //0;
    end
    
    // video buffer contains 30x80 character codes and attributes: {underline, fgColor, bgColor, charCode}
    reg [32:0] displayBuf[29:0][79:0];    
    integer row, col;  
    initial begin
        // initialize each char in the display buffer to {<no underline>, <white fg>, <black bg>, <blank>)
        for (row = 0; row < 30; row=row+1) begin
            for (col = 0; col < 80; col=col+1) begin
                displayBuf[row][col] = 33'h0F0000F00 + col + 80 + 33'h100000000; //33'h0FFF00000; 
            end
        end
    end

    // latch data defined by dtype using dstrobe
    always @(posedge dstrobe) begin
        case (dtype)
            0: begin
                // character code and attributes in video buffer:
                //   [32]    - underline
                //   [31:20] - fg color: [31:28] = red, [27:24] = green, [23:20] = blue
                //   [19:8]  - bg color: [19:16] = red, [15:12] = green, [11:8] = blue
                //   [7:0]   - character code
                // set the character code and attributes into the video buffer at the cursor position
                displayBuf[cursorRow][cursorCol] <= {underln, fgclr, bgclr, data};   
                
                // increment the cursor location
                cursorCol <= (cursorCol < 79) ? cursorCol + 1 : 0;
                if (cursorCol == 79) begin
                    cursorRow <= (cursorRow < 29) ? cursorRow + 1 : 0;
                end
            end
            
            1: begin
                // latch the cursor column
                cursorCol <= data[6:0];
            end
            
            2: begin
                // latch the cursor row
                cursorRow <= data[4:0];
            end
        endcase
    end

    // the char to be displayed is:
    //   - the cursor if the current location coincides with the cursor location and the cursor is visible
    //   - otherwise the char in the display buffer, i.e. the lower 8 bits of the indexed display buffer element
    wire [7:0] char = (vpos[8:4] == cursorRow && hpos[9:3] == cursorCol && curvis) ? ((curblk == 1) ? 8'hDB : 8'h5F) : displayBuf[vpos[8:4]][hpos[9:3]][7:0];
    
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

    // the character is displayed as follows:
    //  - if a bit is 1 in the slice, output the fg color
    //  - if a bit is 0 in the slice, output the bg color
    //  - if underline is true the 11th slice of the font will be all foreground color
    //  - the 14th thru 16th slice will be the background color of the char
    assign red   = (display_on) ? ((yofs < 12 && bits[xofs ^ 3'b111]) || (yofs == 10 && displayBuf[vpos[8:4]][hpos[9:3]][32])) ? displayBuf[vpos[8:4]][hpos[9:3]][31:28] : displayBuf[vpos[8:4]][hpos[9:3]][19:16] : 0;
    assign green = (display_on) ? ((yofs < 12 && bits[xofs ^ 3'b111]) || (yofs == 10 && displayBuf[vpos[8:4]][hpos[9:3]][32])) ? displayBuf[vpos[8:4]][hpos[9:3]][27:24] : displayBuf[vpos[8:4]][hpos[9:3]][15:12] : 0;
    assign blue  = (display_on) ? ((yofs < 12 && bits[xofs ^ 3'b111]) || (yofs == 10 && displayBuf[vpos[8:4]][hpos[9:3]][32])) ? displayBuf[vpos[8:4]][hpos[9:3]][23:20] : displayBuf[vpos[8:4]][hpos[9:3]][11:8]  : 0;

    // outputs
    assign currow = cursorRow;
    assign curcol = cursorCol;
    assign charrc = displayBuf[cursorRow][cursorCol][7:0];
    assign attrrc = displayBuf[cursorRow][cursorCol][32:8];

endmodule
