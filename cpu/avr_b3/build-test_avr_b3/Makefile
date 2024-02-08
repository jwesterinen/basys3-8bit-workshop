SHELL=/bin/sh

APP=main
APP_OBJS=$(APP).o 

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
#CFLAGS=-mmcu=$(ARCH) -D__COMPILING_AVR_LIBC__ -Wall -pedantic -O3
CFLAGS=-mmcu=$(ARCH) -D__COMPILING_AVR_LIBC__ -funsigned-char -funsigned-bitfields -fpack-struct -fshort-enums -Wall -Wstrict-prototypes -Wno-unused-function -O3
LDFLAGS=-mmcu=$(ARCH)

PMEM_DEPTH=12
PMEM_WORDS=$(shell echo $$((1<<$(PMEM_DEPTH))))
PMEM_SIZE=$(shell echo $$((2<<$(PMEM_DEPTH))))

.PHONY: all clean dummy

TARGET=$(APP).mem

all: $(TARGET)

$(ARCH_LIB): crt0.S
	$(AS) $(AFLAGS) -o crt0.o crt0.S

$(APP).elf: $(APP_OBJS) $(ARCH_LIB)
	$(CC) $(LDFLAGS) -o $@ $^
	$(OBJDUMP) -s -m $(ARCH) -d $(APP).elf > $(APP).disasm

$(APP).bin: $(APP).elf
	$(OBJCOPY) -j .text -j .data -O binary $(APP).elf $(APP).bin

$(TARGET): $(APP).bin
	cat $(APP).bin /dev/zero | head -c $(PMEM_SIZE) | hexdump -v -e '/2 "%.4x\n"' > $(TARGET)

clean:
	rm -f $(TARGET) *.o *.elf *.mem *.disasm *.hex *.bin
