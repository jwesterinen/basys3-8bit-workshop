#
#  Name: Makefile
# 
#  Description: This is the Makefile for the CPU16 assembler
#
#  Copyright:   Copyright (C) 2023 Jeff Westerinen
#               All rights reserved.
# 

TARGET = cpu16_asm

DEPS = code.h parser.h symbolTable.h
OBJ =  $(TARGET).o code.o parser.o symbolTable.o

CPP=g++
CPPFLAGS = -I. -O0 -g -Wall -c


%.o: %.cpp $(DEPS)
	$(CPP) -c -o $@ $< $(CPPFLAGS)

all: $(TARGET)
	make -C stdlib

$(TARGET): $(OBJ)
	$(CPP) -o $@ $^

clean:
	rm -f $(TARGET) *.o
	make -C stdlib clean

install:
	/usr/bin/install -m 755 $(TARGET) $(INST_BIN_DIR)
	make INST_LIB_DIR=$(INST_LIB_DIR) INST_INCL_DIR=$(INST_INCL_DIR) -C stdlib install

uninstall:
	rm -f $(INST_BIN_DIR)/$(TARGET)
	make INST_LIB_DIR=$(INST_LIB_DIR) INST_INCL_DIR=$(INST_INCL_DIR) -C stdlib uninstall

.PHONY: clean install uninstall
	
