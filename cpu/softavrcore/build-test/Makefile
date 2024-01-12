SHELL=/bin/sh

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

#PROGMEM=../util/progmem-ice40.sh
PROGMEM=../util/progmem-generic.sh

.PHONY: all clean dummy

TARGETS=dummy main.disasm main.mem flash_array.v main.instr main.v

all: $(TARGETS)

dummy: dummy.v dummy.mem

crt0.o: crt0.S
	$(AS) $(AFLAGS) -o crt0.o crt0.S

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

main.elf: main.o $(ARCH_LIB)
	$(CC) $(LDFLAGS) -o main.elf main.o $(ARCH_LIB)

main.hex: main.elf
	$(OBJCOPY) -j .text -j .data -O ihex main.elf main.hex

main.bin: main.elf
	$(OBJCOPY) -j .text -j .data -O binary main.elf main.bin
	$(CHMOD) -x main.bin

main.mem: main.bin
	cat main.bin /dev/zero | head -c $(PMEM_SIZE) | hexdump -v -e '/2 "%.4x\n"' > main.mem

flash_array.v: main.mem
	awk '{ printf("flash_array[%d]=16%ch%s;\n",n,39,$$1);n++; }' < main.mem > flash_array.v

main.disasm: main.elf
	$(OBJDUMP) -s -m $(ARCH) -d main.elf > main.disasm

main.instr: main.disasm
	expand < main.disasm | grep -v "file format" | awk '{ if ( substr($$1,length($$1))==":" ) print substr($$0,25,8); }' | sort | uniq | awk '{ print $$1; }' > main.instr

main.v: main.bin
	$(PROGMEM) --depth $(PMEM_DEPTH) --name flash main.bin > main.v

dummy.bin:
	$(DD) if=/dev/urandom bs=$(PMEM_WORDS) count=2 of=dummy.bin 2>/dev/null

dummy.mem: dummy.bin
	cat dummy.bin /dev/zero | head -c $(PMEM_SIZE) | hexdump -v -e '/2 "%.4x\n"' > dummy.mem

dummy.v: dummy.bin
	$(PROGMEM) --depth $(PMEM_DEPTH) --name flash dummy.bin > dummy.v

clean:
	rm -f $(TARGETS) *.o *.elf *.mem *.disasm *.hex *.bin
