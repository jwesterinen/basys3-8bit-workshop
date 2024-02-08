module flash
 #(	parameter	flash_file = "main.mem",
	parameter	flash_width = 10
 )
 (	input				clk,
	input				mem_ce,
	input	[flash_width-1:0]	mem_a,
	output	[15:0]			mem_d
 );

reg [15:0] flash_array [0:2**flash_width-1];

reg [15:0] data_read;

assign mem_d = data_read;

always @(posedge clk) begin
	if (mem_ce) data_read <= flash_array[mem_a];
end

initial begin
	$readmemh(flash_file, flash_array);
end

endmodule
