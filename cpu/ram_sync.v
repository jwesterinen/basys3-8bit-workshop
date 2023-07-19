/*
    Module: ram_sync - Synchronous RAM module.

    Module parameters:
        ADDR_WIDTH - number of address bits (default = 16)
        DATA_WIDTH - number of data bits (default = 16)
    
    The dout register and its assignment from the memory array will cause
    the compiler to use block RAM instead of discreet logic.
    
*/

module RAM_sync(clk, addr, din, dout, we_);
  
    parameter ADDR_WIDTH = 16;
    parameter DATA_WIDTH = 16;
  
    input  clk;		                    // clock
    input  [ADDR_WIDTH-1:0] addr;       // address
    input  [DATA_WIDTH-1:0] din;        // input data
    input  we_;		                    // write enable - active low

    output reg [DATA_WIDTH-1:0] dout;   // output data latch

    reg [DATA_WIDTH-1:0] mem [0:(1<<ADDR_WIDTH)-1];  // memory array

    always @(posedge clk) begin
        if (~we_)
            mem[addr] <= din;
        dout <= mem[addr];
    end

endmodule


