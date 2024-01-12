/*****************************************************************************/
/* avr_io_uart.v							     */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/* (c) 2019-2020; Andras Pal <apal@szofi.net>				     */
/*****************************************************************************/

module uart_tx (input clk, input [7:0] prescaler, input [7:0] tx_in, input strobe, output reg txd, output busy, output prefetch);

parameter TX_STATE_IDLE = 0;
parameter TX_STATE_TRANSMIT = 1;

reg state = TX_STATE_IDLE;

//reg txd = 1;

reg [7:0] count = 0;
reg [7:0] scaler_counter = 0;

reg [7:0] dataout = 0;

parameter count_step = 2;
assign busy = state; 

wire scaler_limit = (scaler_counter==0);
wire bit_limit = (count[3:0] == 4'b0000);
wire last_bit = (count[7:4]==4'd10);

assign prefetch = ( state==TX_STATE_TRANSMIT ) & scaler_limit & bit_limit & last_bit;

always @(posedge clk) begin
	
	if ( state==TX_STATE_IDLE && strobe ) begin
		state <= TX_STATE_TRANSMIT;
		txd <= 0;
		count <= count_step;
		dataout <= tx_in;
		scaler_counter <= prescaler;
	end else if ( state==TX_STATE_TRANSMIT && scaler_limit ) begin
		if ( bit_limit ) begin
			if ( last_bit ) begin
				if ( strobe ) begin
					txd <= 0;
					count <= count_step;
					dataout <= tx_in;
				end else begin
					txd <= 1;
					state <= TX_STATE_IDLE;
				end
			end else begin
				txd <= dataout[0];
				dataout <= { 1'b1, dataout[7:1] };
				count <= count + count_step;
			end
		end else begin
			count <= count + count_step;
		end 
		scaler_counter <= prescaler;
	end else if ( state==TX_STATE_TRANSMIT ) begin
		scaler_counter <= scaler_counter - 1;
	end

end

endmodule

/*****************************************************************************/

module uart_rx (input clk, input [7:0] prescaler, input rxd, input reset, output [7:0] rx_out, output reg avail);

parameter STATE_IDLE = 0;
parameter STATE_STARTBIT = 2;
parameter STATE_RECEIVE = 3;

reg [1:0] state = STATE_IDLE;

reg [7:0] count = 0;
reg [7:0] scaler_counter = 0;

reg [7:0] datain = 0;
//reg avail = 0;

parameter count_step = 2;

wire rx_sub_bit = ( state==STATE_RECEIVE && scaler_counter==0 );
wire rx_bit = (rx_sub_bit && count[3:0] == 4'b0000);
wire rx_completed = (rx_bit && count[7:4]==4'd9 );

assign rx_out = datain;

always @(posedge clk) begin
	
	if ( state==STATE_IDLE && rxd==0 ) begin
		state <= STATE_STARTBIT;
		count <= count_step;
		scaler_counter <= prescaler;
	end else if ( state==STATE_STARTBIT && scaler_counter==0 ) begin
		if ( count[3:0] == 4'b1000 ) begin
			state <= STATE_RECEIVE;
			count <= count_step;
		end else begin
			count <= count + count_step;
		end
		scaler_counter <= prescaler;
	end else if ( state==STATE_RECEIVE && scaler_counter==0 ) begin
		if ( count[3:0] == 4'b0000 ) begin
			if ( count[7:4]==4'd9 ) begin
				state <= STATE_IDLE;
			end else begin
				datain <= { rxd, datain[7:1] };
				count <= count + count_step;
			end
		end else begin
			count <= count + count_step;
		end
		scaler_counter <= prescaler; 
	end else if ( state[1] ) begin
		scaler_counter <= scaler_counter - 1;	
	end

	avail <= rx_completed | (avail & ~reset);

end

endmodule

/*****************************************************************************/

module avr_io_uart
 (	input clk,
	input rst,

	input io_re,
	input io_we,
	input [1:0] io_a,
	output [7:0] io_do,
	input [7:0] io_di,

	output txd,
	input rxd,

	output	[2:0] irq
 );

reg [7:0] UDR_TX = 0;
reg [7:0] UDR_RX  = 0;
reg [7:0] UCSRB  = 0;
reg [7:0] UBRR  = 0;

parameter UCSRA_RXB8	= 3'd0;
parameter UCSRA_x1	= 3'd1;
parameter UCSRA_PE	= 3'd2;
parameter UCSRA_DOR	= 3'd3; 
parameter UCSRA_FE	= 3'd4;
parameter UCSRA_UDRE	= 3'd5;
parameter UCSRA_TXC	= 3'd6; 
parameter UCSRA_RXC	= 3'd7;

wire	RXCIE, TXCIE, UDRIE, USBS, UPM1, UPM0, UCSZ, TXB8;
assign  { RXCIE, TXCIE, UDRIE, USBS, UPM1, UPM0, UCSZ, TXB8 } = UCSRB;

reg 	rx0_non_empty = 0, rx0_overrun = 0, rx0_reset = 0;
wire	tx0_txd,tx0_busy,tx0_prefetch;
reg	tx0_non_empty = 0;

wire [7:0] UCSRA = { rx0_non_empty, ~tx0_busy, ~tx0_non_empty, 1'b0, 1'b0, 1'b0, 1'b0, 1'b0 };

assign	irq = UCSRB[7:5] & UCSRA[7:5];

/* I/O read: */
reg [7:0] io_do_data;
always @(*) begin
	casex (io_a)
		2'b00: io_do_data = UDR_RX[7:0];
		2'b01: io_do_data = UCSRA;
		2'b10: io_do_data = UCSRB;
		2'b11: io_do_data = UBRR;
	endcase
end
assign io_do = io_re ? io_do_data : 8'b00000000;

/* I/O write: configuration: */
always @(posedge clk) begin
	if ( io_we ) begin
		casex (io_a)
			2'b10: UCSRB <= io_di;
			2'b11: UBRR <= io_di;
		endcase
	end
end

/* TX */


uart_tx tx0 (clk, UBRR, UDR_TX, tx0_non_empty, tx0_txd, tx0_busy, tx0_prefetch);

/* transmitter state changes: */
always @(posedge clk) begin
	if ( io_we && io_a == 2'b00 && ~tx0_non_empty ) begin
		tx0_non_empty <= 1;
		UDR_TX <= io_di; 
	end else if ( (tx0_non_empty & ~tx0_busy) | tx0_prefetch )
		tx0_non_empty <= 0;
end

assign	txd = tx0_txd | (~tx0_busy);

/* RX */

wire [7:0] rx0_data;
wire	rx0_avail; 

reg rxd_curr,rxd_prev,rxd_filt;

uart_rx rx0 (clk, UBRR, rxd_filt, rx0_reset, rx0_data, rx0_avail);

always @(posedge clk) begin
	rxd_curr <= rxd;
	rxd_prev <= rxd_curr;
	if ( rxd_curr & rxd_prev )
		rxd_filt <= 1;
	else if ( ~rxd_curr & ~rxd_prev )
		rxd_filt <= 0;
end

/* receiver state changes: */
always @(posedge clk) begin

	if ( io_re && io_a == 2'b00 ) begin
		rx0_non_empty <= 0;	
		rx0_overrun   <= 0;
	end else if ( rx0_avail && ~rx0_reset ) begin
		UDR_RX <= rx0_data;
		rx0_non_empty <= 1;
		rx0_overrun   <= rx0_non_empty;
		rx0_reset <= 1;
	end else begin
		rx0_reset <= 0;
	end
end

/*****************************************************************************/
/* Debug section starts here */

`ifdef SIMULATOR
initial begin
	$dumpvars(1,UDR_TX,UDR_RX,UCSRB,UBRR,tx0_non_empty,tx0_busy,tx0_prefetch);
	$dumpvars(1,rxd,rx0_non_empty,rx0_avail,rx0_data,rx0_reset,rx0_overrun);
end
`endif

/* end of debug section */
/*****************************************************************************/

endmodule

/*****************************************************************************/
