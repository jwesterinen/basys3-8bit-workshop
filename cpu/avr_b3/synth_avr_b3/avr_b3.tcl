set project avr_b3 ;

set xdc_file ${project}.xdc ;

set_param synth.elaboration.rodinMoreOptions "rt::set_parameter compatibilityMode true"
read_verilog sysdefs.h
read_verilog avr_core.v
read_verilog avr_io_uart.v
read_verilog clocks.v
read_verilog flash.v
read_verilog font437.v
read_verilog prescaler.v
read_verilog ps2.v
read_verilog ram.v
read_verilog vgaterm.v
read_verilog display_b3.v
read_verilog basic_io_b3.v
read_verilog keypad_b3.v
read_verilog PmodKYPD.v
read_verilog SN76477.v
read_verilog lfsr.v
read_verilog sound_b3.v
read_verilog mmio.v
read_verilog top.v 
read_xdc $xdc_file

synth_design -top $project -part xc7a35tcpg236-1 ;
write_checkpoint -force build/${project}_post_synth.dcp ;

opt_design ;
place_design ;
write_checkpoint -force build/${project}_post_place.dcp ;
report_utilization -file build/${project}_post_place_util.txt ;

route_design ;
report_timing_summary -file build/${project}_post_route_timing.txt ;
write_checkpoint -force build/${project}_post_route.dcp ;
report_drc -file build/${project}_post_route_drc.txt ;

write_bitstream -force build/${project}.bit ;

quit

