 /*
 *  sound_io_b3.v
 *
 *  Description:
 *      This module contains a sound generator interface to an SN76477 sound generator device.  
 *      The sound is output on "signal_out."
 *
 *      This module is based on the AVR 8-bit I/O port data bus.
 *
 *  Registers:
 *      0x00: VCO1:  {mixer select, LFO mod select, N/A, N/A, VCO1 freq bit 10, VCO1 freq bit 9, VCO1 freq bit 8, VCO1 freq bit 7}
 *      0x01: VCO2:  {mixer select, LFO mod select, N/A, N/A, VCO2 freq bit 10, VCO2 freq bit 9, VCO2 freq bit 8, VCO2 freq bit 7}
 *      0x02: noise: {mixer select, LFO mod select, N/A, N/A, noise freq bit 10, noise freq bit 9, noise freq bit 8, noise freq bit 7}
 *      0x03: LFO:   {mixer select, LFO shift bit 2, LFO shift bit 1, LFO shift bit 0, LFO freq bit 10, LFO freq bit 9, LFO freq bit 8, LFO freq bit 7}
 */
 
`ifdef SYNTHESIS
 `include "SN76477.v"
`endif

module sound_io_b3(
    input clk,              // 100MHz system clock
    input reset,            // active-high reset
    input [3:0] addr,       // reg addresses
    output [7:0] data_out,  // data output from IO device
    input [7:0] data_in,    // data input to IO device
    input re,               // read enable
    input we,               // write enable
    output signal_out       // sound output
);

    // input buffers
    reg [7:0] vco1;         // VCO1 related values as described above
    reg [7:0] vco2;         // VCO2 related values as described above
    reg [7:0] noise;	    // noise related values as described above
    reg [7:0] lfo;		    // LFO related values as described above

    // sound generator with 25MHz clock
    wire clk_25MHz;
    prescaler #(.N(2)) ps2(clk, clk_25MHz);
    SN76477 sndgen
    (
        clk_25MHz, reset, 
        {2'b00,vco1[3:0],6'b000000},        // vco1_freq
        {2'b00,vco2[3:0],6'b000000},        // vco2_freq
        {1'b0,noise[3:0],7'b0000000},       // noise_freq
        {lfo[3:0],6'b000000},               // lfo_freq
        vco1[6],                            // vco1_select
        vco2[6],                            // vco2_select
        noise[6],                           // noise_select
        lfo[6:4],                           // lfo_shift
        {lfo[7],noise[7],vco2[7],vco1[7]},  // mixer
        signal_out
    );

    // local address decoding for writing to registers
    always @(posedge clk) begin
        if (we)
            casez (addr)
                4'h0: vco1  <= data_in;
                4'h1: vco2  <= data_in;
                4'h2: noise <= data_in;
                4'h3: lfo   <= data_in;
            endcase
    end

    // local address decoding for reading from registers
    assign data_out = 
        ((addr == 4'h0) && re) ? vco1  :
        ((addr == 4'h1) && re) ? vco2  :
        ((addr == 4'h2) && re) ? noise :
        ((addr == 4'h3) && re) ? lfo   :
        0;

endmodule
