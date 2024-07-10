module prescaler #(
    parameter N = 0
) (
    input clk_in,
    output clk_out
);
    //-- divisor register
    reg [N-1:0] divcounter;
    initial
        divcounter = 0;

    //-- N bit counter
    always @(posedge clk_in)
        divcounter <= divcounter + 1;

    //-- Use the most significant bit as output
    assign clk_out = divcounter[N-1];
    
endmodule

