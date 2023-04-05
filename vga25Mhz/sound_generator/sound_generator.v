/*
 * Project: sound_generator
 *
 * This is the top module of the project.
 *
 */

`include "../prescaler.v"
`include "SN76477.v"

module sound_generator (
    input  clk,     // 100MHz clock
    input  btnC,    // reset button (center button on Basys3)
    output JA1      // connector JA pin 1
);

    // 1.5MHz clock
    wire CLK_25MHz;
    prescaler #(.N(2)) ps2(clk, CLK_25MHz);
    
    SN76477 sndgen(
        .clk(CLK_25MHz),
        .reset(btnC),
        .spkr(JA1),
        .lfo_freq(1000),
        .noise_freq(90),
        .vco_freq(250),
        .vco_select(1),
        .noise_select(1),
        .lfo_shift(1),
        .mixer(3)
    );
    
endmodule
