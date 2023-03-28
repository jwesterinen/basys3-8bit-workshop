/**
 * Module: vgachar
 *
 * This module is the guts of the terminal peripheral.
 *
 */

`include "prescaler.v"
`include "hvsync_generator.v"
`include "font437_rom.v"
`include "display_buffer.v"

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
    reg [4:0] curRow = 15;
    reg [6:0] curCol = 40;
    
    // character code and attributes under cursor
    reg [7:0] charrcReg;
    reg [24:0] attrrcReg;

    // character code and attributes
    reg [32:0] wrChar;
    wire [32:0] rdChar;
        
    display_buffer dispbuf(
        .clk(ck25),
        .we(dstrobe), 
        .wr(curRow), 
        .wc(curCol), 
        .wd(wrChar), 
        .rr(vpos[8:4]), 
        .rc(hpos[9:3]), 
        .rd(rdChar)
    );

    // latch data defined by dtype using dstrobe
    always @(posedge dstrobe) begin
        case (dtype)
            0: begin
                // character code and attributes in video buffer:
                //   [32]    - underline
                //   [31:20] - fg color: [31:28] = red, [27:24] = green, [23:20] = blue
                //   [19:8]  - bg color: [19:16] = red, [15:12] = green, [11:8] = blue
                //   [7:0]   - character code
                // write the character code and attributes into the video buffer at the cursor position
                wrChar <= {underln, fgclr, bgclr, data};   
                
                // increment the cursor location
                curCol <= (curCol < 79) ? curCol + 1 : 0;
                if (curCol == 79) begin
                    curRow <= (curRow < 29) ? curRow + 1 : 0;
                end
            end
            
            1: begin
                // latch the cursor column
                curCol <= data[6:0];
            end
            
            2: begin
                // latch the cursor row
                curRow <= data[4:0];
            end
        endcase
    end

    // latch the char and attributes under the cursor
    always@(posedge ck25)
    begin
        if (vpos[8:4] == curRow && hpos[9:3] == curCol) begin
            charrcReg <= rdChar[7:0];
            attrrcReg <= rdChar[32:8];
        end
    end
    
    // the char to be displayed is:
    //   - the cursor if the current location coincides with the cursor location and the cursor is visible
    //   - otherwise the char in the display buffer, i.e. the lower 8 bits of the indexed display buffer element
    wire [7:0] char = (vpos[8:4] == curRow && hpos[9:3] == curCol && curvis) ? ((curblk == 1) ? 8'hDB : 8'h5F) : rdChar[7:0];
    
    // index of the vertical slice of the char to be displayed
    wire [3:0] yofs = vpos[3:0];
    
    // index of the horizontal slice of the char to be displayed which
    // is compensated for the 2 clocks it takes to read the character and its font
    wire [2:0] xofs = hpos[2:0] - 2;
  
    // horizontal 8-bit slice of the char to be displayed
    wire [7:0] bits;
    
    font437_rom font(
        .clk(ck25),
        .char(char),
        .yofs(yofs),
        .bits(bits)
    );

    // the character is displayed as follows:
    //  - if a bit is 1 in the slice, output the fg color
    //  - if a bit is 0 in the slice, output the bg color
    //  - if underline is true the 11th slice of the font will be all foreground color
    //  - the 14th thru 16th slice will be the background color of the char
    assign red   = (display_on) ? ((yofs < 12 && bits[xofs ^ 3'b111]) || (yofs == 10 && rdChar[32])) ? rdChar[31:28] : rdChar[19:16] : 0;
    assign green = (display_on) ? ((yofs < 12 && bits[xofs ^ 3'b111]) || (yofs == 10 && rdChar[32])) ? rdChar[27:24] : rdChar[15:12] : 0;
    assign blue  = (display_on) ? ((yofs < 12 && bits[xofs ^ 3'b111]) || (yofs == 10 && rdChar[32])) ? rdChar[23:20] : rdChar[11:8]  : 0;

    // outputs
    assign currow = curRow;
    assign curcol = curCol;
    assign charrc = charrcReg;
    assign attrrc = attrrcReg;

endmodule
