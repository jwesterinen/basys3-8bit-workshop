// Force error when implicit net has no type.
`default_nettype none

module cpu16_tb;

    // Inputs
    reg clk = 1;
    reg reset = 0;
    reg hold = 0;
    reg [15:0] dataIn;

    // Output
    wire busy;
    wire [15:0] address;
    wire [15:0] dataOut;
    wire write;

    // Instantiate DUT (device under test)
    CPU16 cpu_test(clk, reset, hold, busy, address, dataIn, dataOut, write);

    initial
        forever #1 clk = ~clk;

    // Main testbench code
    initial begin
        $monitor($time, ": clk = %b, busy = %b, write = %b, address = %x, dataIn = %x, dataOut = %x", 
            clk, busy, write, address, dataIn, dataOut);
        $dumpfile("cpu16.vcd");
        $dumpvars(0, cpu16_tb);

        // reset
        reset <= 1;
        
        // canonical test with NOP on the data input bus and free spinning clock
        // ld a,0
        #2 reset <= 0; dataIn <= 0;
        
        #30

        // test computation
/*
        // ld a,0
        #2 reset <= 0;
        #6 dataIn <= 'b0000000001011001;
        #8 dataIn <= 'b0000000101011000;
        #8 dataIn <= 'b0000000001011001;
        #8 dataIn <= 'b0000000101011000;
        #8 dataIn <= 'b0000000001011001;
        #8 dataIn <= 'b0000000101011000;
/* */

/*
        // mov sp, @6fff
        #2  reset <= 0;
        #6  dataIn <= 'b0001111001011000;
        #4  dataIn <= 'b0110111111111111;
        #6  dataIn <= 'b0000000000000000;
        #8  dataIn <= 'b0000000000000000;
*/

        /*
            relative 8-bit branches both directions
            
                  jmp	Bar ; 8000
                  zero	ax
                  zero	ax
                  zero	ax
            Foo:
                  zero	ax
                  zero	ax
            Bar:
                  bcc	Baz
                  ;bcc	Foo
                  zero	ax
            Baz:
                  zero	ax
                  
        #2 reset <= 0;
        #6 dataIn <= 'b0001111101011000;  // 8000: jmp 8007
        #4 dataIn <= 'b1000000000000111;
        #6 dataIn <= 'b1000000100000001;  // 8007: bcc 8009 ; 
//        #6 dataIn <= 'b1000000111111101;  // 8007: bcc 8005 ; 
        #6 dataIn <= 'b0000000000000000;  // 8009: nop
        #6 dataIn <= 'b0000000000000000;  // 8010: nop
        #6
        */
        
        /* relative load
	    mov	bx,@0x50
        mov	ax,[bx+0x05]
        mov	ax,[bx+0x1b]	; -0x05
        */         

        #2  reset <= 0;
        #6  dataIn <= 'b1101100101010000;    // 0xd950
        #10 dataIn <= 'b0100100000101001;    // 0x4829
        #10 dataIn <= 'b0100100011011001;    // 0x48d9;
        #6  dataIn <= 'b0000000000000000;    // nop
        #6  dataIn <= 'b0000000000000000;    // nop
        #6
        
        $finish;
    end

endmodule

