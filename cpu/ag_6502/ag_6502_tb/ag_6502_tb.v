// Force error when implicit net has no type.
`default_nettype none

// test module
module ag6502_tb;

    // inputs
    reg CLK = 1;
    reg RESET = 0;
    reg [7:0] DI;       // data in, read bus
    reg IRQ = 0;        // interrupt request
    reg NMI = 0;        // non-maskable interrupt request
    reg RDY = 1;        // Ready signal. Pauses CPU when RDY=0 
    reg SO = 1;         // Set overflow.
    
    // outputs
    wire CLK_01;
    wire CLK_02;
    wire [15:0] AB;     // address bus
    wire [7:0] DO;      // data out, write bus
    wire RDWR_;         // write enable
    wire SYNC;          // synchronize
    
    ag6502 cpu( CLK, CLK_01, CLK_02, AB, RDWR_, DI, DO, RDY, RESET, IRQ, NMI, SO, SYNC );

    initial
        forever #1 CLK = ~CLK;

    // Main testbench code
    initial begin
        $monitor($time, ": CLK = %b, RESET = %b, RDWR_ = %b, AB = %x, DI = %x, DO = %x", 
            CLK, RESET, RDWR_, AB, DI, DO);
        $dumpfile("ag_6502.vcd");
        $dumpvars(0, ag6502_tb);

        // canonical test with NOP on the data input bus and free spinning clock
        
        // reset vector @0xfffc = 0xe000
        #16 RESET <= 1;
        #8 DI <= 'b00000000;
        #2 DI <= 'b11100000;
        
        // nop
        #2 DI <= 'b11101010;
        #30

        $finish;
    end

  
endmodule

