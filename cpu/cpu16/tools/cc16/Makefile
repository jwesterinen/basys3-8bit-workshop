#
#  Name: Makefile
# 
#  Description: This is the Makefile for cc16, the asm16 C compiler.
#
#  Copyright:   Copyright (C) 2023 Jeff Westerinen
#               All rights reserved.
# 

TARGET = cc16

#DEFINES = -DYYDEBUG -DTRACE
#DEFINES = -DTRACE

CFLAGS = -O0 -g -Wall -c
LIBS = -ll

HEADERS = error.h symtab.h message.h gen.h gen_cpu16.h
OBJECTS = main.o error.o symtab.o message.o gen.o gen_cpu16.o lex.yy.o y.tab.o

all: $(TARGET)

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

uninstall:
	rm -f $(INST_BIN_DIR)/$(TARGET)

.PHONY: clean install uninstall
	
