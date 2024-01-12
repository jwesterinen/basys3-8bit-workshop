/*****************************************************************************/
/* avr_systick.v							     */
/*****************************************************************************/
/* Registers								     */
/* 	STCNTL	r	BASE + 0x00		{ CNT[7:0] }		     */
/*	STCNTH	r	BASE + 0x01		{ OVERFLOW, CNT[14:8] }	     */
/*	STLOADL	r+w	BASE + 0x02		{ CLOAD[7:0] }		     */
/*	STLOADH	r+w	BASE + 0x03		{ IENABLE, CLOAD[14:8] }     */
/*****************************************************************************/

module avr_systick
 (	input clk,
	input rst,

	input	io_re,
	input	io_we,
	input	[1:0] io_a,
	output	[7:0] io_do,
	input	[7:0] io_di,
	output	irq,
	input	ack
 );

reg IENABLE;
reg [14:0] CLOAD = 15'd23999;
reg OVERFLOW;
reg [14:0] CNT;
reg [6:0] CTMP;

assign irq = IENABLE & OVERFLOW;

/* I/O read: */
reg [7:0] io_do_data;
always @(*) begin
	casex (io_a)
		2'b00: io_do_data = CNT[7:0];
		2'b01: io_do_data = { OVERFLOW, CTMP };
		2'b10: io_do_data = CLOAD[7:0];
		2'b11: io_do_data = { IENABLE, CLOAD[14:8] } ;
        endcase
end

assign io_do = io_re ? io_do_data : 8'b00000000;

wire reset_overflow_bit = (io_we & (io_a[1]==0)) | ack;

always @(posedge clk) begin

	if (io_we & ~io_re) begin
		if ( io_a==2'b10 ) begin
			CLOAD[7:0]  <= io_di;
		end else if ( io_a==2'b11 ) begin
			{ IENABLE, CLOAD[14:8] } <= io_di;
		end
	end else if ( io_re ) begin
		if ( io_a==2'b00 )
			CTMP <= { 1'b0, CNT[14:8] };
	end

end

always @(posedge clk) begin

	if ( CNT[14:0]==0 ) begin

		CNT <= CLOAD;
		OVERFLOW <= (reset_overflow_bit ? 0 : 1);

	end else begin

		if ( reset_overflow_bit )
			OVERFLOW <= 0;

		/* reset counter if IENABLE is set from 0 to 1: */
		if ( io_we && io_a==3 && ~IENABLE && io_di[7] ) begin
			CNT <= CLOAD;
			OVERFLOW <= 0;
		end else begin
			CNT <= CNT - 1;	
		end

	end
end

/*****************************************************************************/
/* Debug section starts here */

/* end of debug section */
/*****************************************************************************/

endmodule

/*****************************************************************************/
