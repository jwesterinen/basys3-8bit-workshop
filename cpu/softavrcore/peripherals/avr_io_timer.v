/*****************************************************************************/
/* avr_io_timer.v							     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* (c) 2019-2020; Andras Pal <apal@szofi.net>				     */
/*****************************************************************************/

module avr_io_timer
 (	input clk,
	input rst,

	input io_re,
	input io_we,
	input [1:0] io_a,
	output [7:0] io_do,
	input [7:0] io_di,
	output	irq
 );

reg [15:0] TCNT;
reg [7:0]  TTMP;
reg [7:0]  TCR;

reg [11:0]	prescaler;
reg [3:0]	pre_prev;
reg		overflow; 

wire [7:0] TSR = { overflow, 7'b0000000 };

assign irq = TSR[7] & TCR[7];

/* I/O read: */
reg [7:0] io_do_data;
always @(*) begin
	casex (io_a)
		2'b00: io_do_data = TCNT[7:0];
		2'b01: io_do_data = TTMP[7:0];
		2'b10: io_do_data = TCR;
		2'b11: io_do_data = TSR;
        endcase
end
assign io_do = io_re ? io_do_data : 8'b00000000;


always @(posedge clk) begin

	if (io_we & ~io_re) begin
		if ( io_a==2'b01 ) TTMP <= io_di;
		if ( io_a==2'b10 ) TCR  <= io_di;
	end else if ( io_re ) begin
		if ( io_a==2'b00 )
			TTMP <= TCNT[15:8];
	end

end

wire tcnt_write = io_we & (io_a==2'b00);
wire tcr_write  = io_we & (io_a==2'b10);

/* Note: the interrupt is cleared when the overflow flag is reset: therefore, any write
into the TNCT _or_ the TCR register would clear the interrupt: */

wire o = overflow & (~tcr_write);

reg increment;

always @(*) begin
	casex (TCR[1:0])
		2'b00: increment = 1;
		2'b01: increment = (~prescaler[ 3])&pre_prev[0];
		2'b10: increment = (~prescaler[ 7])&pre_prev[1];
		2'b11: increment = (~prescaler[11])&pre_prev[2];
	endcase
end

always @(posedge clk) begin
	if ( ! tcnt_write ) begin
		prescaler <= prescaler + 1;
		pre_prev  <= { prescaler[11], prescaler[7], prescaler[3] };
		{ overflow, TCNT } <= { o, 16'd0 } | ( { o, TCNT } + increment );
	end else begin
		TCNT <= { TTMP, io_di };
		prescaler <= 0;
		overflow <= 0;
	end
end

/*****************************************************************************/
/* Debug section starts here */

/* end of debug section */
/*****************************************************************************/

endmodule

/*****************************************************************************/
