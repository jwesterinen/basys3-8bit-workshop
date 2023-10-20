 /*
 *  sound_io_16.v
 *
 *  Description:
 *      This module contains a sound generator interface to an SN76477 sound generator device.  
 *      The sound is output on "signal_out."
 *
 *      This module is based on a 16-bit data bus.
 *
 *  The memory map for this module is as follows:
 *      BASE_ADDR + 0x00: VCO1 freq
 *      BASE_ADDR + 0x01: VCO2 freq
 *      BASE_ADDR + 0x02: noise freq
 *      BASE_ADDR + 0x03: LFO freq
 *      BASE_ADDR + 0x04: LFO modulation depth
 *      BASE_ADDR + 0x05: modulation select: {noise, VCO2, VCO1}
 *      BASE_ADDR + 0x06: mixer: {LFO, noise, VCO2, VCO1}
 */
 
module sound_io_16(
    input clk,                  // system clock
    input reset,                // active-high reset
    input [15:0] addr,          // register address
    input [15:0] data_in,       // data input
    output [15:0] data_out,     // data input
    input we,                   // write enable
    output signal_out           // sound output
);
	parameter BASE_ADDR = 0;    // base address for registers
	
    // input buffers
    reg [11:0] vco1_freq;   // VCO frequency (12 bits)
    reg [11:0] vco2_freq;   // VCO frequency (12 bits)
    reg [11:0] noise_freq;	// noise frequency (12 bits)
    reg [9:0] lfo_freq;		// LFO frequency (10 bits)    
    reg [2:0] mod_select;   // select which oscillators are modulated {Noise, VCO2, VCO1}    
    reg [2:0] lfo_shift;    // LFO modulation depth 3 bits
    reg [3:0] mixer;        // select which oscillators are enabled {LFO, Noise, VCO2, VCO1}

    // sound generator with 25MHz clock
    wire clk_25MHz;
    prescaler #(.N(2)) ps2(clk, clk_25MHz);
    SN76477 sndgen(clk_25MHz, reset, vco1_freq, vco2_freq, noise_freq, lfo_freq, mod_select[0], mod_select[1],  mod_select[2], lfo_shift, mixer, signal_out);

    // local address decoding for writing to registers
    always @(posedge clk) begin
        if (we)
            casez (addr)
                {BASE_ADDR[15:8],8'h00}: vco1_freq  <= data_in[11:0];
                {BASE_ADDR[15:8],8'h01}: vco2_freq  <= data_in[11:0];
                {BASE_ADDR[15:8],8'h02}: noise_freq <= data_in[11:0];
                {BASE_ADDR[15:8],8'h03}: lfo_freq   <= data_in[9:0];
                {BASE_ADDR[15:8],8'h04}: lfo_shift  <= data_in[2:0];
                {BASE_ADDR[15:8],8'h05}: mod_select <= data_in[2:0];
                {BASE_ADDR[15:8],8'h06}: mixer      <= data_in[3:0];
            endcase
    end

    // local address decoding for reading from registers
    assign data_out = 
        (addr == {BASE_ADDR[15:8],8'h00}) ? vco1_freq  :
        (addr == {BASE_ADDR[15:8],8'h01}) ? vco2_freq  :
        (addr == {BASE_ADDR[15:8],8'h02}) ? noise_freq :
        (addr == {BASE_ADDR[15:8],8'h03}) ? lfo_freq   :
        (addr == {BASE_ADDR[15:8],8'h04}) ? lfo_shift  :
        (addr == {BASE_ADDR[15:8],8'h05}) ? mod_select :
        (addr == {BASE_ADDR[15:8],8'h06}) ? mixer      :
        0;

endmodule
