# Makefile to run verilog simulations
#
# Targets:
#    "make compile"             compiles only
#    "make run"                 runs only
#    "make view"                starts waveform viewer
#    "make clean"               deletes temporary files and dirs


#----- Useful variables
NAME_TOP	:= avr_b3

#----- Targets, iverilog
# Use this to compile without running simulation
compile:
	iverilog -tvvp -c $(NAME_TOP).vf -o $(NAME_TOP).vvp -v > $(NAME_TOP).log

# Run simulation
run: compile
	vvp $(NAME_TOP).vvp
	mv dump.vcd $(NAME_TOP).vcd

# Start viewer
view: run
	gtkwave $(NAME_TOP).vcd $(NAME_TOP).gtkw &

# iverilog help, command line
help:
	man iverilog

#----- Cleanup
# Delete temporary files
clean:
	rm -f $(NAME_TOP).log
	rm -f $(NAME_TOP).vvp
	rm -f $(NAME_TOP).vcd
