test ?= 
TARGET = $(test).bin
OBJ = $(test).asm

SYSTEM16_DIR = ../../../system16

%.asm: %.c
	cc16 $<

%.bin: %.asm
	asm16 $<

all: $(TARGET)

$(TARGET): $(OBJ)

clean:
	rm -f $(TARGET) *.asm *.obj *.bin

install: $(TARGET)
	cp $< $(SYSTEM16_DIR)/rom.bin

.PHONY: clean install uninstall
	
