
`ifndef CPU16_H
`define CPU16_H

// ALU operations
`define OP_ZERO     4'h0
`define OP_LOAD_A   4'h1
`define OP_INC      4'h2
`define OP_DEC      4'h3
`define OP_ASL      4'h4
`define OP_LSR      4'h5
`define OP_ROL      4'h6
`define OP_ROR      4'h7
`define OP_OR       4'h8
`define OP_AND      4'h9
`define OP_XOR      4'ha
`define OP_LOAD_B   4'hb
`define OP_ADD      4'hc
`define OP_SUB      4'hd
`define OP_ADC      4'he
`define OP_SBB      4'hf


module ALU(A, B, carry, aluop, Y);

    parameter N = 8;	// default width = 8 bits
    input  [N-1:0] A;	// A input
    input  [N-1:0] B;	// B input
    input  carry;		// carry input
    input  [3:0] aluop;	// alu operation
    output reg [N:0] Y;	// Y output + carry
  
    always @(*)
        case (aluop)
            // unary operations
            `OP_ZERO:         Y = 0;
            `OP_LOAD_A:       Y = {1'b0, A};
            `OP_INC:          Y = A + 1;
            `OP_DEC:          Y = A - 1;
            
            // unary operations that generate and/or use carry
            `OP_ASL:          Y = {A, 1'b0};
            `OP_LSR:          Y = {A[0], 1'b0, A[N-1:1]};
            `OP_ROL:          Y = {A, carry};
            `OP_ROR:          Y = {A[0], carry, A[N-1:1]};
            
            // binary operations
            `OP_OR:           Y = {1'b0, A | B};
            `OP_AND:          Y = {1'b0, A & B};
            `OP_XOR:          Y = {1'b0, A ^ B};
            `OP_LOAD_B:       Y = {1'b0, B};
            
            // binary operations that generate and/or use carry
            `OP_ADD:          Y = A + B;
            `OP_SUB:          Y = A - B;
            `OP_ADC:          Y = A + B + (carry?1:0);
            `OP_SBB:          Y = A - B - (carry?1:0);
        endcase

endmodule

/*
00000aaa 0++++bbb	operation A + B -> A
00001aaa 0++++bbb	operation A + [B] -> A
00011aaa 0++++000	operation A + imm16 -> A
00101aaa ########	load zero page
00110aaa ########	store zero page
01000bbb 00000aaa	store A -> [B]
01001aaa #####bbb	load [B + #] -> A
01010aaa #####bbb   store A -> [B + #]
01100000 00000aaa   store A -> [imm16]
01101aaa 01+++000	operation A <op> [imm16] -> A - Note: _only_ binary ops
01110aaa 00cccbbb	store A -> [B], C -> IP
1000tttt ########	conditional branch
11+++aaa ########	immediate binary operation
*/

module CPU16(clk, reset, hold, busy, address, data_in, data_out, write);

    input             clk;
    input             reset;
    input	          hold;
    output reg        busy;
    output reg [15:0] address;
    input      [15:0] data_in;
    output reg [15:0] data_out;
    output reg        write;

    // wait state for RAM?
    parameter RAM_WAIT = 1;

    reg [15:0] regs[0:7];   // 8 16-bit registers
    reg [2:0] state;        // CPU state

    reg carry = 0;	        // carry flag
    reg zero = 0;	        // zero flag
    reg neg = 0;	        // negative flag

    wire [16:0] Y;	        // ALU 16-bit + carry output
    reg [3:0] aluop;	    // ALU operation

    reg [15:0] opcode;      // used to decode ALU inputs
    wire [2:0] rdest = opcode[10:8]; // ALU A input reg.
    wire [2:0] rsrc = opcode[2:0]; // ALU B input reg.
    wire Bconst = opcode[15]; // ALU B = 8-bit constant
    wire Bload  = opcode[11]; // ALU B = data bus
    
    reg [15:0] sub_addr;

    // CPU states
    localparam S_RESET              = 0;
    localparam S_SELECT             = 1;
    localparam S_DECODE             = 2;
    localparam S_COMPUTE            = 3;
    localparam S_DECODE_WAIT        = 4;
    localparam S_COMPUTE_WAIT       = 5;
    localparam S_COMPUTE_ADDR       = 6;
    localparam S_COMPUTE_SUB_ADDR   = 7;

    localparam EP = 4; // eval stack ptr = register 4
    localparam BP = 5; // base ptr = register 5
    localparam SP = 6; // stack ptr = register 6
    localparam IP = 7; // IP = register 7

    ALU #(16) alu(
        .A(regs[rdest]),
        .B(Bconst ? {8'b0, opcode[7:0]} 
          : Bload ? data_in 
                  : regs[rsrc]),
        .Y(Y),
        .aluop(aluop),
        .carry(carry)
    );

    // main state machine
    always @(posedge clk)
    if (reset) begin
        state <= S_RESET;
        busy <= 1;
    end else begin
        case (state)
        
            // state 0: reset
            S_RESET: begin
                //regs[IP] <= 16'h8000;
                regs[IP] <= 16'hf000;
                write <= 0;
                state <= S_SELECT;
            end
            
            // state 1: select opcode address
            S_SELECT: begin
                write <= 0;
                if (hold) begin
                    busy <= 1;
                    state <= S_SELECT;
                end else begin
                    busy <= 0;
                    address <= regs[IP];
                    regs[IP] <= regs[IP] + 1;
                    state <= RAM_WAIT ? S_DECODE_WAIT : S_DECODE;
                end
            end
            
            // state 2: read/decode opcode
            S_DECODE: begin
                // default next state - if data_in[11] need to go to a wait state to give sync memory a clock to respond
                state <= RAM_WAIT && data_in[11] ? S_COMPUTE_WAIT : S_COMPUTE;
                casez (data_in)
                
                    //  00000aaa0++++bbb	operation A+B->A
                    16'b00000???0???????: begin
                        aluop <= data_in[6:3];
                    end
                    
                    //  00001aaa01+++bbb	operation A+[B]->A - Note: _only_ binary ops
                    16'b00001???01??????: begin
                        address <= regs[data_in[2:0]];
                        aluop <= data_in[6:3];
                        if (data_in[2:0] == SP)
                            regs[SP] <= regs[SP] + 1;
                    end
                    
                    //  00011aaa0++++000	operation A+imm16 -> A
                    16'b00011???0????000: begin
                        address <= regs[IP];
                        regs[IP] <= regs[IP] + 1;
                        aluop <= data_in[6:3];
                    end
                    
                    //  11+++aaa########	immediate binary operation
                    16'b11??????????????: begin
                        aluop <= data_in[14:11];
                    end
                    
                    //  00101aaa########	load ZP memory
                    16'b00101???????????: begin
                        address <= {8'b0, data_in[7:0]};
                        aluop <= `OP_LOAD_B;
                    end
                    
                    //  00110aaa########	store ZP memory
                    16'b00110???????????: begin
                        address <= {8'b0, data_in[7:0]};
                        data_out <= regs[data_in[10:8]];
                        write <= 1;
                        state <= S_SELECT;
                    end
                    
                    //  0100011100000110	store IP -> [SP], <immed16> -> IP - essence of direct subroutine call
                    16'b01000???00000???: begin
                        address <= regs[IP];
                        data_out <= regs[IP];
                        regs[IP] <= regs[IP] + 1;
                        state <= S_COMPUTE_ADDR;
                   end
                    
                    //  01001aaa#####bbb	load [B+#] -> A
                    16'b01001???????????: begin
                        //address <= regs[data_in[2:0]] + 16'($signed(data_in[7:3]));
                        address <= regs[data_in[2:0]] + ((data_in[7]) ? {11'b11111111111, data_in[7:3]} : {11'b00000000000, data_in[7:3]});
                        aluop <= `OP_LOAD_B;
                        // with SP this is a "pop" so autoincrement the SP -- used for the system stack
                        if (data_in[2:0] == SP)
                            regs[SP] <= regs[SP] + 1;
                    end
                    
                    //  01010aaa#####bbb    store A -> [B+#]
                    16'b01010???????????: begin
                        //address <= regs[data_in[2:0]] + 16'($signed(data_in[7:3]));
                        address <= regs[data_in[2:0]] + ((data_in[7]) ? {11'b11111111111, data_in[7:3]} : {11'b00000000000, data_in[7:3]});
                        data_out <= regs[data_in[10:8]];
                        write <= 1;
                        state <= S_SELECT;
                        // with SP this is a "push" so autodecrement the SP -- used for the system stack
                        if (data_in[2:0] == SP)
                            regs[SP] <= regs[SP] - 1;
                    end

                    //  0110000000000aaa    store A -> [imm16] -- direct address store
                    16'b0110000000000???: begin
                        address <= regs[IP];
                        data_out <= regs[data_in[2:0]];
                        regs[IP] <= regs[IP] + 1;
                        state <= S_COMPUTE_ADDR;
                    end
                    
                    //  01101aaa01+++000	operation A <op> [imm16] -> A - Note: _only_ binary ops
                    16'b01101???01???000: begin
                        address <= regs[IP];
                        regs[IP] <= regs[IP] + 1;
                        aluop <= data_in[6:3];
                        state <= S_COMPUTE_ADDR;
                    end
                    
                    //  01110aaa00cccbbb	store A -> [B], C -> IP - essence of subroutine call
                    16'b01110???00??????: begin
                        address <= regs[data_in[2:0]];
                        data_out <= regs[data_in[10:8]];
                        write <= 1;
                        state <= S_SELECT;
                        if (data_in[2:0] == SP)
                            regs[SP] <= regs[SP] - 1;
                        regs[IP] <= regs[data_in[5:3]];
                    end

                    //  1000tttt########	conditional branch
                    16'b1000????????????: begin
                        if (
                            (data_in[11:8] == 4'b0000) ||             // bra - branch always
                            (data_in[8] && (data_in[11] == carry)) ||
                            (data_in[9] && (data_in[11] == zero)) ||
                            (data_in[10] && (data_in[11] == neg))) 
                        begin
                            // relative branch, sign extended
                            //regs[IP] <= regs[IP] + 16'($signed(data_in[7:0]));
                            regs[IP] <= regs[IP] + ((data_in[7]) ? {8'b11111111, data_in[7:0]} : {8'b00000000, data_in[7:0]});
                        end
                        state <= S_SELECT;
                    end
                    
                    //  1010tttt########	conditional subroutine branch, bsa, bsxx
                    16'b1010????????????: begin
                        if (
                            (data_in[11:8] == 4'b0000) ||             // bra - branch always
                            (data_in[8] && (data_in[11] == carry)) ||
                            (data_in[9] && (data_in[11] == zero)) ||
                            (data_in[10] && (data_in[11] == neg))) 
                        begin
                            // relative branch, sign extended                            
                            address <= regs[SP];
                            data_out <= regs[IP];
                            write <= 1;
                            state <= S_SELECT;
                            regs[SP] <= regs[SP] - 1;
                            regs[IP] <= regs[IP] + ((data_in[7]) ? {8'b11111111, data_in[7:0]} : {8'b00000000, data_in[7:0]});
                        end
                        state <= S_SELECT;
                    end
                    
                    // fall-through RESET
                    default: begin
                        state <= S_RESET; // reset
                    end
                    
                endcase
                opcode <= data_in; // (only use opcode next cycle)
            end

            // state 6: compute a direct address
            S_COMPUTE_ADDR: begin
                casez (opcode[15:11])
                    5'b01000: begin
                        // direct address subroutine call -- push the IP onto the stack and save the second word as the subroutine address
                        address <= regs[SP];
                        data_out <= regs[IP];
                        sub_addr <= data_in;
                        write <= 1;
                        regs[SP] <= regs[SP] - 1;
                        state <= S_COMPUTE_SUB_ADDR;
                    end
                    
                    5'b01100: begin
                        // direct address store -- transfer the second word to the address bus then go on to the next instruction
                        address <= data_in;
                        write <= 1;
                        state <= S_SELECT;
                    end
                    
                    5'b01101: begin
                        // direct address load -- transfer the second word to the address bus then go on to compute
                        address <= data_in;
                        state <= S_COMPUTE;
                    end
                endcase
            end
                        
            S_COMPUTE_SUB_ADDR: begin
                // direct address subroutine call -- load the IP from the saved subroutine address then go on to the next instruction
                regs[IP] <= sub_addr;                
                write <= 0;
                state <= S_SELECT;
            end
            
            // state 3: compute ALU op and flags
            S_COMPUTE: begin
                // transfer ALU output to destination
                regs[rdest] <= Y[15:0];

                // set carry for certain operations (4-7,12-15)
                if (aluop[2]) carry <= Y[16];
                
                // set zero flag
                zero <= ~|Y[15:0];
                neg <= Y[15];
                
                // repeat CPU loop
                state <= S_SELECT;
            end

            // state 4: wait 1 cycle for RAM read
            S_DECODE_WAIT: begin
                state <= S_DECODE;
            end
            
            // state 5: wati 1 cycle for RAM read
            S_COMPUTE_WAIT : begin
                state <= S_COMPUTE;
            end
            
        endcase
    end

endmodule

`ifdef TOPMOD__test_CPU16_top

module test_CPU16_top(
    input  clk,
    input  reset,
    output [15:0] address_bus,
    output reg [15:0] to_cpu,
    output [15:0] from_cpu,
    output write_enable,
    output [15:0] IP,
    output zero,
    output carry,
    output busy,
    output [2:0] state
);

    reg [15:0] ram[0:65535];
    reg [15:0] rom[0:255];

    assign IP = cpu.regs[7];
    assign zero = cpu.zero;
    assign carry = cpu.carry;
    assign state = cpu.state;
  
    CPU16 cpu(
        .clk(clk),
        .reset(reset),
        .hold(0),
        .busy(busy),
        .address(address_bus),
        .data_in(to_cpu),
        .data_out(from_cpu),
        .write(write_enable)
    );

    always @(posedge clk)
        if (write_enable) begin
            ram[address_bus] <= from_cpu;
        end
  
    always @(posedge clk)
        if (address_bus[15] == 0)
            to_cpu <= ram[address_bus];
        else
            to_cpu <= rom[address_bus[7:0]];
  
`ifdef EXT_INLINE_ASM
    initial begin
        rom = '{
            __asm
                .arch femto16
                .org 0x8000
                .len 256
                
                jmp	Bar
                zero	ax
                zero	ax
                zero	ax
                Foo:
                zero	ax
                zero	ax
                Bar:
                ;bcc	Foo
                bcs	Baz
                zero	ax
                zero	ax
                zero	ax
                zero	ax
                zero	ax
                Baz:
                zero	ax


                mov	sp,@$6fff
                mov	dx,@Fib
                jsr	dx
                reset
                Fib:
                mov	ax,#1
                mov	bx,#0
                Loop:
                mov	cx,ax
                add	ax,bx
                mov	bx,cx
                push	ax
                pop	ax
                mov	[42],ax
                mov	ax,[42]
                bcc	Loop
                rts
            __endasm
        };
    end
`endif

endmodule

`endif

`endif

