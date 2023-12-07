#
#  Name: Makefile
# 
#  Description: This is the Makefile for the asm16 standard C library.
#
#  Copyright:   Copyright (C) 2023 Jeff Westerinen
#               All rights reserved.
# 


SYS_ASM_FILES = fputb.c fputw.c
HEADER = libasm.h
DEFS = system16.h

install:
	/usr/bin/install -m 644 -D -t $(INST_INCL_DIR) $(SYS_ASM_FILES)
	/usr/bin/install -m 644 -D -t $(INST_INCL_DIR) $(HEADER)
	/usr/bin/install -m 644 -D -t $(INST_INCL_DIR) $(DEFS)

uninstall:
	rm -fr $(INST_INCL_DIR)

.PHONY: install uninstall
	
