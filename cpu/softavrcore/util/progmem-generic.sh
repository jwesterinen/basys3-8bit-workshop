#!/bin/bash

infile=""

depth=8

name=rom_16bit

while [ -n "$1" ]; do
	case "$1" in
		-h|--help)
			echo -e "Usage:\t$0 [-h|--help] [-w|--width <width>] <16-bit-image.bin>"
			exit	0
			;;
		-w|--width|-d|--depth)
			depth="$2"
			shift
			;;
		-n|--name)
			name="$2"
			shift
			;;
		-*)
			echo -e "$0: error: invalid command line argument near '$1'." >> /dev/stderr
			exit	1
			;;
		*)
			infile="$1"
			;;
	esac; shift
done

if ! [ -n "$infile" ] || ! [ -f "$infile" ]; then
	echo "$0: error: input file is missing or not found." >> /dev/stderr
	exit	1
fi

nword=$((1<<depth))

cat "$infile" /dev/zero | \
head -c $((2*nword)) | \
hexdump -v -e '/2 "%.4x\n"' | \
awk -v f=0 -v i=0 -v n=0 -v nword=$nword -v depth=$depth -v name=$name \
 'BEGIN \
  {	printf("module %s\n",name);
	printf("#(	parameter	flash_width = %d\n",depth);
	printf(" )\n");
	printf(" (	input		clk,\n");
	printf("	input		mem_ce,\n");
	printf("	input	[flash_width-1:0] mem_a,\n");
	printf("	output	[15:0]	mem_d\n");
	printf(" );\n");
	printf("reg [15:0] flash_array [0:2**flash_width-1];\n");
	printf("reg [15:0] data_read;\n");
	printf("assign mem_d = data_read;\n");
	printf("always @(posedge clk) begin\n");
	printf("	if (mem_ce) data_read <= flash_array[mem_a];\n");
	printf("end\n");
	printf("initial begin\n");
  } \
  {	printf("	flash_array[%d]=16%ch%s;\n",n,39,$1);n++;
  } \
 END \
  {	printf("end\n");
	printf("endmodule\n");
  }'	

