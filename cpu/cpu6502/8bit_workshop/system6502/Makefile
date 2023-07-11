# make file for a cpu16 project from the 8-bit Workshop on the Basys3 board

PROJ = system6502
PROJ_SOURCES = $(SRC_DIR)/../cpu6502.v $(COMMON_DIR)/rom_sync.v $(COMMON_DIR)/ram_sync.v $(SRC_DIR)/rom.bin

COMMON_DIR = $(SRC_DIR)/../../..
SRC_DIR = .
XDC_DIR = $(COMMON_DIR)
BUILD_DIR = build

STATUS_FILE = status
TARGET = $(BUILD_DIR)/$(PROJ).bit
DEVICE = arty_a7_35t

SOURCES = $(COMMON_DIR)/prescaler.v $(SRC_DIR)/$(PROJ).v $(PROJ_SOURCES)

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

