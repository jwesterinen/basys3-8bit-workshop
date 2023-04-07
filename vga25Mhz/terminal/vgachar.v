/**
 * Module: vgachar
 *
 * This module is the guts of the terminal peripheral.  It consists of a VGA controller,
 * a display buffer, a font ROM, and logic to control the cursor and the character to be
 * written to the cursor position.  The VGA controller provides the current location of
 * the pixel being displayed.  The display buffer contains both the codes and
 * attributes of the characters accessed by row and column. The character codes are
 * indeces to the font ROM that contains the shape of the character. The attributes
 * of each character includes its foreground and background color as well as whether
 * the character is underlined.  Each character is 8 pixels wide by 12 pixels high
 * which means that each column of characters is defined by the absolute pixel
 * horizontal position and each row of characters is defined by the absolute pixel
 * vertical position divided by 12.  The font ROM describes the shape of each
 * character as 12 "slices" of 8 bits. The characters are drawn slice by slice in
 * the rows. If a bit is "on," e.g. = 1, the foreground color is displayed else the
 * background color is displayed. If the underline attribute is set to 1, the 12th
 * slice is replaced by all foreground color. The cursor and character control logic
 * allows the cursor's row and column to be specified as well as the character code
 * to be written to the current cursor location. The cursor can be specified to be
 * either visible or not, and if visible, it can be specified to appear as a block
 * character or an underline. If the cursor is visible, the current character that
 * resides at the same location will be drawn as the cursor character.
 *
 * Note: In order to put the display buffer and font ROM into the FPGA's block RAM,
 * the structures must be read synchronously which introduces a 2 clock delay from the
 * actual actual pixel horizontal position to what is being displayed. In order to compensate
 * for this, the font ROM receives a horizontal value that is 2 less than the pixel's
 * actual position.  This will allow the characters to be correctly drawn, but they will
 * be drawn 2 pixel locations late.  In order to conpensate for this, the VGA controller's
 * horizongtal back porch is reduced by 2 so that the characters are drawn such that the
 * current row begins at the beginning of the VGA display.
 *
 */

`include "prescaler.v"
`include "hvsync_generator.v"
`include "font437_rom.v"
`include "display_buffer.v"

module vgachar(CLK_I, ck100, data, dstrobe, dtype, curvis, curblk, fgclr, bgclr, underln,
                currow, curcol, charrc, attrrc, hsync, vsync, red, green, blue);
    input  CLK_I;           // Host interface bus clock
    input  ck100;           // 100 MHz clock
    input  [7:0] data;      // data to latch
    input  dstrobe;         // data available on positive edge of CLK_I
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

    // character code and attributes under cursor
    reg [5:0] cursorRow;    // Row location of cursor
    reg [6:0] cursorCol;    // Column location of cursor
    reg [7:0] charrcReg;    // Character at cursor location
    reg [24:0] attrrcReg;   // Attributes of char at cursor location
    reg [32:0] wrChar;      // Char and attr of character to add to buffer
    wire [32:0] rdChar;     // Char data from the buffer
    wire display_on;        // Set if raster is in the 640x480 display area
    wire [9:0] hpos;        // Horizontal location of raster
    wire [9:0] vpos;        // Vertical location of raster
    wire [3:0] yofs;        // y offset of the char to be displayed
    wire [2:0] xofs;        // x offset of char to be displayed
    wire [7:0] bits;        // horizontal 8-bit slice of the char to be displayed
    wire [7:0] char;        // the char to be displayed is:
                            // the cursor if current location matches cursor location
                            // and cursor is visible, or the char from the display buffer
    assign char = (((vpos/12) == cursorRow) && (hpos[9:3] == cursorCol && curvis)) ?
                      ((curblk == 1) ? 8'hDB : 8'h5F) : rdChar[7:0];
    assign yofs = vpos % 12; // index of the vertical slice of the char to be displayed
    assign xofs = hpos[2:0] - 1;  // -1 to add time for buffer RAM read

    // Instatiate the VGA sync generator
    hvsync_generator hvsync_gen( .clk(ck25), .reset(0), .hsync(hsync), .vsync(vsync),
        .display_on(display_on), .hpos(hpos), .vpos(vpos) );

    // Instantiate the character buffer
    display_buffer dispbuf( .wclk(CLK_I), .rclk(ck25), .we(dstrobe && (dtype == 0)),
        .wr(cursorRow), .wc(cursorCol), .wd({underln, fgclr, bgclr, data}),
        .rr(vpos/12), .rc(hpos[9:3]), .rd(rdChar) );

    // Instantiate the font ROM
    font437_rom font( .clk(ck25), .char(char), .yofs(yofs), .bits(bits) );


    initial begin
        cursorRow = 19;
        cursorCol = 40;
    end


    // latch data defined by dtype using dstrobe on host clock (CLK_I) edge
    always @(posedge CLK_I)
    begin
        if (dstrobe)
        begin
            if (dtype == 0)
            begin
                // increment the cursor location
                cursorCol <= (cursorCol < 79) ? cursorCol + 1 : 0;
                if (cursorCol == 79)
                    cursorRow <= (cursorRow < 39) ? cursorRow + 1 : 0;
            end
            if (dtype == 1)
                cursorCol <= data[6:0];
            else if (dtype == 2)
                cursorRow <= data[5:0];
        end
    end


    // latch the char and attributes under the cursor
    always@(posedge ck25)
    begin
        if ((vpos/12) == cursorRow && hpos[9:3] == cursorCol) begin
            charrcReg <= rdChar[7:0];
            attrrcReg <= rdChar[32:8];
        end
    end


    // the character is displayed as follows:
    //  - if a bit is 1 in the slice, output the fg color
    //  - if a bit is 0 in the slice, output the bg color
    //  - if underline is true the 12th slice of the font will be all foreground color
    assign red   = (display_on) ? (bits[xofs ^ 3'b111] || (yofs == 10 && rdChar[32])) ? rdChar[31:28] : rdChar[19:16] : 0;
    assign green = (display_on) ? (bits[xofs ^ 3'b111] || (yofs == 10 && rdChar[32])) ? rdChar[27:24] : rdChar[15:12] : 0;
    assign blue  = (display_on) ? (bits[xofs ^ 3'b111] || (yofs == 10 && rdChar[32])) ? rdChar[23:20] : rdChar[11:8]  : 0;

    // outputs
    assign currow = cursorRow;
    assign curcol = cursorCol;
    assign charrc = charrcReg;
    assign attrrc = attrrcReg;

endmodule
