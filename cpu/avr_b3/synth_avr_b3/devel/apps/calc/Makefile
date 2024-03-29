SHELL=/bin/sh

APP=calc

SYNTH_DIR = ../../..
TB_DIR = ../../../avr_b3_tb
LIB_DIR = ../../lib
INC_DIR = ../../include

OBJECTS=$(APP).o $(LIB_DIR)/avr_b3_lib.o
#OBJECTS=$(APP).o

CC=avr-gcc
AS=avr-as
AR=avr-ar
OBJCOPY=avr-objcopy
OBJDUMP=avr-objdump
CHMOD=chmod
STAT=stat
DD=dd

#ARCH = avr25
ARCH = avr51
ARCH_LIB = $(LIB_DIR)/crt0.o

AFLAGS=-mmcu=$(ARCH)
#CFLAGS=-mmcu=$(ARCH) -D__COMPILING_AVR_LIBC__ -Wall -pedantic -O3
CFLAGS=-mmcu=$(ARCH) -D__COMPILING_AVR_LIBC__ -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wno-unused-function -O3
LDFLAGS=-mmcu=$(ARCH) -T ../../misc/avr_b3_51.xn
#LDFLAGS=-mmcu=$(ARCH)
#LDFLAGS=-mmcu=$(ARCH) -Wl,--verbose

PMEM_DEPTH=12
PMEM_WORDS=$(shell echo $$((1<<$(PMEM_DEPTH))))
PMEM_SIZE=$(shell echo $$((2<<$(PMEM_DEPTH))))

.PHONY: all clean dummy

TARGET=rom.mem

all: $(TARGET)

$(ARCH_LIB): $(LIB_DIR)/crt0.S
	$(AS) $(AFLAGS) -o $(ARCH_LIB) $^

$(APP).elf: $(OBJECTS) $(ARCH_LIB)
	$(CC) $(LDFLAGS) -o $@ $^
	$(OBJDUMP) -s -m $(ARCH) -d $(APP).elf > $(APP).disasm

$(APP).bin: $(APP).elf
	$(OBJCOPY) -j .text -j .data -O binary $(APP).elf $(APP).bin

$(TARGET): $(APP).bin
	cat $(APP).bin /dev/zero | head -c $(PMEM_SIZE) | hexdump -v -e '/2 "%.4x\n"' > $(TARGET)

install: $(TARGET)
	cp $< $(SYNTH_DIR)/$<
	cp $< $(TB_DIR)/$<

clean:
	rm -f $(TARGET) *.o *.elf *.mem *.disasm *.hex *.bin $(LIB_DIR)/*.o
	
clean-all:
	rm -f $(TARGET) $(SYNTH_DIR)/$(TARGET) $(TB_DIR)/$(TARGET) *.o *.elf *.mem *.disasm *.hex *.bin $(LIB_DIR)/*.o
	
