 /*
 *  sound_b3.v
 *
 *  Description:
 *      This module contains a sound generator interface to an SN76477 sound generator device.  
 *      The sound is output on "signal_out."
 *
 *  The memory map for this module is as follows:
 *      0x00: VCO1 freq lo
 *      0x01: VCO1 freq hi
 *      0x02: VCO2 freq lo
 *      0x03: VCO2 freq hi
 *      0x04: noise freq lo
 *      0x05: noise freq hi
 *      0x06: LFO freq lo
 *      0x07: LFO freq hi
 *      0x08: LFO modulation depth
 *      0x09: modulation select: {noise, VCO2, VCO1}
 *      0x0a: mixer: {LFO, noise, VCO2, VCO1}
 */
 
module sound_b3(
    input clk,              // system clock
    input reset,            // active-high reset
    input [7:0] addr,       // port addresses
    output [7:0] data_out,  // data output from IO device
    input [7:0] data_in,    // data input to IO device
    input re,               // read enable
    input we,               // write enable
    output signal_out       // sound output
);
	
    // input buffers
    reg [11:0] vco1_freq;   // VCO1 frequency (12 bits)
    reg [11:0] vco2_freq;   // VCO2 frequency (12 bits)
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
                8'h0: vco1_freq[7:0]   <= data_in;
                8'h1: vco1_freq[11:8]  <= data_in[3:0];
                8'h2: vco2_freq[7:0]   <= data_in;
                8'h3: vco2_freq[11:8]  <= data_in[3:0];
                8'h4: noise_freq[7:0]  <= data_in;
                8'h5: noise_freq[11:8] <= data_in[3:0];
                8'h6: lfo_freq[7:0]    <= data_in;
                8'h7: lfo_freq[9:8]    <= data_in[1:0];
                8'h8: lfo_shift        <= data_in[2:0];
                8'h9: mod_select       <= data_in[2:0];
                8'ha: mixer            <= data_in[3:0];
            endcase
    end

    // local address decoding for reading from registers
    assign data_out = 
        ((addr == 8'h0) && re) ? vco1_freq[7:0]     :
        ((addr == 8'h1) && re) ? vco1_freq[11:8]    :
        ((addr == 8'h2) && re) ? vco2_freq[7:0]     :
        ((addr == 8'h3) && re) ? vco2_freq[11:8]    :
        ((addr == 8'h4) && re) ? noise_freq[7:0]    :
        ((addr == 8'h5) && re) ? noise_freq [11:8]  :
        ((addr == 8'h6) && re) ? lfo_freq[7:0]      :
        ((addr == 8'h7) && re) ? lfo_freq[9:8]      :
        ((addr == 8'h8) && re) ? lfo_shift          :
        ((addr == 8'h9) && re) ? mod_select         :
        ((addr == 8'ha) && re) ? mixer              :
        0;

endmodule
