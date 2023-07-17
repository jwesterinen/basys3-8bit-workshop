 /*
 *  basic_io.v
 *
 *  Description:
 *      This module contains all of the basic, on-board I/O devices of the Basys3 board which  
 *  includes 16 switches and 5 buttons for input and 16 LEDs and four 7-segment displays plus
 *  decimal point for output.
 *
 *  The local memory map for this module is as follows:
 *      0x00: switches LSB
 *      0x01: switches MSB
 *      0x02: buttons: 00001 = btnC, 00010 = btnU, 00100 = btnL, 01000 = btnR, 10000 = btnD
 *      0x10: LEDs LSB
 *      0x11: LEDs MSB
 *      0x20: display 0
 *      0x21: display 1
 *      0x22: display 2
 *      0x23: display 3
 */
 
module basic_io_8(
    input  clk,             // system clock
    input [7:0] addr,       // 8-bit local address space
    input [7:0] data_in,    // data from CPU
    output [7:0] data_out,  // data to CPU
    input we,               // CPU write enable
    input [15:0] sw,        // switches
    input [4:0] btn,        // buttons
    output [15:0] led,      // LEDs
    output [6:0] seg,       // display segments
    output dp,              // display decimal point
    output [3:0] an         // display select
);
    // input buffers
    reg [15:0] sw_buf;
    reg [4:0] btn_buf;
    
    // output buffers
    reg [7:0] led_buf[0:1];
    reg [7:0] display_buf [0:3];
    reg [1:0] display_index = 0;
        
    integer i;
    initial begin
        for (i = 0; i < 4; i=i+1)
            display_buf[i] <= ~(7'b1000000);
    end
    
    // local address decoding for writing to IO devices
    //always @(posedge clk) begin
    always @(posedge we) begin
/*    
        if      (we & addr[7:3] == 5'h01) led_buf[addr[1:0]] <= data_in;         // write to LEDs
        else if (we & addr[7:3] == 5'h10) display_buf[addr[1:0]] <= data_in;     // write to displays
*/        
        if      (addr[7:3] == 5'h01) led_buf[addr[1:0]] <= data_in;         // write to LEDs
        else if (addr[7:3] == 5'h10) display_buf[addr[1:0]] <= data_in;     // write to displays
    end

    // local address decoding for reading from IO devices
    assign data_out = (addr == 8'h00) ? sw[7:0]  : 
                      (addr == 8'h01) ? sw[15:8] : 
                      (addr == 8'h02) ? btn      : 
                      0;

    // display selection
    always @(posedge clk)
        display_index <= display_index + 1;
    
    // attach output devices                
    assign led = {led_buf[1], led_buf[0]};
    assign seg = display_buf[display_index];
    assign an = (display_index == 2'h0) ? 4'b0111 :     // leftmost display
                (display_index == 2'h1) ? 4'b1011 :     // next to leftmost display
                (display_index == 2'h2) ? 4'b1101 :     // next to rightmost display
                                          4'b1110 ;     // rightmost display

endmodule
