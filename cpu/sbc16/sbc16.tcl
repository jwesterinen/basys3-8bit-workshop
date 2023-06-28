set project sbc16 ;

set xdc_file ${project}_basys3.xdc

read_verilog ${project}.v ;
read_xdc $xdc_file ;

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

