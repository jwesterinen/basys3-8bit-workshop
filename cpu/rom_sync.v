/*
    Module: rom_sync - synchronous ROM module with initialization

    Module parameters:
        ADDR_WIDTH - number of address bits (default = 16)
        DATA_WIDTH - number of data bits (default = 16)
        MEM_INIT_FILE - initialization binary file
    
    The data register and its assignment from the memory array will cause
    the compiler to use block RAM instead of discreet logic.
    
*/


module ROM_sync (clk, addr, data);

    parameter DATA_WIDTH = 16;
    parameter ADDR_WIDTH = 16;
    parameter MEM_INIT_FILE = "rom.bin";
    
    input clk;                                      // clock
    input [ADDR_WIDTH-1:0] addr;                    // address
    
    output reg [DATA_WIDTH-1:0] data;               // output data latch

    reg [DATA_WIDTH-1:0] mem [0:(1<<ADDR_WIDTH)-1]; // memory array

    // load the ROM with the binary file
    initial $readmemh(MEM_INIT_FILE, mem);

    always @(posedge clk) begin
        data <= mem[addr];
    end
    
endmodule

