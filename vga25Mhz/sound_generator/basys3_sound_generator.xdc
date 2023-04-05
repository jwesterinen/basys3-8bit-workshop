## This file is .xdc file for the sound controller project on the Basys3 rev B board

## Clock signal
set_property -dict { PACKAGE_PIN W5   IOSTANDARD LVCMOS33 } [get_ports clk]
create_clock -add -name sys_clk_pin -period 10.00 -waveform {0 5} [get_ports clk]


##Buttons
set_property -dict { PACKAGE_PIN U18   IOSTANDARD LVCMOS33 } [get_ports btnC]

## Switches
set_property -dict { PACKAGE_PIN V17   IOSTANDARD LVCMOS33 } [get_ports sw0]

##Pmod Header JA
set_property -dict { PACKAGE_PIN J1   IOSTANDARD LVCMOS33 } [get_ports JA1]
set_property -dict { PACKAGE_PIN L2   IOSTANDARD LVCMOS33 } [get_ports JA2]
set_property -dict { PACKAGE_PIN G2   IOSTANDARD LVCMOS33 } [get_ports JA4]

## Configuration options, can be used for all designs
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]
