/*
 * Project: sound_generator
 *
 * This project is the Basys3 port of the 8-bit Workshop "Sound Generator" project.
 * It is basically a Verilog interpretation of the SN76477 Complex Sound Generator
 * device.  In this project, the sound is output to connector JA1 so that a Pmod
 * AMP2 can be plugged into the JA connector to allow the low level sound to be
 * heard thru a speaker connected to the AMP2's output jack.  Switch sw0 controls
 * the gain.
 *
 */

`include "../prescaler.v"
`include "SN76477.v"

module sound_generator (
    input  clk,         // 100MHz clock
    input  btnC,        // reset button (center button on Basys3)
    input [15:0] sw,     // SW0 to control gain 0=12dB, 1=6dB
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
        .spkr(JA1),
        .lfo_freq({sw[5],sw[4],sw[3],7'b0}),
        .noise_freq({sw[8],sw[7],sw[6],5'b0}),
        .vco_freq({sw[2],sw[1],sw[0],6'b0}),
        .vco_select(sw[9]),
        .noise_select(sw[10]),
        .lfo_shift({sw[12],sw[11]}),
        .mixer({sw[15],sw[14],sw[13]})
    );
    
    assign JA2 = 0;
    assign JA4 = 1;
    
endmodule
