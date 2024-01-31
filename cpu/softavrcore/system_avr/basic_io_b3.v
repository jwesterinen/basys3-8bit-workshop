`ifdef SYNTHESIS
 `include "PmodKYPD.v"
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
 *      BASE_ADDR + 0x0: switches LSB
 *      BASE_ADDR + 0x1: switches MSB
 *      BASE_ADDR + 0x2: buttons: 00001 = btnC, 00010 = btnU, 00100 = btnL, 01000 = btnR, 10000 = btnD, 0 = no button pressed
 *      BASE_ADDR + 0x3: keypad: codes 0x10 - 0x1f, 0x00 = no key pressed
 *      BASE_ADDR + 0x4: LEDs LSB
 *      BASE_ADDR + 0x5: LEDs MSB
 *      BASE_ADDR + 0x8: display control - 0 = pattern display, !0 = raw  display
 *      BASE_ADDR + 0xc: display 0
 *      BASE_ADDR + 0xd: display 1
 *      BASE_ADDR + 0xe: display 2
 *      BASE_ADDR + 0xf: display 3
 */
 
module basic_io_b3(
    input  clk,                 // system clock
    input [5:0] addr,           // port addresses
    output [7:0] data_out,      // data output from IO device
    input [7:0] data_in,        // data input to IO device
    input re,                   // read enable
    input we,                   // write enable
    input [15:0] sw,            // switches
    input [4:0] btn,            // buttons
    output [15:0] led,          // LEDs
    output [6:0] seg,           // display segments
    output dp,                  // display decimal point
`ifdef SYNTHESIS
    output [3:0] an,            // display select
    input [3:0] row,            // rows on KYPD
    output [3:0] col            // columns on KYPD
`else
    output [3:0] an             // display select
`endif    
);
	parameter BASE_ADDR = 6'h00;    // base address for registers

    // input buffers
    reg [15:0] sw_buf;
    reg [4:0] btn_buf;
    wire [4:0] kpd_buf;
    
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
    
    wire switch_clk;
`ifdef SYNTHESIS    
    // scale the input clock to ~50Hz (~20ms period)
    prescaler #(.N(20)) ps20(clk, switch_clk);
`else
    // no scaling for simulator
    assign switch_clk = clk;
`endif    
    
    // IO peripherals
`ifdef SYNTHESIS
	keypad kp(clk, row, col, kpd_buf);    
`endif	
    display_io_b3 display(clk, display_ctrl_reg, display_buf[0], display_buf[1], display_buf[2], display_buf[3], seg,  dp,  an);
    assign led = led_buf;


    // debounce the buttons and switches
    always @(posedge switch_clk) begin
        sw_buf = sw;
        btn_buf = btn;
    end
    
    // local address decoding for writing to IO devices
    always @(posedge clk) begin
        if (we)
            casez (addr)
                {BASE_ADDR[5:4],4'b0100}: led_buf[7:0]              <= data_in;     // write to LEDs LSB
                {BASE_ADDR[5:4],4'b0101}: led_buf[15:8]             <= data_in;     // write to LEDs MSB
                {BASE_ADDR[5:4],4'b1000}: display_ctrl_reg          <= data_in;     // write to display control reg
                {BASE_ADDR[5:4],4'b11??}: display_buf[addr[1:0]]    <= data_in;     // write to displays
            endcase
    end

    // local address decoding for reading from IO devices
    assign data_out = 
        (addr == {BASE_ADDR[5:4],4'b0000} && re) ? sw_buf[7:0]      :   // switche buffer LSB
        (addr == {BASE_ADDR[5:4],4'b0001} && re) ? sw_buf[15:8]     :   // switche buffer MSB
        (addr == {BASE_ADDR[5:4],4'b0010} && re) ? {3'h0,btn_buf}   :   // button codes
        (addr == {BASE_ADDR[5:4],4'b0011} && re) ? {3'h0,kpd_buf}   :   // keypad keycodes
        (addr == {BASE_ADDR[5:4],4'b0100} && re) ? led_buf[7:0]     :   // LED buffer LSB to read back what was written
        (addr == {BASE_ADDR[5:4],4'b0101} && re) ? led_buf[15:8]    :   // LED buffer MSB to read back what was written
        0;

endmodule
