app ?= 
TARGET = $(app).bin
OBJ = $(app).asm

SYSTEM16_DIR = ../..

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
	
