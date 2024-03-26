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
 *  Registers:
 *      0x00: switches LSB
 *      0x01: switches MSB
 *      0x02: buttons: 00001 = btnC, 00010 = btnU, 00100 = btnL, 01000 = btnR, 10000 = btnD, 0 = no button pressed
 *      0x03: N/C
 *      0x04: LEDs LSB
 *      0x05: LEDs MSB
 *      0x06: DPs
 *      0x07: N/C
 *      0x08: N/C
 *      0x09: N/C
 *      0x0a: N/C
 *      0x0b: display control - 0 = pattern display, !0 = raw  display
 *      0x0c: display 0
 *      0x0d: display 1
 *      0x0e: display 2
 *      0x0f: display 3
 */
 
module basic_io_b3(
    input  clk,                 // 100MHz system clock
    input [7:0] addr,           // register addresses
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
    reg [3:0] dp_buf;
    
    initial begin
        // init the display and DPs to all blank
        display_buf[0] <= 8'h10;
        display_buf[1] <= 8'h10;
        display_buf[2] <= 8'h10;
        display_buf[3] <= 8'h10;
        dp_buf <= 4'b0000;
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
    display_io_b3 display(clk, display_ctrl_reg, display_buf[0], display_buf[1], display_buf[2], display_buf[3], dp_buf, seg,  dp,  an);
    assign led = led_buf;

    // local address decoding for writing to IO device regs
    always @(posedge clk) begin
        if (we)
            casez (addr)
                8'h04: led_buf[7:0]                 <= data_in; // write to LEDs LSB
                8'h05: led_buf[15:8]                <= data_in; // write to LEDs MSB
                8'h06: dp_buf                       <= data_in; // write to DP buf
                8'h0b: display_ctrl_reg             <= data_in; // write to display control reg
                8'b000011??: display_buf[addr[1:0]] <= data_in; // write to displays
            endcase
    end

    // local address decoding for reading from IO device regs
    assign data_out = 
        ((addr == 8'h00) && re) ? sw_buf[7:0]         :   // switche buffer LSB
        ((addr == 8'h01) && re) ? sw_buf[15:8]        :   // switche buffer MSB
        ((addr == 8'h02) && re) ? btn_buf             :   // button code
        ((addr == 8'h04) && re) ? led_buf[7:0]        :   // LED buffer LSB to read back what was written
        ((addr == 8'h05) && re) ? led_buf[15:8]       :   // LED buffer MSB to read back what was written
        ((addr == 8'h06) && re) ? dp_buf              :   // DP buffer to read back what was written
        ((addr == 8'h0b) && re) ? display_ctrl_reg    :   // display control reg to read back what was written
        ((addr == 8'h0c) && re) ? display_buf[0]      :   // LED buffer MSB to read back what was written
        ((addr == 8'h0d) && re) ? display_buf[1]      :   // LED buffer MSB to read back what was written
        ((addr == 8'h0e) && re) ? display_buf[2]      :   // LED buffer MSB to read back what was written
        ((addr == 8'h0f) && re) ? display_buf[3]      :   // LED buffer MSB to read back what was written
        0;

endmodule
