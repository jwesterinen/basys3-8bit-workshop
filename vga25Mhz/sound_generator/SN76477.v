
`include "lfsr.v"

/*
 * Module: SN76477
 *
 * This module is a modified SN76477 Complex Sound Generator device. It has two 
 * square-wave oscillators (VCOs) which can be modulated by a low-frequency 
 * oscillator (LFO) and also mixed with a LFSR noise source.
 *
 */

module SN76477(clk, reset, spkr,
               lfo_freq,noise_freq, 
               vco1_freq, vco2_freq,
               vco1_select, vco2_select, 
               noise_select, lfo_shift, mixer);

    input clk, reset;
    output reg spkr = 0;		// module output

    input [9:0] lfo_freq;		// LFO frequency (10 bits)
    input [11:0] noise_freq;	// noise frequency (12 bits)
    
    input [11:0] vco1_freq;	    // VCO frequency (12 bits)
    input [11:0] vco2_freq;	    // VCO frequency (12 bits)
    input vco1_select;		    // 1 = LFO modulates VCO
    input vco2_select;		    // 1 = LFO modulates VCO
    
    input noise_select;		    // 1 = LFO modulates Noise
    input [2:0] lfo_shift;	    // LFO modulation depth
    input [3:0] mixer;		    // mix enable {LFO, Noise, VCO2, VCO1}

    reg [3:0] div16;		    // divide-by-16 counter
    reg [17:0] lfo_count;		// LFO counter (18 bits)
    reg lfo_state;		        // LFO output
    reg [12:0] noise_count;	    // Noise counter (13 bits)
    reg noise_state;		    // Noise output
    
    reg [12:0] vco1_count;		// VCO 1 counter (12 bits)
    reg vco1_state;		        // VCO 1 output
    reg [12:0] vco2_count;		// VCO 2 counter (12 bits)
    reg vco2_state;		        // VCO 2 output

    wire [15:0] lfsr;		    // LFSR output

    LFSR #(16, 16'b1000000001011, 0) lfsr_gen(
        .clk(clk),
        .reset(reset),
        .enable(div16 == 0 && noise_count == 0),
        .lfsr(lfsr)
    );

    // create triangle waveform from LFO
    wire [11:0] lfo_triangle = lfo_count[17] ? ~lfo_count[17:6] : lfo_count[17:6];
    wire [11:0] vco1_delta = lfo_triangle >> lfo_shift;
    wire [11:0] vco2_delta = lfo_triangle >> lfo_shift;

    always @(posedge clk) begin
        // divide clock by 64
        div16 <= div16 + 1;
        if (div16 == 0) begin
        
            // VCO  1 oscillator
            if (reset || vco1_count == 0) begin
                vco1_state <= ~vco1_state;
                if (vco1_select)
                    vco1_count <= vco1_freq + vco1_delta;
                else
                    vco1_count <= vco1_freq + 0;
            end else
                vco1_count <= vco1_count - 1;
                
            // VCO  2 oscillator
            if (reset || vco2_count == 0) begin
                vco2_state <= ~vco2_state;
                if (vco2_select)
                    vco2_count <= vco2_freq + vco2_delta;
                else
                    vco2_count <= vco2_freq + 0;
            end else
                vco2_count <= vco2_count - 1;
                
            // LFO oscillator
            if (reset || lfo_count == 0) begin
                lfo_state <= ~lfo_state;
                lfo_count <= {lfo_freq, 8'b0};
            end else
                lfo_count <= lfo_count - 1;

            // Noise oscillator
            if (reset || noise_count == 0) begin
                if (lfsr[0])
                    noise_state <= ~noise_state;
                if (noise_select)
                    noise_count <= noise_freq + vco1_delta;
                else
                    noise_count <= noise_freq + 0;
            end else
                noise_count <= noise_count - 1;
                
            // Mixer            
            spkr <= (lfo_state | ~mixer[3]) & (noise_state | ~mixer[2]) & (vco2_state | ~mixer[1]) & (vco1_state | ~mixer[0]);
        end
    end

endmodule

