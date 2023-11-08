#
#  Name: Makefile
# 
#  Description: This is the top level Makefile for the asm16 tools.
#
#  Copyright:   Copyright (C) 2023 Jeff Westerinen
#               All rights reserved.
# 

PREFIX ?= /usr/local
INST_BIN_DIR = $(PREFIX)/bin
INST_LIB_DIR = $(PREFIX)/lib/asm16
INST_INCL_DIR = $(PREFIX)/include/asm16

all:
	#make -C cc16 all
	make -C asm16 all

clean:
	#make -C cc16 clean
	make -C asm16 clean

install:
	#make INST_BIN_DIR=$(INST_BIN_DIR) -C cc16 install
	make INST_BIN_DIR=$(INST_BIN_DIR) INST_INCL_DIR=$(INST_INCL_DIR) -C asm16 install

uninstall:
	#make INST_BIN_DIR=$(INST_BIN_DIR) -C cc16 uninstall
	make INST_BIN_DIR=$(INST_BIN_DIR) INST_INCL_DIR=$(INST_INCL_DIR) -C asm16 uninstall

.PHONY: clean install uninstall
	
