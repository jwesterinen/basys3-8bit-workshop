 /*
 *  basic_io.v
 *
 *  Description:
 *      This module contains all of the basic, on-board I/O devices of the Basys3 board which  
 *  includes 16 switches and 5 buttons for input and 16 LEDs and four 7-segment displays plus
 *  decimal point for output.
 *
 *  The local memory map for this module is as follows:
 *      0x00: switches
 *      0x01: buttons: 00001 = btnC, 00010 = btnU, 00100 = btnL, 01000 = btnR, 10000 = btnD
 *      0x02: LEDs
 *      0x03: display 0
 *      0x04: display 1
 *      0x05: display 2
 *      0x06: display 3
 *      0x07: display decimal point (all 4 dp's)
 */
 
module basic_io(
    input  clk,             // system clock
    input [7:0] addr,       // 8-bit local address space
    input [15:0] data_in,   // data from CPU
    output [15:0] data_out, // data to CPU
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
    reg [15:0] led_buf;
    reg [6:0] display_buf [0:1];
    reg [3:0] dp_buf;
    
    reg [1:0] display_index = 0;
    
    integer i;
    initial begin
        for (i = 0; i < 4; i=i+1)
            display_buf[i] = 7'b1000000;
        dp_buf = 4'b0000;
    end
    
    // bring the input device data into our clock domain
    always @(posedge clk) begin
        sw_buf <= sw;
        btn_buf <= btn;
    end
    
    // local address decoding for writing to IO devices
    always @(posedge clk)
        if (we)
            case(addr)
                8'h02: led_buf <= data_in;
                8'h03: display_buf[0] <= data_in;
                8'h04: display_buf[1] <= data_in;
                8'h05: display_buf[2] <= data_in;
                8'h06: display_buf[3] <= data_in;
                8'h07: dp_buf <= data_in;
            endcase

    // display selection
    always @(posedge clk)
        display_index <= display_index + 2'h1;
    
    // attach output devices                
    assign led = led_buf;
    assign seg = display_buf[display_index];
    assign dp = (dp_buf >> display_index) && 4'b0001;
    //assign an = (1 << display_index);
    assign an = 4'b1111;

    // local address decoding for reading from IO devices
    assign data_out = (addr == 8'h00) ? sw  : 
                      (addr == 8'h01) ? btn : 
                      0;

endmodule
