# make file for system_avr

PROJ = system_avr

PROJ_SOURCES = $(SRC_DIR)/avr_core.v $(SRC_DIR)/avr_io_out.v $(SRC_DIR)/flash.v $(SRC_DIR)/ram.v $(SRC_DIR)/rom.bin

PROJ_XDC_FILE = $(SRC_DIR)/$(PROJ).xdc

SRC_DIR = .
BUILD_DIR = build

STATUS_FILE = status
TARGET = $(BUILD_DIR)/$(PROJ).bit
DEVICE = arty_a7_35t

SOURCES = $(SRC_DIR)/$(PROJ).v $(PROJ_SOURCES) $(PROJ_XDC_FILE)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p build
	/tools/Xilinx/Vivado/2019.1/bin/vivado -mode tcl < $(PROJ).tcl > $(BUILD_DIR)/$(STATUS_FILE)

install: $(TARGET)
	openFPGALoader -b $(DEVICE) $<

flash: $(TARGET)
	openFPGALoader -b $(DEVICE) --write-flash $<

.PHONY: clean
clean:
	rm -f *.html *.xml *.jou *.log build/*

