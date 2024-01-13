app ?= 

TARGET = $(app).bin
SYNTH_DIR = ..

CC=avr-gcc
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump

LANGUAGE = assembler-with-cpp
ARCH = avr25

CFLAGS=-x $(LANGUAGE) -mmcu=$(ARCH)

all: $(TARGET)

$(TARGET): $(app).S
	$(CC) $(CFLAGS) $< -o $(app).elf
	$(OBJCOPY) -j .text -j .data -O binary $(app).elf temp.bin
	od temp.bin -A n -t x2 > $(app).bin
	rm temp.bin
	$(OBJDUMP) -s -m $(ARCH) -d $(app).elf > $(app).disasm

clean:
	rm -f $(app).bin $(app).elf $(app).disasm

install: $(TARGET)
	cp $< $(SYNTH_DIR)/rom.bin

.PHONY: clean install
	
