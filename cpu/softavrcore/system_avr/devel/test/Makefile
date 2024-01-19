SHELL=/bin/sh

test ?= 

TARGET=$(test).mem

SYNTH_DIR = ../..
TB_DIR = ../../system_avr_tb

CC=avr-gcc
AS=avr-as
AR=avr-ar
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
CHMOD=chmod
STAT=stat
DD=dd

ARCH = avr25
ARCH_LIB = crt0.o

AFLAGS=-mmcu=$(ARCH)
CFLAGS=-mmcu=$(ARCH) -D__COMPILING_AVR_LIBC__ -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wno-unused-function -O3
LDFLAGS=-mmcu=$(ARCH)

PMEM_DEPTH=12
PMEM_WORDS=$(shell echo $$((1<<$(PMEM_DEPTH))))
PMEM_SIZE=$(shell echo $$((2<<$(PMEM_DEPTH))))

.PHONY: all install clean

all: $(TARGET)

crt0.o: crt0.S
	$(AS) $(AFLAGS) -o crt0.o crt0.S

$(test).o: $(test).c
	$(CC) $(CFLAGS) -c $(test).c

$(test).elf: $(test).o $(ARCH_LIB)
	$(CC) $(LDFLAGS) -o $(test).elf $(test).o $(ARCH_LIB)
	$(OBJDUMP) -s -m $(ARCH) -d $(test).elf > $(test).disasm

$(test).bin: $(test).elf
	$(OBJCOPY) -j .text -j .data -O binary $(test).elf $(test).bin

$(test).mem: $(test).bin
	cat $(test).bin /dev/zero | head -c $(PMEM_SIZE) | hexdump -v -e '/2 "%.4x\n"' > $(test).mem

install: $(TARGET)
	cp $< $(SYNTH_DIR)/rom.bin
	cp $< $(TB_DIR)/rom.bin

clean:
	rm -f $(TARGET) *.o *.elf *.bin *.disasm

clean_all:
	rm -f *.mem *.o *.elf *.bin *.disasm

