/*
 *  Display buffer module for DP terminal project.
 *
 *  This buffer is implemented as a dual port RAM with synchronous read.
 *
 */
 
 module display_buffer(clk, we, wr, wc, wd, rr, rc, rd);
    input   clk;        // system clock
    input   we;         // write strobe
    input   [4:0] wr;   // write row
    input   [6:0] wc;   // write col
    input   [32:0] wd;  // write data
    input   [4:0] rr;   // read row
    input   [6:0] rc;   // read col
    output  [32:0] rd;  // read data
 
    // video buffer contains 30x80 character codes and attributes: {underline, fgColor, bgColor, charCode}
    reg [32:0] rdReg;
    //reg [32:0] ram[29:0][79:0];  
    reg [32:0] ram[2399:0];  
      
    integer row, col;    
    initial begin
        // initialize each char in the display buffer to {<no underline>, <white fg>, <black bg>, <blank>)
        for (row = 0; row < 30; row=row+1) begin
            for (col = 0; col < 80; col=col+1) begin
                //ram[row][col] = 33'h0F0000F00 + col + 80 + 33'h100000000; //33'h0FFF00000; 
                ram[(row * 80) + col] = 33'h0F0000F00 + col + 80 + 33'h100000000; //33'h0FFF00000; 
            end
        end
    end

    always@(posedge clk)
    begin
/*    
        if (we)
            ram[wr][wc] <= wd;
        rdReg <= ram[rr][rc];
*/
        if (we)
            ram[(wr * 80) + wc] <= wd;
        rdReg <= ram[(rr * 80) + rc];
    end

    assign rd = rdReg;

endmodule

