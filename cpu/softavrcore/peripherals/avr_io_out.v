/*****************************************************************************/
/* avr_io_out.v								     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* (c) 2019-2020; Andras Pal <apal@szofi.net>				     */
/*****************************************************************************/

module avr_io_out
 (	input clk,
	input rst,

	input io_re,
	input io_we,
	output [7:0] io_do,
	input [7:0] io_di,

	output [7:0] port
 );

reg [7:0] PORT = 0;

assign port = PORT; 

assign io_do = io_re ? PORT : 8'b00000000;

always @(posedge clk) begin

	if (io_we) begin
		PORT <= io_di;
	end

end


/*****************************************************************************/
/* Debug section starts here */

/* end of debug section */
/*****************************************************************************/

endmodule

/*****************************************************************************/
