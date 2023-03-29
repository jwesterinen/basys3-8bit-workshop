/*
 *  Module: display_buffer.v
 *
 *  This module provides a display buffer for the terminal peripheral which implemented 
 *  as a dual port RAM with synchronous read and contains 40 rows of 80 character codes 
 *  and attributes: {underline, fgColor, bgColor, charCode}. It provides synchronization 
 *  for the reading of the RAM which allows the compiler to use block RAM as the 
 *  implementation of the ROM structure.
 *
 *  Note: This algorithm uses multiplication which is expensive and could be replaced with the use
 *  of a list of strides indexed by the row number which when added to the column yields the buffer 
 *  location.
 *
 */
 
 module display_buffer(clk, we, wr, wc, wd, rr, rc, rd);
    input   clk;        // system clock
    input   we;         // write strobe
    input   [5:0] wr;   // write row
    input   [6:0] wc;   // write col
    input   [32:0] wd;  // write data
    input   [5:0] rr;   // read row
    input   [6:0] rc;   // read col
    output  [32:0] rd;  // read data
 
    reg [32:0] rdReg;
    reg [32:0] ram[3199:0]; // 80x40
      
    integer row, col;   
    integer char;
    initial begin
        // initialize each char in the display buffer to {<no underline>, <white fg>, <black bg>, <blank>)
`ifdef REAL_INIT        
        for (row = 0; row < 40; row=row+1) begin
            for (col = 0; col < 80; col=col+1) begin
                ram[(row * 80) + col] = 33'h0FFF00000; // default initialization
            end
        end
`else 
        // continually draw all characters in the code page for 20 rows the last 10 rows are underlined      
        for (row = 0, char = 0; row < 20; row=row+1) begin
            for (col = 0; col < 80; col=col+1) begin
                ram[(row * 80) + col] = 33'h0F0000F00 + char + ((row < 10) ? 0 : 33'h100000000);
                char = char + 1;
                if (char >= 256)
                    char = 0;
            end
        end
        
        // draw the pseudo-graphics characters to see how they align and fit together
        for (row = 20; row < 40; row=row+1) begin
            for (col = 0; col < 80; col=col+1) begin
                ram[(row * 80) + col] = 33'h0F0000F00 + (176 + col);
            end
        end
    end
`endif    

    always@(posedge clk)
    begin
        if (we)
            ram[(wr * 80) + wc] <= wd;
        rdReg <= ram[(rr * 80) + rc];
    end

    assign rd = rdReg;

endmodule

