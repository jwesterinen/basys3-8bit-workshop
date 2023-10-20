 /*
 *  basic_io16.v
 *
 *  Description:
 *      This module contains all of the basic, on-board I/O devices of the Basys3 board which  
 *  includes 16 switches and 5 buttons for input and 16 LEDs and four 7-segment displays plus
 *  decimal point for output.  This module is based on a 16-bit data bus.
 *
 *  The local memory map for this module is as follows:
 *      BASE_ADDR + 0x00: switches
 *      BASE_ADDR + 0x02: buttons: 00001 = btnC, 00010 = btnU, 00100 = btnL, 01000 = btnR, 10000 = btnD
 *      BASE_ADDR + 0x10: LEDs
 *      BASE_ADDR + 0x20: display 0
 *      BASE_ADDR + 0x21: display 1
 *      BASE_ADDR + 0x22: display 2
 *      BASE_ADDR + 0x23: display 3
 *      BASE_ADDR + 0x24: display control - 0 = pattern display, !0 = raw  display
 */
 
module basic_io_16(
    input  clk,                 // system clock
    input [15:0] addr,          // register addresses
    input [15:0] data_in,       // data input
    output [15:0] data_out,     // data output
    input we,                   // write enable
    input [15:0] sw,            // switches
    input [4:0] btn,            // buttons
    output [15:0] led,          // LEDs
    output [6:0] seg,           // display segments
    output dp,                  // display decimal point
    output [3:0] an             // display select
);
	parameter BASE_ADDR = 0;    // base address for registers
	
    // input buffers
    reg [15:0] sw_buf;
    reg [4:0] btn_buf;
    
    // output buffers
    reg [15:0] led_buf;
    reg [7:0] display_buf [0:3];
    reg [1:0] display_index = 0;
    reg display_ctrl = 0;
    
    // display patterns
    reg [7:0] display_pats[0:31];
        
    // TODO: complete this        
    initial begin
        display_pats[18'h000] <= 7'b1000000;    // '0'
        display_pats[18'h001] <= 7'b1111001;    // '1'
        display_pats[18'h002] <= 7'b0100100;    // '2'
        display_pats[18'h003] <= 7'b0110000;    // '3'
        display_pats[18'h004] <= 7'b0011001;    // '4'
        display_pats[18'h005] <= 7'b0010010;    // '5'
        display_pats[18'h006] <= 7'b0000010;    // '6'
        display_pats[18'h007] <= 7'b1111000;    // '7'
        display_pats[18'h008] <= 7'b0000000;    // '8'
        display_pats[18'h009] <= 7'b0011000;    // '9'
        display_pats[18'h00a] <= 7'b0001000;    // 'a'
        display_pats[18'h00b] <= 7'b0000011;    // 'b'
        display_pats[18'h00c] <= 7'b1000110;    // 'c'
        display_pats[18'h00d] <= 7'b0100001;    // 'd'
        display_pats[18'h00e] <= 7'b0000110;    // 'e'
        display_pats[18'h00f] <= 7'b0001110;    // 'f'
        display_pats[18'h010] <= 7'b1111111;    // blank
        display_pats[18'h011] <= 7'b0111111;    // '-'
        display_pats[18'h012] <= 7'b1100001;    // 'J'

        display_buf[0] <= 18'h010;
        display_buf[1] <= 18'h010;
        display_buf[2] <= 18'h010;
        display_buf[3] <= 18'h010;
    end
    
    // scale the input clock to ~2ms ~= 500Hz
    wire display_clk;
    prescaler #(.N(16)) ps16(clk, display_clk);
    
    // display selection
    always @(posedge display_clk)
        display_index <= display_index + 1;
    
    // local address decoding for writing to IO devices
    always @(posedge clk) begin
        if (we)
            casez (addr)
                {BASE_ADDR[15:8],8'b00010000}: led_buf <= data_in;                // write to LEDs
                {BASE_ADDR[15:8],8'b001000??}: display_buf[addr[1:0]] <= data_in; // write to displays
                {BASE_ADDR[15:8],8'b00100100}: display_ctrl <= data_in;           // write to display control reg
            endcase
    end

    // local address decoding for reading from IO devices
    assign data_out = 
        (addr == {BASE_ADDR[15:8],8'h00}) ? sw                    :    // switches
        (addr == {BASE_ADDR[15:8],8'h02}) ? {11'b00000000000,btn} :    // buttons
        (addr == {BASE_ADDR[15:8],8'h10}) ? led_buf               :    // LED buffer to read back what was written
        0;

    // LEDs
    assign led = led_buf;
    
    // display segment
    assign seg = 
        (display_ctrl == 0) ? display_pats[display_buf[display_index]]: // pattern display
                              ~(display_buf[display_index]);            // raw display
                              
    // display selection                          
    assign an = 
        (display_index == 2'h0) ? 4'b0111 :     // leftmost display
        (display_index == 2'h1) ? 4'b1011 :     // next to leftmost display
        (display_index == 2'h2) ? 4'b1101 :     // next to rightmost display
                                  4'b1110 ;     // rightmost display

endmodule
