#
#  Name: Makefile
# 
#  Description: This is the Makefile for the asm16 assembler.
#
#  Copyright:   Copyright (C) 2023 Jeff Westerinen
#               All rights reserved.
# 

TARGET = asm16

#PREFIX ?= /usr/local
#INST_BIN_DIR = $(PREFIX)/bin

#DEFINES = -DYYDEBUG -DTRACE
#DEFINES = -DTRACE

CFLAGS = -O0 -g -Wall -c
LIBS = -ll

HEADERS = error.h symtab.h message.h codegen.h asm16.h
OBJECTS = main.o error.o symtab.o message.o codegen.o lex.yy.o y.tab.o

all: $(TARGET)
	make -C stdlib

$(TARGET): lex.yy.c y.tab.c y.tab.h $(OBJECTS)
	cc $(OBJECTS) $(LIBS) -o $@

%.o: %.c $(HEADERS)
	cc $(DEFINES) $(CFLAGS) -o $@ $<

lex.yy.c: $(TARGET).l y.tab.h
	lex $<

y.tab.c y.tab.h: $(TARGET).y
	yacc -d $<

clean:
	rm -f $(TARGET) *.o lex.yy.c y.tab.c y.tab.h

install:
	/usr/bin/install -m 755 $(TARGET) $(INST_BIN_DIR)
	make INST_INCL_DIR=$(INST_INCL_DIR) -C stdlib install

uninstall:
	rm -f $(INST_BIN_DIR)/$(TARGET)
	make INST_INCL_DIR=$(INST_INCL_DIR) -C stdlib uninstall

.PHONY: clean install uninstall
	
