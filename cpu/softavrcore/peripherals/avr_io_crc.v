/*****************************************************************************/
/* avr_io_crc.v								     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* (c) 2023; Andras Pal <apal@szofi.net>				     */
/*****************************************************************************/

module avr_io_crc
 (	input clk,
	input rst,

	input io_re,
	input io_we,
	input [1:0] io_a,
	output [7:0] io_do,
	input [7:0] io_di
 );

function [31:0] crc_polynomials_1;
input value;
        crc_polynomials_1 = 0;
        case ( value )
                1'b0:	crc_polynomials_1 = 32'h00000000;
                1'b1:	crc_polynomials_1 = 32'h82F63B78;
        endcase
endfunction

function [31:0] crc_polynomials_2;
input [1:0] value;
        crc_polynomials_2 = 0;
        case ( value )
                2'b00:	crc_polynomials_2 = 32'h00000000;
                2'b01:	crc_polynomials_2 = 32'h417B1DBC;
                2'b10:	crc_polynomials_2 = 32'h82F63B78;
                2'b11:	crc_polynomials_2 = 32'hC38D26C4;
        endcase
endfunction

reg [31:0] CRC = 0;
reg [7:0] DR = 0;

reg [1:0] cnt = 0;
reg state = 0 ;

/* I/O read: */
reg [7:0] io_do_data;
always @(*) begin
	casex (io_a)
		2'b00: io_do_data = CRC[7:0];
		2'b01: io_do_data = CRC[15:8];
		2'b10: io_do_data = CRC[23:16];
		2'b11: io_do_data = CRC[31:24];
	endcase
end
assign io_do = io_re ? io_do_data : 8'b00000000;

always @(posedge clk) begin

	if ( io_we && io_a == 0 ) begin

		DR <= io_di;
		state <= 1;
		cnt <= 0;

	end else if ( io_we && io_a == 1 ) begin

		if ( io_di[0] )
			CRC <= ~CRC;
		else if ( io_di[1] )
			CRC <= 32'hFFFFFFFF;

	end else if ( ! io_we && state ) begin

		/*
		CRC <= crc_polynomials_1(CRC[0:0]^DR[0:0])^(CRC>>1);
		DR  <= DR >> 1;
		*/

		CRC <= crc_polynomials_2(CRC[1:0]^DR[1:0])^(CRC>>2);
		DR  <= DR >> 2;

		if ( cnt == 3 )
			state <= 0;
		else
			cnt <= cnt+1;
	end
end


/*****************************************************************************/
/* Debug section starts here */

`ifdef SIMULATOR
initial begin
	$dumpvars(1,CRC);
end
`endif

/* end of debug section */
/*****************************************************************************/

endmodule

/*****************************************************************************/
