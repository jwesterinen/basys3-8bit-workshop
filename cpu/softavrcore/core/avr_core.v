/*****************************************************************************/
/* avr_core.v								     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* (c) 2019-2020, 2022-2023; Andras Pal <apal@szofi.net>		     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* Portions of the code got inspiration from the Navre project		     */
/* (https://opencores.org/projects/navre)				     */
/*****************************************************************************/

/* select architecture below: */
//`define AVR_200
//`define AVR_250
//`define AVR_310
//`define AVR_350
//`define AVR_400
//`define AVR_500
`define AVR_510

/* on some systems, memory access needs to be in an "inital" stage if PLLs are also used: */
`define AVR_INITIAL

/* enable the sleep instruction and sleep mode if needed: */
`define AVR_HAVE_SLEEP

/* enable WDR instruction if needed: */
`define AVR_HAVE_WDR

/* although the presence of AVR_HAVE_22BITPC (on avr3* and avr5* architectures) 
   seemingly implies this option, the interrupt vector table does not need to 
   be 32-bit in fact if all of the vectors are located in the first 8k 
   (4k word) of the program memory. Therefore, define this below in accordance 
   to your crt0.S: */
// `define AVR_2WORD_INTERRUPTS

/*****************************************************************************/

module avr_core
#(	parameter pmem_width = 9, 	/* PMEM address bus width (16-bit instr)   	*/
	parameter dmem_width = 9, 	/* RAM address bus width (bytes)	     	*/
	parameter interrupt  = 1,	/* change it to 0 to disable interrupt support	*/
	parameter intr_width = 2, 	/* number of interrupt vector bits	    	*/
	parameter lsb_call = 0,	  	/* change it to 1 to have LSB ordering in the call stack	*/
	parameter ld_st_io_reg = 1	/* change it to 0 to omit support for LD/ST to/from registers and I/O space */
 ) 
 (	input	clk,
	input	rst,

	output	pmem_ce,
	output	[pmem_width-1:0] pmem_a,
	input	[15:0] pmem_d,

	output	dmem_re,
	output	dmem_we,
	output	[dmem_width-1:0] dmem_a,
	input	[7:0] dmem_di,
	output	[7:0] dmem_do,

	output	io_re,
	output	io_we,
	output	[5:0] io_a,
	input	[7:0] io_di,
	output	[7:0] io_do,

	input	in_iflag,
	input	[intr_width-1:0] in_ivect,

	output	[1:0]	mode,
	output	[intr_width-1:0] in_ieack
 );

/*****************************************************************************/

/******************************************************************************
	     |	avr2	avr25	avr31	avr35	avr4	avr5	avr51
-------------+-----------------------------------------------------------------
MOVW	     |		  X	   X	  X	  X	  X	  X
LPM  Rd,Z(+) |		  X	   X	  X	  X	  X	  X
ELPM	     |			   X	  		  	  X
ELPM Rd,Z(+) |							  X
JMP/CALL     |			   X	  X		  X	  X
SPM	     |		  X		  X	  X	  X	  X 	
MUL	     |					  X	  X	  X
******************************************************************************/

`ifdef AVR_250
`define AVR_HAVE_MOVW
`define AVR_HAVE_LPMZ
`define	AVR_HAVE_SPM
`endif

`ifdef AVR_310
`define AVR_HAVE_MOVW
`define AVR_HAVE_LPMZ
`define AVR_HAVE_ELPM
`define AVR_HAVE_22BITPC
`endif

`ifdef AVR_350
`define AVR_HAVE_MOVW
`define AVR_HAVE_LPMZ
`define AVR_HAVE_22BITPC
`define	AVR_HAVE_SPM
`endif

`ifdef AVR_400
`define AVR_HAVE_MOVW
`define AVR_HAVE_LPMZ
`define	AVR_HAVE_SPM
`define AVR_HAVE_MUL
`endif

`ifdef AVR_500
`define AVR_HAVE_MOVW
`define AVR_HAVE_LPMZ
`define AVR_HAVE_22BITPC
`define	AVR_HAVE_SPM
`define AVR_HAVE_MUL
`endif

`ifdef AVR_510
`define AVR_HAVE_MOVW
`define AVR_HAVE_LPMZ
`define AVR_HAVE_ELPM
`define AVR_HAVE_ELPMZ
`define AVR_HAVE_22BITPC
`define	AVR_HAVE_SPM
`define AVR_HAVE_MUL
`endif

/* This block below is now deliberately commented out, see note above */

/*
`ifdef 	AVR_HAVE_22BITPC
`define AVR_2WORD_INTERRUPTS
`endif
*/

/*****************************************************************************/

localparam MEM_OFFSET = 0;

/*****************************************************************************/

`ifdef AVR_INITIAL
localparam	STATE_INITIAL	= 4'd0;
localparam	STATE_STALL	= 4'd1;
`else
localparam	STATE_STALL	= 4'd0;
`endif
localparam	STATE_NORMAL	= 4'd2;
localparam	STATE_TWOWORD	= 4'd3;
localparam	STATE_SKIP	= 4'd4;
localparam	STATE_LD	= 4'd5;
localparam	STATE_CALL	= 4'd6;
localparam	STATE_RET	= 4'd7;
localparam	STATE_RET2	= 4'd8;
localparam	STATE_LPM	= 4'd9;
localparam	STATE_LPM2	= 4'd10;
localparam	STATE_ADIW	= 4'd11;
localparam	STATE_IO_BIT	= 4'd12;
localparam	STATE_MUL	= 4'd13;
localparam	STATE_ST	= 4'd14;
localparam	STATE_SLEEP	= 4'd15;

/*****************************************************************************/

/* CPU core state registers: */
reg [pmem_width-1:0] PC = 0;	/* <= 0 */
reg [3:0] state = 0;		/* <= 0: hence, STALL state should be zero! */

reg [15:0] INSTR;
reg [15:0] PREVI;

`ifdef	AVR_HAVE_WDR
reg WDR;
assign mode = (state==STATE_SLEEP?2'b01 : WDR?2'b10 : 2'b00);
`else
assign mode = (state==STATE_SLEEP?2'b01 : 2'b00);
`endif
		
reg [7:0] GPR[0:31];

`ifdef AVR_INITIAL
`ifdef SIMULATOR
localparam	init_depth = 3;
`else
localparam	init_depth = 8;
`endif
reg [init_depth-1:0] init_count = 0;
`endif

`ifdef SIMULATOR
integer i;
initial begin
	for ( i=0;i<32;i=i+1)
		GPR[i] <= 8'hA0 + i;
end
`endif

wire [15:0] RX = { GPR[27], GPR[26] };
wire [15:0] RY = { GPR[29], GPR[28] };
wire [15:0] RZ = { GPR[31], GPR[30] };

reg I, T, H, S, V, N, Z, C;	/* initialized as <= 0  */

wire [7:0] SREG = { I, T, H, S, V, N, Z, C };

reg [15:0] SP;			/* initialized as <= 0	*/


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

reg nI, nT, nH, nS, nV, nN, nZ, nC;
wire [7:0] nSREG = { nI, nT, nH, nS, nV, nN, nZ, nC };

reg [7:0] R;			/* generic result for writeback 	*/
reg [7:0] R_high;		/* result for 1-word writeback - high byte */

wire [15:0] RX_inc_dec = (INSTR[0] ? RX + 1 : RX - 1);
wire [15:0] RY_inc_dec = (INSTR[0] ? RY + 1 : RY - 1);
wire [15:0] RZ_inc_dec = (INSTR[0] ? RZ + 1 : RZ - 1);
wire [15:0] RZ_inc     = (INSTR[0] ? RZ + 1 :   RZ  );

/* arithmetic and logic operators, followed by an 8-bit immediate operand: */
/* only these instruction imply destination registers r16 ... r31 */

/* <= INSTR[] */
reg [4:0] Rd; 	

wire [4:0] Rd_prev = PREVI[8:4];

wire [4:0] Rr = { INSTR[9], INSTR[3:0] };
wire [1:0] Rd16 = INSTR[5:4];

wire [1:0] Rd16_prev = PREVI[5:4];

wire [7:0] GPR_Rd = GPR[Rd];

wire [2:0] b = INSTR[2:0];

wire [7:0] GPR_Rr = GPR[Rr];

wire [3:0] RD16 = INSTR[7:4];
wire [3:0] RR16 = INSTR[3:0];

/* used by: ANDI, ORI, SBCI, SUBI, CPI, LDI (i.e. where immediate is true): */
wire [7:0] K = { INSTR[11:8], INSTR[3:0] };
/* used by: LDD Rd, Y+q; LDD RD, Z+q; STD Y+q, Rd; STD Z+q, Rd: */
wire [5:0] q = { INSTR[13], INSTR[11:10], INSTR[2:0] };
/* used by: ADIW, SBIW: */
wire [5:0] K16 = { INSTR[7:6], INSTR[3:0] };

/* <= INSTR[] */
reg two_word_instr;


`ifdef AVR_HAVE_MUL
/* <= */
wire [4:0] Rd_mul = INSTR[15] ? INSTR[8:4] : ~INSTR[8] ? { 1'b1, INSTR[7:4] } : { 2'b10, INSTR[6:4] };
/* <= */
wire [4:0] Rr_mul = INSTR[15] ? Rr         : ~INSTR[8] ? { 1'b1, INSTR[3:0] } : { 2'b10, INSTR[2:0] };

wire [7:0] GPR_Rd_mul = GPR[Rd_mul];
wire [7:0] GPR_Rr_mul = GPR[Rr_mul];
reg [7:0] mul_rd;
reg [7:0] mul_rr;
reg [15:0] product;

/* FMUL, FMULS, FMULSU: */
/* <= */
wire fmulxx = ~INSTR[15] & (INSTR[3]  | INSTR[7]); 
/* MULS, FMULS: */
/* <= */
wire xmulsx = ~INSTR[15] & (~INSTR[8] | ({INSTR[7],INSTR[3]} == 2'b10));
/* MULS, MULSU, FMULS, FMULSU: */
/* <= */
wire xmulsu = ~INSTR[15] & (~INSTR[8] | ({INSTR[7],INSTR[3]} != 2'b01));
reg [2:0] mul_type; /* = { fmulxx, xmulsx, xmulsu }; */
`endif

/* stage2 temporary registers: */

reg [2:0] writeback;
reg change_z;
reg update_nsz;
reg [pmem_width-1:0] pc_next;
reg [15:0] sp_next;
reg sp_update;

reg [4:0] Rd_ld_save;
reg lpm_z_low; 

localparam	WRITEBACK_NONE   = 3'd0;
localparam	WRITEBACK_GPR    = 3'd1;
localparam	WRITEBACK_ZINC   = 3'd4;
localparam	WRITEBACK_ZY     = 3'd5;
localparam	WRITEBACK_X      = 3'd6;

reg [3:0] next_state;

reg  [15:0] pc_call_next;
reg  [15:0] pc_call;

wire [15:0] pc_full = { {(16-pmem_width){1'b0}}, PC };
wire [15:0] pc_full_dec = { {(16-pmem_width){1'b0}}, PC - 1'b1 };

/* data memory interface: */
reg [15:0] d_addr;
reg d_read;
reg d_write;
reg [7:0] d_out;

/* interrupts: */
wire is_tail_reti;
wire is_interrupt;

reg iflag;
reg [intr_width-1:0] ivect;

reg  is_int_enter;
reg [intr_width-1:0] ieack;

assign	in_ieack = ieack;

wire	state_normal=(state == STATE_NORMAL);

generate 
	if ( interrupt ) begin
		assign is_tail_reti = INSTR[4] & iflag;
		assign is_interrupt = I & iflag & state_normal;
	end else begin
		assign is_tail_reti = 0;
		assign is_interrupt = 0;
	end 
endgenerate

/* Instructions performing memory I/O via the dmem_* bus: */

// 16-bit instructions:

// 16'b10q0_qq0d_dddd_0qqq		LD Rd, Z+q		=> STATE_LD
// 16'b10q0_qq0d_dddd_1qqq		LD Rd, Y+q		=> STATE_LD
// 16'b10q0_qq1d_dddd_0qqq		ST Z+q, Rd		=> STATE_ST		
// 16'b10q0_qq1d_dddd_1qqq		ST Y+q, Rd		=> STATE_ST
// 16'b1001_000d_dddd_0001		LD Rd, Z++		=> STATE_LD
// 16'b1001_000d_dddd_1001		LD Rd, Y++		=> STATE_LD
// 16'b1001_001d_dddd_0001		ST Z++, Rd		=> STATE_ST
// 16'b1001_001d_dddd_1001		ST Y++, Rd		=> STATE_ST	
// 16'b1001_000d_dddd_0010		LD Rd, --Z		=> STATE_LD
// 16'b1001_000d_dddd_1010		LD Rd, --Y		=> STATE_LD
// 16'b1001_001d_dddd_0010		ST --Z, Rd		=> STATE_ST
// 16'b1001_001d_dddd_1010		ST --Y, Rd		=> STATE_ST
// 16'b1001_000d_dddd_1100		LD Rd, X		=> STATE_LD
// 16'b1001_000d_dddd_1101		LD Rd, X++		=> STATE_LD
// 16'b1001_000d_dddd_1110		LD Rd, --X		=> STATE_LD
// 16'b1001_001d_dddd_1100		ST X, Rd		=> STATE_ST
// 16'b1001_001d_dddd_1101		ST X++, Rd		=> STATE_ST
// 16'b1001_001d_dddd_1110		ST --X, Rd		=> STATE_ST
// 16'b1001_000d_dddd_1111		POP Rd	== LD Rd, ++SP	=> STATE_LD
// 16'b1001_001d_dddd_1111		PUSH Rd == ST SP--, Rd	=> STATE_ST

// 16'b1001_000d_dddd_1101              LD Rd, X++		=> STATE_LD
// 16'b1001_000d_dddd_1110              LD Rd, --X		=> STATE_LD
// 16'b1001_001d_dddd_1101              ST X++, Rd		=> STATE_ST
// 16'b1001_001d_dddd_1110              ST --X, Rd		=> STATE_ST

// 16'b1001_000d_dddd_1001              LD Rd, Y++		=> STATE_LD
// 16'b1001_000d_dddd_1010              LD Rd, --Y		=> STATE_LD
// 16'b1001_001d_dddd_1001              ST Y++, Rd     		=> STATE_ST
// 16'b1001_001d_dddd_1010              ST --Y, Rd		=> STATE_ST

// 16'b1001_000d_dddd_0001              LD Rd, Z++		=> STATE_LD
// 16'b1001_000d_dddd_0010              LD Rd, --Z		=> STATE_LD
// 16'b1001_001d_dddd_0001              ST Z++, Rd		=> STATE_ST
// 16'b1001_001d_dddd_0010              ST --Z, Rd		=> STATE_ST

// 16'b1101_xxxx_xxxx_xxxx		RCALL			=> STATE_CALL => STATE_ST
// 16'b1001_0101_0000_1001		ICALL			=> STATE_CALL => STATE_ST

// 16'b1001_0101_000x_1000		RET, RETI		=> STATE_RET => STATE_RET2  => STATE_STALL

// 32-bit instructions:						

// 16'b1001_000x_xxxx_0000		LDS			=> STATE_LD
// 16'b1001_001x_xxxx_0000		STS			=> STATE_ST
// 16'b1001_010x_xxxx_111x		CALL			=> STATE_CALL => STATE_ST

// xxxx					interrupt generation	=> STATE_CALL => STATE_ST


// INSTR[15:10] == 6'b100100
// X increased/decreased: INSTR[3:2] == 2'b11	
// Y increased/decreased: INSTR[3:2] == 2'b10
// Z increased/decreased: INSTR[3:2] == 2'b00
// Z increased		  INSTR[3:2] == 2'b01	

/* setting up memory interface lines (d_addr, d_out, d_read and d_write): */

reg [4:0] d_equiv_dest;
reg [15:0] d_equiv_instr;
reg d_inext_equiv;

always @(*) begin

	d_out = 0;
	d_addr = 0;
	d_read = 0;
	d_write = 0;

	case (state)

	    STATE_NORMAL: begin

		casex (INSTR)

		    16'b10x0_xx0x_xxxx_xxxx: begin	/* IN Rd, <d_addr> | MOV Rd, R<d_addr> */
			/* LD Rd, Z+q */
			/* LD Rd, Y+q */
			d_addr = (~INSTR[3]?RZ:RY) + q;
			d_read = 1;
			end

		    16'b10x0_xx1x_xxxx_xxxx: begin	/* OUT <d_addr>, Rd | MOV R<d_addr>, Rd */
			/* ST Z+q, Rd */
			/* ST Y+q, Rd */
			d_addr = (~INSTR[3]?RZ:RY) + q;
			d_out = GPR_Rd;
			d_write = 1;
			end

		    16'b1001_000x_xxxx_x001: begin	/* IN Rd, <d_addr> | MOV Rd, R<d_addr> */
			/* LD Rd, Z++ */
			/* LD Rd, Y++ */
			d_addr = (~INSTR[3]?RZ:RY);
			d_read = 1;
		    	end

		    16'b1001_001x_xxxx_x001: begin	/* OUT <d_addr>, Rd | MOV R<d_addr>, Rd */
			/* ST Z++, Rd */
			/* ST Y++, Rd */
			d_addr = (~INSTR[3]?RZ:RY);
			d_out = GPR_Rd;
			d_write = 1;
		    	end

		    16'b1001_000x_xxxx_x010: begin	/* IN Rd, <d_addr> | MOV Rd, R<d_addr> */
			/* LD Rd, --Z */
			/* LD Rd, --Y */
			d_addr = (~INSTR[3]?RZ:RY) - 1;
			d_read = 1;
		    	end

		    16'b1001_001x_xxxx_x010: begin	/* OUT <d_addr>, Rd | MOV R<d_addr>, Rd */
			/* ST --Z, Rd */
			/* ST --Y, Rd */
			d_addr = (~INSTR[3]?RZ:RY) - 1;
			d_out = GPR_Rd;
			d_write = 1;
		    	end

		    16'b1001_000x_xxxx_110x: begin	/* IN Rd, <d_addr> | MOV Rd, R<d_addr> */
			/* LD Rd, X */
			/* LD Rd, X++ */
			d_addr = RX;
			d_read = 1;
			end
		    16'b1001_000x_xxxx_1110: begin	/* IN Rd, <d_addr> | MOV Rd, R<d_addr> */
			/* LD Rd, --X */
			d_addr = RX - 1;
			d_read = 1;
			end

		    16'b1001_001x_xxxx_110x: begin	/* OUT <d_addr>, Rd | MOV R<d_addr>, Rd */
			/* ST X, Rd */
			/* ST X++, Rd */
			d_addr = RX;
			d_out = GPR_Rd;
			d_write = 1;
			end

		    16'b1001_001x_xxxx_1110: begin	/* OUT <d_addr>, Rd | MOV R<d_addr>, Rd */
			/* ST --X, Rd */
			d_addr = RX - 1;
			d_out = GPR_Rd;
			d_write = 1;
			end

		    16'b1001_000x_xxxx_1111: begin	/* undefined for registers or I/O space */
			/* POP Rd -- LD Rd, ++SP */
			d_addr = SP + 1;
			d_read = 1;
			end

		    16'b1001_001x_xxxx_1111: begin	/* might be undefined for registers or I/O space */
			/* PUSH Rd -- ST SP--, Rd */
			d_addr = SP;
			d_out = GPR_Rd;
			d_write = 1;
			end

		    16'b1101_xxxx_xxxx_xxxx, 
		    16'b1001_0101_0000_1001: begin	/* might be undefined for registers or I/O space */
			/* RCALL */
			/* ICALL */
			d_addr = SP;

			if ( lsb_call )
				d_out = pc_full[15:8];
			else
				d_out = pc_full[7:0];

			d_write = 1;
			end

		    16'b1001_0101_000x_1000: begin	/* might be undefined for registers or I/O space */
			/* RET, RETI */
			d_addr = SP + 1;
			d_read = 1;
			end

		endcase

		if (is_interrupt) begin
			d_addr = SP;
			if ( lsb_call )
				d_out = pc_full_dec[15:8];
			else
				d_out = pc_full_dec[7:0];
			d_write = 1;

			/* many thanks for Jesus Arias Alvarez for alluding the lack of this line below: */
			d_read = 0;	
		end

		end

	    STATE_TWOWORD: begin
		casex(PREVI)

		    16'b1001_000x_xxxx_0000: begin	/* IN Rd, <d_addr> | MOV Rd, R<d_addr> */
			/* LDS Rd, 0xXXXX */
			d_addr = INSTR;
			d_read = 1;
			end

		    16'b1001_001x_xxxx_0000: begin	/* OUT <d_addr>, Rd | MOV R<d_addr>, Rd */
			/* STS 0xXXXX, Rd */
			d_addr = INSTR;
			d_out = GPR[Rd_prev];
			d_write = 1;
			end

		`ifdef AVR_HAVE_22BITPC			/* undefined for registers or I/O space */
		    16'b1001_010x_xxxx_111x: begin
			/* CALL k */
			d_addr = SP;

			if ( lsb_call )
				d_out = pc_full[15:8];
			else
				d_out = pc_full[7:0];
		
			d_write = 1;
			end
		`endif

		endcase

		end

	    STATE_CALL: begin
		/* RCALL */
		/* ICALL */
		d_addr = SP;
		if ( lsb_call )
			d_out = pc_full[7:0];
		else
			d_out = pc_full[15:8];
		d_write = 1;
		end

	    STATE_RET: begin
		d_addr = SP + 1;
		d_read = 1;
		end

	endcase	

	d_inext_equiv = 0;
	d_equiv_instr = 16'bxxxx_xxxx_xxxx_xxxx;

	if ( ld_st_io_reg & ~is_interrupt ) begin

		d_equiv_dest = (state_normal ? INSTR[8:4] : PREVI[8:4]);
	
		if ( d_read ) begin 
			// translate to MOV Rd, R<d_addr>	
			if ( d_addr < 32 ) begin
				d_equiv_instr = { 6'b0010_11, d_addr[4], d_equiv_dest, d_addr[3:0] };
				d_inext_equiv = 1;
				d_read = 0;
			// translate to IN Rd, <d_addr>-32
			end else if ( d_addr < 96 ) begin
				d_equiv_instr = { 5'b1011_0, ~d_addr[5], d_addr[4], d_equiv_dest, d_addr[3:0] };
				d_inext_equiv = 1;
				d_read = 0;
			end 
		end else if ( d_write ) begin
			// translate to MOV R<d_addr>, Rd
			if ( d_addr < 32 ) begin
				d_equiv_instr = { 6'b0010_11, d_equiv_dest[4], d_addr[4:0], d_equiv_dest[3:0] };
				d_inext_equiv = 1;
				d_write = 0;
			// translate to OUT <d_addr>-32, Rd
			end else if ( d_addr < 96 ) begin
				d_equiv_instr = { 5'b1011_1, ~d_addr[5], d_addr[4], d_equiv_dest, d_addr[3:0] };
				d_inext_equiv = 1;
				d_write = 0;
			end 
		end 

	end 


end

assign  dmem_a  = d_addr[dmem_width-1:0] - MEM_OFFSET;
assign	dmem_re = d_read;
assign	dmem_we = d_write;
assign	dmem_do = d_out; 


/* IN/OUT assignments: */

/* used by: IN, OUT, SBIC, SBIS, CBI, SBI: */
// 16'b1011_0aad_dddd_aaaa: 	/* IN	*/
// 16'b1011_1aad_dddd_aaaa:	/* OUT	*/
// 16'b1001_1000_aaaa_abbb: 	/* CBI	*/
// 16'b1001_1010_aaaa_abbb: 	/* SBI	*/
// 16'b1001_1001_aaaa_abbb:	/* SBIC */
// 16'b1001_1011_aaaa_abbb:	/* SBIS */

reg [7:0] Rio;
reg [5:0] a;
reg	io_instr_rw;
reg	io_instr_bf;

reg	a_61,a_62,a_63;

wire	io_act = ~is_interrupt & io_instr_rw & state_normal;

assign	io_a = (state == STATE_IO_BIT ? {1'b0, PREVI[7:3]} : a);
assign	io_do = (state == STATE_IO_BIT ? Rio : GPR_Rd);
assign	io_re = (io_act & (INSTR[11]==1'b0)) | (state_normal & io_instr_bf & ~is_interrupt);
assign	io_we = (io_act & (INSTR[11]==1'b1)) | (state == STATE_IO_BIT);

wire [7:0] Rin = (a_61?(SP[7:0]):(a_62?(SP[15:8]):(a_63?SREG:io_di)));

/*
reg [7:0] Rin;
always @(*) begin
	if (a==6'b111101) begin
		Rin = SP[7:0];
	end else if (a==6'b111110) begin
		Rin = SP[15:8];
	end else if (a==6'b111111) begin
		Rin = SREG;
	end else begin
		Rin = io_di;
	end
end
*/

/* CPU instruction pipeline, stage 1: */

assign pmem_ce = 1'b1;

assign pmem_a = rst ? 0 : pc_next;
// assign pmem_a = PC;
/* always @(posedge clk) pmem_d <= FLASH[pmem_a]; */	
// assign INSTR = pmem_d;

wire [15:0] M_INSTR = (d_inext_equiv ? d_equiv_instr : pmem_d);

wire immediate = (M_INSTR[15:14] == 2'b01  ) | 	/* ANDI, ORI, SBCI, SUBI */
		 (M_INSTR[15:12] == 4'b0011) |	/* CPI			 */
		 (M_INSTR[15:12] == 4'b1110);	/* LDI			 */

wire two_word_lds_sts = ((M_INSTR[15:10]==6'b100100) & (M_INSTR[3:0]==4'b0000));

`ifdef AVR_HAVE_22BITPC
/* this is for avr3 (avr31, avr35) and avr5 (avr51): */
/* <= */
wire two_word_jmp_call = ((M_INSTR[15:9]==7'b1001010) & (M_INSTR[3:2]==2'b11));
/* <= */
wire M_two_word_instr = two_word_lds_sts | two_word_jmp_call;
`else
/* otherwise (avr2, avr25 and avr4): */
/* <= */
wire M_two_word_instr = two_word_lds_sts;
`endif

wire [5:0] M_a = M_INSTR[13] ? {M_INSTR[10:9], M_INSTR[3:0]} : {1'b0, M_INSTR[7:3]};

`ifdef	AVR_HAVE_WDR
reg i_wdr;
`endif

always @(posedge clk) begin

	INSTR <= M_INSTR;

	Rd <= { immediate | M_INSTR[8], M_INSTR[7:4] };
	two_word_instr <= M_two_word_instr;

	a <= M_a;
	io_instr_rw <= (M_INSTR[15:12]==4'b1011);
	io_instr_bf <= (M_INSTR[15:10]==6'b1001_10);
	a_61 <= (M_a == 6'd61);
	a_62 <= (M_a == 6'd62);
	a_63 <= (M_a == 6'd63);

	PREVI <= INSTR;

	iflag <= in_iflag & I;
	ivect <= in_ivect;

end /* always @(posedge clk) */

/* CPU instruction pipeline, stage 2: */

always @(*) begin

	R = 0;
	R_high = 0;

	writeback = WRITEBACK_NONE ;
	change_z = 1;
	update_nsz = 0;

	next_state = STATE_NORMAL;
	pc_next = PC;

	sp_next = SP;
	sp_update = 0;

	pc_call_next = 0;

	is_int_enter = 0;

	`ifdef AVR_HAVE_WDR
	i_wdr = 0;
	`endif

	{ nI, nT, nH, nS, nV, nN, nZ, nC } = SREG;

	case(state)

	    STATE_NORMAL: begin

		casex(INSTR) 

		`ifdef AVR_HAVE_MOVW
		    16'b0000_0001_xxxx_xxxx: begin
			R = GPR[2*RR16+0];
                	R_high = GPR[2*RR16+1];
			pc_next = PC + 1;
			end
		`endif

		`ifdef AVR_HAVE_MUL
		    16'b1001_11xx_xxxx_xxxx,
		    16'b0000_001x_xxxx_xxxx: begin
			/* MUL */
			/* MULS */
			/* MULSU */
			/* FMUL */
			/* FMULS */
			/* FMULSU */
			next_state = STATE_MUL;
			end
		`endif

		    16'b000x_10xx_xxxx_xxxx, /* subtract */
		    16'b000x_01xx_xxxx_xxxx: /* compare  */ begin
			/* SUB - SBC / CP - CPC */
			{nC, R} = GPR_Rd - GPR_Rr - (~INSTR[12] & C);
			nH = (~GPR_Rd[3] & GPR_Rr[3])|(GPR_Rr[3] & R[3])|(R[3] & ~GPR_Rd[3]);
			nV = (GPR_Rd[7] & ~GPR_Rr[7] & ~R[7])|(~GPR_Rd[7] & GPR_Rr[7] & R[7]);
			update_nsz = 1;
			if (~INSTR[12])
				change_z = 1'b0;
			if (INSTR[11])
				writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b000x_11xx_xxxx_xxxx: begin
			/* ADD - ADC */
			{nC, R} = GPR_Rd + GPR_Rr + (INSTR[12] & C);
			nH = (GPR_Rd[3] & GPR_Rr[3])|(GPR_Rr[3] & ~R[3])|(~R[3] & GPR_Rd[3]);
			nV = (GPR_Rd[7] & GPR_Rr[7] & ~R[7])|(~GPR_Rd[7] & ~GPR_Rr[7] & R[7]);
			update_nsz = 1;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b010x_xxxx_xxxx_xxxx, /* subtract */
		    16'b0011_xxxx_xxxx_xxxx: /* compare  */ begin
			/* SUBI - SBCI / CPI */
			{nC, R} = GPR_Rd - K - (~INSTR[12] & C);
			nH = (~GPR_Rd[3] & K[3])|(K[3] & R[3])|(R[3] & ~GPR_Rd[3]);
			nV = (GPR_Rd[7] & ~K[7] & ~R[7])|(~GPR_Rd[7] & K[7] & R[7]);
			update_nsz = 1;
			if (~INSTR[12])
				change_z = 1'b0;
			if (INSTR[14])
				writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end

		    16'b0111_xxxx_xxxx_xxxx: begin
			/* ANDI Rd, K; */
			R = GPR_Rd & K;
			nV = 1'b0; 
			update_nsz = 1;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end

		    16'b0110_xxxx_xxxx_xxxx: begin
			/* ORI Rd, K; */
			R = GPR_Rd | K;
			nV = 1'b0; 
			update_nsz = 1;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end

		    16'b0010_00xx_xxxx_xxxx: begin
			/* AND */
			R = GPR_Rd & GPR_Rr;
	                nV = 1'b0;
			update_nsz = 1;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b0010_01xx_xxxx_xxxx: begin
			/* EOR */
			R = GPR_Rd ^ GPR_Rr;
	                nV = 1'b0;
			update_nsz = 1;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b0010_10xx_xxxx_xxxx: begin
			/* OR */
			R = GPR_Rd | GPR_Rr;
	                nV = 1'b0;
			update_nsz = 1;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b0010_11xx_xxxx_xxxx: begin
			/* MOV */
			R = GPR_Rr;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b1001_010x_xxxx_0000: begin
			/* COM */
			R = ~GPR_Rd;
			nV = 1'b0;
			nC = 1'b1;
			update_nsz = 1;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b1001_010x_xxxx_0001: begin
			/* NEG */
			{nC, R} = 8'h00 - GPR_Rd;
			nH = R[3] | GPR_Rd[3];
			nV = (R == 8'h80);
			update_nsz = 1;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b1001_010x_xxxx_0011: begin
			/* INC */
			R = GPR_Rd + 8'd1;
			nV = (R == 8'h80);
			update_nsz = 1;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b1001_010x_xxxx_1010: begin
			/* DEC */
			R = GPR_Rd - 8'd1;
			nV = (R == 8'h7f);
			update_nsz = 1;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b1001_010x_xxxx_011x: begin
			/* LSR - ROR */
			R = {INSTR[0] & C, GPR_Rd[7:1]};
			nC = GPR_Rd[0];
			nV = R[7] ^ GPR_Rd[0];
			update_nsz = 1;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b1001_010x_xxxx_0101: begin
			/* ASR */
			R = {GPR_Rd[7], GPR_Rd[7:1]};
			nC = GPR_Rd[0];
			nV = R[7] ^ GPR_Rd[0];
			update_nsz = 1;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b1001_010x_xxxx_0010: begin
			/* SWAP */
			R = {GPR_Rd[3:0], GPR_Rd[7:4]};
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b1001_0100_xxxx_1000: begin
			/* BSET - BCLR */
			case(INSTR[7:4])
				4'b0000: nC = 1'b1;
				4'b0001: nZ = 1'b1;
				4'b0010: nN = 1'b1;
				4'b0011: nV = 1'b1;
				4'b0100: nS = 1'b1;
				4'b0101: nH = 1'b1;
				4'b0110: nT = 1'b1;
				4'b0111: nI = 1'b1;
				4'b1000: nC = 1'b0;
				4'b1001: nZ = 1'b0;
				4'b1010: nN = 1'b0;
				4'b1011: nV = 1'b0;
				4'b1100: nS = 1'b0;
				4'b1101: nH = 1'b0;
				4'b1110: nT = 1'b0;
				4'b1111: nI = 1'b0;
			endcase
			pc_next = PC + 1;
			end

		    16'b1001_011x_xxxx_xxxx: begin
			/* SBIW */
			/* ADIW */
			if(INSTR[8]) begin	/* SBIW */
				{nC, R} = GPR[24+2*Rd16] - K16;
			end else begin		/* ADIW */
				{nC, R} = GPR[24+2*Rd16] + K16;
			end
			nZ = (R==0);
			next_state = STATE_ADIW;
			end

		    16'b1001_00xx_xxxx_0000: begin
			/* LDS Rd, 0xXXXX */
			/* STS 0xXXXX, Rd */
			pc_next = PC + 1;
			next_state = STATE_TWOWORD;
			end

		`ifdef AVR_HAVE_22BITPC
		    16'b1001_010x_xxxx_11xx: begin
			/* JMP k */
			/* CALL k */
			pc_next = PC + 1;
			next_state = STATE_TWOWORD;
			end
		`endif

		    16'b10x0_xx0x_xxxx_xxxx: begin
			/* LD Rd, Z+q; */
			/* LD Rd, Y+q; */
			next_state = STATE_LD; 
			end

		    16'b10x0_xx1x_xxxx_xxxx: begin
			/* ST Z+q, Rd */
			/* ST Y+q, Rd */
			// pc_next = PC + 1;
			next_state = STATE_ST;
			end

		    16'b1001_000x_xxxx_x001: begin
			/* LD Rd, Z++ */
			/* LD Rd, Y++ */
			next_state = STATE_LD;
			/* save dest register for the next cycle: */
			writeback = WRITEBACK_ZY;
			end

		    16'b1001_001x_xxxx_x001: begin
			/* ST Z++, Rd */
			/* ST Y++, Rd */
			// pc_next = PC + 1;
			next_state = STATE_ST;
			writeback = WRITEBACK_ZY;			
			end

		    16'b1001_000x_xxxx_x010: begin
			/* LD Rd, --Z */
			/* LD Rd, --Y */
			next_state = STATE_LD;
			/* save dest register for the next cycle: */
			writeback = WRITEBACK_ZY;
			end

		    16'b1001_001x_xxxx_x010: begin
			/* ST --Z, Rd */
			/* ST --Y, Rd */
			// pc_next = PC + 1;
			next_state = STATE_ST;
			writeback = WRITEBACK_ZY;			
			end

		    16'b1001_000x_xxxx_1100: begin
			/* LD Rd, X */
			next_state = STATE_LD;
			end

		    16'b1001_001x_xxxx_1100: begin
			/* ST X, Rd */
			// pc_next = PC + 1;
			next_state = STATE_ST;
			end

		    16'b1001_000x_xxxx_1101: begin
			/* LD Rd, X++ */
			writeback = WRITEBACK_X;
			next_state = STATE_LD;
			end

		    16'b1001_001x_xxxx_1101: begin
			/* ST X++, Rd */
			writeback = WRITEBACK_X;
			pc_next = PC + 1;
			end

		    16'b1001_000x_xxxx_1110: begin
			/* LD Rd, --X */
			writeback = WRITEBACK_X;
			next_state = STATE_LD;
			end

		    16'b1001_001x_xxxx_1110: begin
			/* ST --X, Rd */
			// pc_next = PC + 1;
			next_state = STATE_ST;
			writeback = WRITEBACK_X;
			end

		    16'b1001_000x_xxxx_1111: begin
			/* POP Rd */	/* LD Rd, ++SP */
			sp_next = SP + 1;
			sp_update = 1;
			next_state = STATE_LD;
			end

		    16'b1001_001x_xxxx_1111: begin
			/* PUSH Rd */	/* ST SP--, Rd */
			sp_next = SP - 1;
			sp_update = 1;
			next_state = STATE_ST;
			end

		    16'b1100_xxxx_xxxx_xxxx: begin
			/* RJMP */
			next_state = STATE_STALL;
			pc_next = PC + { {4{INSTR[11]}}, INSTR[11:0] };
			end
		    16'b1001_0100_0000_1001: begin
			/* IJMP */
			next_state = STATE_STALL;
			pc_next = RZ;
			end

		    16'b1101_xxxx_xxxx_xxxx: begin
			/* RCALL */
			pc_call_next = pc_full + { {4{INSTR[11]}}, INSTR[11:0] };
			sp_next = SP - 1;
			sp_update = 1;
			next_state = STATE_CALL;
			end
		    16'b1001_0101_0000_1001: begin
			/* ICALL */
			pc_call_next = RZ;
			sp_next = SP - 1;
			sp_update = 1;
			next_state = STATE_CALL;
			end

		    16'b1001_0101_0000_1000,		/* INSTR: 0x9508 */
		    16'b1001_0101_0001_1000: begin	/* INSTR: 0x9518 */
			/* RET */
			/* RETI */

			if ( is_tail_reti ) begin

				// Tail chaining: `RETI` is equivalent to 
				// `JMP ivect` if there is a pending interrupt:
				next_state = STATE_STALL;

				`ifdef	AVR_2WORD_INTERRUPTS
				pc_next = { ivect, 1'b0 };
				`else
				pc_next = ivect;
				`endif

				is_int_enter = 1;
			end else begin 
				// Otherwise, RETI and RET is the same...
				next_state = STATE_RET;
				sp_next = SP + 1;
				sp_update = 1;
				// ... besides that RETI sets the I flag: 
				if (INSTR[4])
					nI = 1'b1;

			end

			end

		`ifdef AVR_HAVE_SLEEP
		    16'b1001_0101_1000_1000: begin	/* INSTR: 0x9588 */
			/* SLEEP */
			next_state = STATE_SLEEP;
			end
		`endif

		`ifdef AVR_HAVE_WDR
		    16'b1001_0101_1010_1000: begin	/* INSTR: 0x95A8 */
			/* WDR */
			pc_next = PC + 1;
			i_wdr = 1;
			end
		`endif

		    16'b1001_0101_110x_1000: begin
			/* LPM */
			/* ELPM */
			pc_call_next = PC;
			pc_next = RZ[pmem_width:1];
			next_state = STATE_LPM;
			end

//		`ifdef AVR_HAVE_SPM
//		    16'b1001_0101_111x_1000: begin
//			/* SPM Z */
//			/* SPM Z+ */
//			pc_call_next = PC;
//			pc_next = RZ[pmem_width:1];
//			next_state = STATE_LPM;
//			end
//		`endif

		`ifdef AVR_HAVE_LPMZ
		    16'b1001_000x_xxxx_01xx: begin
			/* LPM Rd, Z */
			/* LPM Rd, Z++ */
			/* ELPM Rd, Z */
			/* ELPM Rd, Z++ */
			pc_call_next = PC;
			pc_next = RZ[pmem_width:1];
			next_state = STATE_LPM;
			writeback = WRITEBACK_ZINC;
			end
		`endif	

		    16'b1111_0xxx_xxxx_xxxx: begin
			/* BRxS - BRxC */
			if (SREG[b] ^ INSTR[10]) begin
				next_state = STATE_STALL;
				pc_next = PC + { {9{INSTR[9]}}, INSTR[9:3] };
			end else begin
				pc_next = PC + 1;
			end
			end			
		    16'b1111_11xx_xxxx_0xxx: begin
			/* SBRC */
			/* SBRS */
			if (GPR_Rd[b] == INSTR[9]) begin
				next_state = STATE_SKIP;
			end
			pc_next = PC + 1;
			end
		    16'b1001_10x0_xxxx_xxxx: begin
			/* CBI */
			/* SBI */
                        next_state = STATE_IO_BIT;
			end
		    16'b1001_10x1_xxxx_xxxx: begin
			/* SBIC */
			/* SBIS */
			if (Rin[b]==INSTR[9]) begin
				next_state = STATE_SKIP;
			end
			pc_next = PC + 1;
			end
		    16'b0001_00xx_xxxx_xxxx: begin
			/* CPSE */
			if (GPR_Rd == GPR_Rr) begin
				next_state = STATE_SKIP;
			end
			pc_next = PC + 1;
			end			
		    16'b1110_xxxx_xxxx_xxxx: begin
	                /* LDI */
			R = K;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b1111_10xx_xxxx_0xxx: begin
			/* BST */
			/* BLD */
			if (INSTR[9]) begin	/* BST */
				nT = GPR_Rd[b];
			end else begin		/* BLD */
				case (b)
					3'd0: R = { GPR_Rd[7:1], T };
					3'd1: R = { GPR_Rd[7:2], T, GPR_Rd[0] };
					3'd2: R = { GPR_Rd[7:3], T, GPR_Rd[1:0] };
					3'd3: R = { GPR_Rd[7:4], T, GPR_Rd[2:0] };
					3'd4: R = { GPR_Rd[7:5], T, GPR_Rd[3:0] };
					3'd5: R = { GPR_Rd[7:6], T, GPR_Rd[4:0] };
					3'd6: R = { GPR_Rd[7], T, GPR_Rd[5:0] };
					3'd7: R = { T, GPR_Rd[6:0] };
				endcase
				writeback = WRITEBACK_GPR; 
			end
			pc_next = PC + 1;
			end
		    16'b1011_0xxx_xxxx_xxxx: begin
			/* IN */
			R = Rin;
			writeback = WRITEBACK_GPR;
			pc_next = PC + 1;
			end
		    16'b1011_1xxx_xxxx_xxxx: begin
			/* OUT */
			if ( a_61 ) begin			/* SPL */
				sp_next = { SP[15:8], GPR_Rd };
				sp_update = 1;
			end else if ( a_62 ) begin		/* SPH */
				sp_next = { GPR_Rd, SP[7:0] };
				sp_update = 1;
			end else if ( a_63 ) begin		/* SREG */
				{ nI, nT, nH, nS, nV, nN, nZ, nC } = GPR_Rd;
			end
			// in all other cases, the data flow is handled
			// by the IN/OUT persistent assignments (see earlier)
			pc_next = PC + 1;
			end

		    default:
			/* NOP */
			/* ignore unknown, 1-word instructions (like NOP or unimplemented ones): */
			pc_next = PC + 1;

		endcase
		
		if (update_nsz) begin
			nN = R[7];
			nS = nN ^ nV;
			nZ = (R == 8'h00) & (change_z|Z);
		end

		if (is_interrupt) begin
			// An interrupt is equivalent to a CALL, however,
			// the actual program counter needed to be saved, not 
			// the PC corresponding to the following instruction.
			// Due to the two-stage pipeline, the current 
			// instruction is PC - 1, and not PC: 
			writeback = 0;
			`ifdef	AVR_2WORD_INTERRUPTS
			pc_call_next = { ivect, 1'b0 } ;
			`else
			pc_call_next = ivect;
			`endif
			sp_next = SP - 1;
			sp_update = 1;
			next_state = STATE_CALL;
			pc_next = PC - 1;
			// nI = 0;
			{ nI, nT, nH, nS, nV, nN, nZ, nC } = { 1'b0, SREG[6:0] };
			is_int_enter = 1;
		end

		end /* STATE_NORMAL */
 
	    STATE_TWOWORD: begin
		casex(PREVI)

		    16'b1001_000x_xxxx_0000: begin
			/* LDS Rd, 0xXXXX */
			next_state = STATE_LD; 
			end

		    16'b1001_001x_xxxx_0000: begin
			/* STS 0xXXXX, Rd */
			// pc_next = PC + 1;
			next_state = STATE_ST;
			end

		`ifdef AVR_HAVE_22BITPC
		    16'b1001_010x_xxxx_110x: begin
			/* JMP K */
			next_state = STATE_STALL;
			pc_next = INSTR;
			end
		    16'b1001_010x_xxxx_111x: begin
			/* CALL K */
			pc_call_next = INSTR;
			sp_next = SP - 1;
			sp_update = 1;
			next_state = STATE_CALL;
			end
		`endif

		    default:
			pc_next = PC + 1;

		endcase			

		end /* STATE_TWOWORD */

	    STATE_STALL: begin
		pc_next = PC + 1;
		next_state = STATE_NORMAL;
		end /* STATE_STALL */

	    STATE_LD: begin
		pc_next = PC + 1;
		next_state = STATE_NORMAL;
		end /* STATE_LD */

	    STATE_ST: begin
		pc_next = PC + 1;
		next_state = STATE_NORMAL;
		end /* STATE_ST */

	    STATE_CALL: begin
		sp_next = SP - 1;
		sp_update = 1;
		pc_next = pc_call; 
		next_state = STATE_ST;
		end /* STATE_CALL */

	    STATE_RET: begin

		if ( lsb_call )
			pc_call_next = { 8'h00, dmem_di };
		else
			pc_call_next = { dmem_di, 8'h00 };

		sp_next = SP + 1;
		sp_update = 1;
		next_state = STATE_RET2;
		end /* STATE_RET */

	    STATE_RET2: begin

		if ( lsb_call )
			pc_next = { dmem_di, pc_call[ 7:0] };
		else
			pc_next = { pc_call[15:8], dmem_di };

		next_state = STATE_STALL;
		end /* STATE_RET2 */

	    STATE_SKIP: begin
		if (two_word_instr)	next_state = STATE_STALL;
		else			next_state = STATE_NORMAL;
		pc_next = PC + 1;
		end /* STATE_SKIP */

	    STATE_LPM: begin
		pc_next = pc_call;
		next_state = STATE_LPM2;
		end

	    STATE_LPM2: begin
		pc_next = PC + 1;
		next_state = STATE_NORMAL;
		end

	    STATE_ADIW: begin
		if (PREVI[8]) begin
			/* SBIW */
			{nC, R_high} = GPR[24+2*Rd16_prev+1] - C;
			nV =  GPR[24+2*Rd16_prev+1][7] & ~R_high[7];
		end else begin
			/* ADIW */
			{nC, R_high} = GPR[24+2*Rd16_prev+1] + C;
			nV = ~GPR[24+2*Rd16_prev+1][7] &  R_high[7];
		end
		nN = R_high[7];
		nS = nN ^ nV;
		nZ = (R_high==0) & Z;
		pc_next = PC + 1;
		next_state = STATE_NORMAL;
		end

            STATE_IO_BIT: begin
                pc_next = PC + 1;
                next_state = STATE_NORMAL;
                end


	`ifdef AVR_INITIAL
	    STATE_INITIAL: begin
		if (init_count[init_depth-1] == 1'b1)
			next_state = STATE_STALL;
		else
			next_state = STATE_INITIAL;
                end
	`endif

	`ifdef AVR_HAVE_MUL
	    STATE_MUL: begin

		{ R_high, R } = product;
		if ( mul_type[0] & mul_rd[7] )
			R_high = R_high - mul_rr;
		if ( mul_type[1] & mul_rr[7] )
			R_high = R_high - mul_rd;

		nZ = ( {R_high, R} == 16'h0000 );
		nC = R_high[7];

		if ( mul_type[2] ) begin
			{ R_high, R } = { R_high[6:0], R, 1'b0 };
		end

		pc_next = PC + 1;
		next_state = STATE_NORMAL;
		end
	`endif

	`ifdef AVR_HAVE_SLEEP
	    STATE_SLEEP: begin

		/* Handling sleep mode is in accordance with the specifications and common sense:	*/
		/*  - exit only if interrupts are enabled (I) and there is a pending interrupt;		*/
		/*  - if there is no reason from exiting, continue being in sleep mode; and		*/
		/*  - otherwise, being in a sleep state is equivalent to being in stall state.		*/
		if ( I & iflag ) begin
			pc_next = PC + 1;
			next_state = STATE_NORMAL;
		end else begin
			next_state = STATE_SLEEP;
		end

		end
	`endif

	endcase

	if ( d_inext_equiv ) begin
		next_state = STATE_NORMAL;
		pc_next = PC;
	end

end /* always @(*) */

// Note: if `interrupt` is 1, then after this point:
//  - state is STATE_NORMAL,
//  - writeback is 0,
//  - next_state is STATE_CALL.

`ifdef AVR_HAVE_MUL
always @(posedge clk) if (next_state==STATE_MUL) begin
	product <= GPR_Rd_mul * GPR_Rr_mul;
	mul_rd <= GPR_Rd_mul;
	mul_rr <= GPR_Rr_mul;
	mul_type <= { fmulxx, xmulsx, xmulsu };
end
`endif

always @(posedge clk) begin

	`ifdef AVR_HAVE_MUL
	if (state==STATE_MUL) begin
	/* writeback: after two-cycle MUL: */
		GPR[0] <= R;
		GPR[1] <= R_high;
	end else
	`endif

	`ifdef AVR_HAVE_MOVW
	/* writeback: after single cycle MOVW: */
	if (state_normal && INSTR[15:8]==8'b0000_0001 && ~is_interrupt) begin
		GPR[2*RD16+0] <= R;			// GPR[2*RR16+0];
		GPR[2*RD16+1] <= R_high;		// GPR[2*RR16+1];
	end else
	`endif

	/* writeback: after the first and second cycle of ADIW and SUBW: */
	if (next_state==STATE_ADIW) 
		GPR[24+2*Rd16+0] <= R;
	else if (state==STATE_ADIW) 
		GPR[24+2*Rd16_prev+1] <= R_high;
	/* writeback after LD: */
	else if ( state==STATE_LD )
		GPR[Rd_ld_save] <= dmem_di;
	else if ( state==STATE_LPM2 ) begin
		if (~lpm_z_low)	GPR[Rd_ld_save] <= INSTR[7:0];
		else		GPR[Rd_ld_save] <= INSTR[15:8];
	end else begin

	/* writeback: all of the another cases (ALU + X/Y/Z inc/dec): */
	case (writeback)
	    WRITEBACK_GPR: begin
		GPR[Rd] <= R;
		end
	    WRITEBACK_ZINC: begin
		GPR[30] <= RZ_inc[7:0];
		GPR[31] <= RZ_inc[15:8];
		end	
	    WRITEBACK_ZY: begin
		if (~INSTR[3]) begin
			GPR[30] <= RZ_inc_dec[7:0];
			GPR[31] <= RZ_inc_dec[15:8];
		end else begin
			GPR[28] <= RY_inc_dec[7:0]; 
			GPR[29] <= RY_inc_dec[15:8];
		end
		end
	    WRITEBACK_X: begin
		GPR[26] <= RX_inc_dec[7:0]; 
		GPR[27] <= RX_inc_dec[15:8];
		end
	endcase

	end

	if (~INSTR[9])	Rio <= Rin & ~(8'h01 << b);
	else		Rio <= Rin |  (8'h01 << b);

	pc_call <= pc_call_next;

	if (next_state == STATE_LD)
		Rd_ld_save <= (state_normal ? Rd : Rd_prev); 
	else if (next_state == STATE_LPM) begin
		Rd_ld_save <= (INSTR[10] ? 5'b00000: Rd);
		lpm_z_low  <= RZ[0];
	end

	`ifdef AVR_INITIAL
	if ( ~init_count[init_depth-1] )
		init_count <= init_count + 1;
	`endif


	ieack <= (is_int_enter ? ivect : 0);

	`ifdef AVR_HAVE_WDR
	WDR <= i_wdr;
	`endif

	/* synchronous reset: */
	if ( rst ) begin
		PC <= 0;
		{ I, T, H, S, V, N, Z, C } <= 0;
		state <= STATE_STALL;
	end else begin
		PC <= pc_next;
		{ I, T, H, S, V, N, Z, C } <= nSREG;
		state <= next_state;
	end

	// if ( sp_update )
		SP <= sp_next;

end

/*****************************************************************************/

/* Debug section starts here */

wire [pmem_width:0] PC_double = {PC,1'b0};
wire [7:0] R0 = GPR[0];
wire [7:0] R1 = GPR[1];
wire [7:0] R2 = GPR[2];
wire [7:0] R3 = GPR[3];
wire [7:0] R4 = GPR[4];
wire [7:0] R5 = GPR[5];
wire [7:0] R6 = GPR[6];
wire [7:0] R7 = GPR[7];
wire [7:0] R8 = GPR[8];
wire [7:0] R9 = GPR[9];
wire [7:0] R10 = GPR[10];
wire [7:0] R11 = GPR[11];
wire [7:0] R12 = GPR[12];
wire [7:0] R13 = GPR[13];
wire [7:0] R14 = GPR[14];
wire [7:0] R15 = GPR[15];
wire [7:0] R16 = GPR[16];
wire [7:0] R17 = GPR[17];
wire [7:0] R18 = GPR[18];
wire [7:0] R19 = GPR[19];
wire [7:0] R20 = GPR[20];
wire [7:0] R21 = GPR[21];
wire [7:0] R22 = GPR[22];
wire [7:0] R23 = GPR[23];
wire [7:0] R24 = GPR[24];
wire [7:0] R25 = GPR[25];
wire [7:0] R26 = GPR[26];
wire [7:0] R27 = GPR[27];
wire [7:0] R28 = GPR[28];
wire [7:0] R29 = GPR[29];
wire [7:0] R30 = GPR[30];
wire [7:0] R31 = GPR[31];

`ifdef SIMULATOR
initial begin
	$dumpvars(1,PC,PC_double,INSTR,SP,SREG,state,pc_call,R);
	$dumpvars(1,R0,R1,R16,R17,R18,R19,R20,R21,R22,R23,R24,R25,RX,RY,RZ);
	$dumpvars(1,io_we,io_re,io_a,io_do,io_di,Rio);
	$dumpvars(1,dmem_we,dmem_re,dmem_a,dmem_do,dmem_di);
end
`endif

/* end of debug section */
/*****************************************************************************/

endmodule

/*****************************************************************************/
