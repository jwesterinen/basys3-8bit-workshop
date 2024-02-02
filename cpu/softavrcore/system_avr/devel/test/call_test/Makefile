SHELL=/bin/sh

PROJNAME=calltest
SYNTH_DIR = ../../..
TB_DIR = ../../../system_avr_tb
OBJS=main.o functions.o

TARGET=$(PROJNAME).mem

CC=avr-gcc
AS=avr-as
AR=avr-ar
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
CHMOD=chmod
STAT=stat
DD=dd

PLATFORM = system_avr_b3
PLATFORM_LIB = $(PLATFORM).o

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

crt0.o: ../crt0.S
	$(AS) $(AFLAGS) -o $@ $<

$(PROJNAME).elf: $(OBJS) $(ARCH_LIB)
	$(CC) $(LDFLAGS) -o $@ $^
	$(OBJDUMP) -s -m $(ARCH) -d $@ > $(PROJNAME).disasm

$(PROJNAME).bin: $(PROJNAME).elf
	$(OBJCOPY) -j .text -j .data -O binary $< $@

$(TARGET): $(PROJNAME).bin
	cat $< /dev/zero | head -c $(PMEM_SIZE) | hexdump -v -e '/2 "%.4x\n"' > $(TARGET)

install: $(TARGET)
	cp $< $(SYNTH_DIR)/rom.bin
	cp $< $(TB_DIR)/rom.bin

clean:
	rm -f $(TARGET) *.o *.elf *.bin *.disasm

