`ifdef SYNTHESIS
 `include "display_io_b3.v"
`endif

/*
 *  basic_io_b3.v
 *
 *  Description:
 *      This module contains all of the basic, on-board I/O devices of the Basys3 board which  
 *  includes 16 switches and 5 buttons for input and 16 LEDs and four 7-segment displays plus
 *  decimal point for output.
 *
 *  The local memory map for this module is as follows:
 *      0x0: switches LSB
 *      0x1: switches MSB
 *      0x2: buttons: 00001 = btnC, 00010 = btnU, 00100 = btnL, 01000 = btnR, 10000 = btnD, 0 = no button pressed
 *      0x4: LEDs LSB
 *      0x5: LEDs MSB
 *      0x8: display control - 0 = pattern display, !0 = raw  display
 *      0xc: display 0
 *      0xd: display 1
 *      0xe: display 2
 *      0xf: display 3
 */
 
module basic_io_b3(
    input  clk,                 // 100MHz system clock
    input [3:0] addr,           // register addresses
    output [7:0] data_out,      // data output from IO device
    input [7:0] data_in,        // data input to IO device
    input re,                   // read enable
    input we,                   // write enable
    input [15:0] sw,            // switches
    input [4:0] btn,            // buttons
    output [15:0] led,          // LEDs
    output [6:0] seg,           // display segments
    output dp,                  // display decimal point
    output [3:0] an             // display select
);

    // input buffers
    reg [15:0] sw_buf;
    reg [4:0] btn_buf;
    
    // output buffers
    reg [15:0] led_buf;
    reg display_ctrl_reg;
    reg [7:0] display_buf [0:3];
    
    initial begin
        // init the display to all blank
        display_buf[0] <= 8'h10;
        display_buf[1] <= 8'h10;
        display_buf[2] <= 8'h10;
        display_buf[3] <= 8'h10;
    end
    
    wire debounce_clk;
`ifdef SYNTHESIS    
    // scale the input clock to ~50Hz (~20ms period)
    prescaler #(.N(21)) ps20(clk, debounce_clk);
`else
    // no scaling for simulator
    assign debounce_clk = clk;
`endif    
    
    // debounce the buttons and switches
    always @(posedge debounce_clk) begin
        sw_buf <= sw;
        btn_buf <= btn;
    end
    
    // IO peripherals
    display_io_b3 display(clk, display_ctrl_reg, display_buf[0], display_buf[1], display_buf[2], display_buf[3], seg,  dp,  an);
    assign led = led_buf;

    // local address decoding for writing to IO device regs
    always @(posedge clk) begin
        if (we)
            casez (addr)
                4'b0100: led_buf[7:0]           <= data_in; // write to LEDs LSB
                4'b0101: led_buf[15:8]          <= data_in; // write to LEDs MSB
                4'b1000: display_ctrl_reg       <= data_in; // write to display control reg
                4'b11??: display_buf[addr[1:0]] <= data_in; // write to displays
            endcase
    end

    // local address decoding for reading from IO device regs
    assign data_out = 
        ((addr == 4'b0000) && re) ? sw_buf[7:0]         :   // switche buffer LSB
        ((addr == 4'b0001) && re) ? sw_buf[15:8]        :   // switche buffer MSB
        ((addr == 4'b0010) && re) ? btn_buf             :   // button code
        ((addr == 4'b0100) && re) ? led_buf[7:0]        :   // LED buffer LSB to read back what was written
        ((addr == 4'b0101) && re) ? led_buf[15:8]       :   // LED buffer MSB to read back what was written
        ((addr == 4'b1000) && re) ? display_ctrl_reg    :   // display control reg to read back what was written
        ((addr == 4'b1100) && re) ? display_buf[0]      :   // LED buffer MSB to read back what was written
        ((addr == 4'b1101) && re) ? display_buf[1]      :   // LED buffer MSB to read back what was written
        ((addr == 4'b1110) && re) ? display_buf[2]      :   // LED buffer MSB to read back what was written
        ((addr == 4'b1111) && re) ? display_buf[3]      :   // LED buffer MSB to read back what was written
        0;

endmodule
