/*
 * Project: sound_generator
 *
 * This project is the Basys3 port of the 8-bit Workshop "Sound Generator" project.
 * It is basically a Verilog interpretation of a modified SN76477 Complex Sound 
 * Generator device.  In this project, the sound is output to connector JA1 so 
 * that a Pmod AMP2 can be plugged into the JA connector to allow the low level 
 * sound to be heard thru a speaker connected to the AMP2's output jack.  The 
 * switches are grouped as follows:
 *  sw2-0 controls the VCO1 frequency
 *  sw5-3 controls the VCO2 frequency
 *  sw7-6 controls the LFO frequency
 *  sw8 is the VCO1 select (1 = modulated by the LFO)
 *  sw9 is the VCO2 select (1 = modulated by the LFO)
 *  sw10 is the noise select (1 = modulated by the LFO)
 *  sw11 is the LFO modulation depth
 *  sw15-12 controls the mixer (LFO, noise, VCO2, VCO1)
 *
 * Note: the noise frequency is fixed
 *
 */

`include "../prescaler.v"
`include "SN76477.v"

module sound_generator (
    input  clk,         // 100MHz clock
    input  btnC,        // reset button (center button on Basys3)
    input [15:0] sw,    // switches to control the inputs of the SN76477
    output JA1,         // Pmod AMP2 audio input
    output JA2,         // Pmod AMP2 gain
    output JA4          // Pmod AMP2 ~shutdown
);

    // 1.5MHz clock
    wire CLK_25MHz;
    prescaler #(.N(2)) ps2(clk, CLK_25MHz);
    
    SN76477 sndgen(
        .clk(CLK_25MHz),
        .reset(btnC),
        .vco1_freq({sw[2],sw[1],sw[0],6'b0}),
        .vco2_freq({sw[5],sw[4],sw[3],6'b0}),
        .noise_freq(90),
        .lfo_freq({1'b1,sw[7],sw[6],7'b0}),
        .vco1_select(sw[8]),
        .vco2_select(sw[9]),
        .noise_select(sw[10]),
        .lfo_shift({0'b0,sw[11],1'b0}),
        .mixer({sw[15],sw[14],sw[13],sw[12]}),
        .signal_out(JA1)
    );
    
    assign JA2 = 0;
    assign JA4 = 1;
    
endmodule
