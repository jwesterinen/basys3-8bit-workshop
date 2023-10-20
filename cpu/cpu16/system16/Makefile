# make file for a cpu16 project from the 8-bit Workshop on the Basys3 board

PROJ = system16

PROJ_SOURCES = $(SRC_DIR)/../cpu16.v $(COMMON_DIR)/rom_sync.v $(COMMON_DIR)/ram_sync.v $(COMMON_DIR)/basic_io_16.v $(COMMON_DIR)/sound_io_16.v $(SRC_DIR)/rom.bin \
               $(VGA_DIR)/sound_generator/SN76477.v $(VGA_DIR)/sound_generator/lfsr.v $(COMMON_DIR)/keypad_io_16.v $(COMMON_DIR)/PmodKYPD.v

PROJ_XDC_FILE = $(SRC_DIR)/$(PROJ).xdc

COMMON_DIR = $(SRC_DIR)/../..
SRC_DIR = .
VGA_DIR = ../../../vga25Mhz
BUILD_DIR = build

STATUS_FILE = status
TARGET = $(BUILD_DIR)/$(PROJ).bit
DEVICE = arty_a7_35t

SOURCES = $(COMMON_DIR)/prescaler.v $(SRC_DIR)/$(PROJ).v $(PROJ_SOURCES) $(PROJ_XDC_FILE)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p build
	# Use Xilinx Vivado to compile the sources to a .bit file
	/tools/Xilinx/Vivado/2019.1/bin/vivado -mode tcl < $(PROJ).tcl > $(BUILD_DIR)/$(STATUS_FILE)

install: $(TARGET)
	openFPGALoader -b $(DEVICE) $<

flash: $(TARGET)
	openFPGALoader -b $(DEVICE) --write-flash $<

.PHONY: clean
clean:
	rm -f *.html *.xml *.jou *.log build/*

