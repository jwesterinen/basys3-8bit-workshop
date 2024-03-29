`ifndef LFSR_V
`define LFSR_V

/*
 *  Configurable Linear Feedback Shift Register.
 *
 *  Parameters:
 *      NBITS:  bit width (default = 8)
 *      TAPS:   bitmask for taps (default = 8'b11101)
 *      INVERT: invert feedback bit? (default = 0)
 *
 */

module LFSR(clk, reset, enable, lfsr);
  
    parameter NBITS  = 8;           // bit width
    parameter TAPS   = 8'b11101;    // bitmask for taps
    parameter INVERT = 0;		    // invert feedback bit?

    input clk, reset;
    input enable;			        // only perform shift when enable=1
    output reg [NBITS-1:0] lfsr;    // shift register

    wire feedback = lfsr[NBITS-1] ^ INVERT;
    
    initial begin
        lfsr = 8'b11111111;          // init with all 1s - note this won't work for other sizes of the lfst reg
    end

    always @(posedge clk)
    begin
        if (reset)
            lfsr <= {lfsr[NBITS-2:0], 1'b1}; // reset loads with all 1s
        else if (enable)
            lfsr <= {lfsr[NBITS-2:0], 1'b0} ^ (feedback ? TAPS : 0);
    end

endmodule

`endif

