#
#  Name: Makefile
# 
#  Description: This is the Makefile for a basic interpreter.
#
#  Copyright:   Copyright (C) 2024 Jeff Westerinen
#               All rights reserved.
# 

TARGET = basic_simulator

#DEFINES = -DYYDEBUG -DTRACE

CFLAGS = -O0 -g -Wall -c

HEADERS = main.h symtab.h lexer.h parser.h runtime.h
OBJECTS = main.o symtab.o lexer.o parser.o runtime.o

all: $(TARGET)

$(TARGET): $(OBJECTS)
	cc  $(OBJECTS) -o $@

%.o: %.c $(HEADERS)
	cc $(DEFINES) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGET) *.o

.PHONY: clean
	
