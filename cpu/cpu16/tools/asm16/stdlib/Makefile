#
#  Name: Makefile
# 
#  Description: This is the Makefile for the asm16 standard library.
#
#  Copyright:   Copyright (C) 2023 Jeff Westerinen
#               All rights reserved.
# 


SYS_ASM_FILES = multiply.asm divide.asm beep.asm delayms.asm display.asm read_button.asm read_keypad.asm append_key_value.asm read_switches.asm show_leds.asm sys.asm
HEADER = libasm.asm
DEFS = system16.asm

install:
	/usr/bin/install -m 644 -D -t $(INST_INCL_DIR) $(SYS_ASM_FILES)
	/usr/bin/install -m 644 -D -t $(INST_INCL_DIR) $(HEADER)
	/usr/bin/install -m 644 -D -t $(INST_INCL_DIR) $(DEFS)

uninstall:
	rm -fr $(INST_INCL_DIR)

.PHONY: install uninstall
	
