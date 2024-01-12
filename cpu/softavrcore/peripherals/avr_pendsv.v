/*****************************************************************************/
/* avr_pendsv.v								     */
/*****************************************************************************/
/* Registers								     */
/* 	PENDSV	r+w	BASE + 0x00	{ 6'b0, PENDSVIE, PENDSV }	     */
/*****************************************************************************/

module avr_pendsv
 (	input clk,
	input rst,

	input	io_re,
	input	io_we,
	output	[7:0] io_do,
	input	[7:0] io_di,
	output	irq
 );

reg	PENDSV, PENDSVIE;

assign	irq = PENDSVIE & PENDSV;
assign	io_do = { 6'b0, io_re ? { PENDSVIE, PENDSV } : { 2'b0 } };

always @(posedge clk) begin

	if (io_we) begin
		PENDSVIE <= io_di[1];
		PENDSV <= io_di[0];
	end

end

/*****************************************************************************/
/* Debug section starts here */

/* end of debug section */
/*****************************************************************************/

endmodule

/*****************************************************************************/
