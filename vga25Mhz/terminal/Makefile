# make file for a VGA project from the 8-bit Workshop on the Basys3 board

PROJ = terminal
PROJ_SOURCES = font437array.v

COMMON_DIR = $(SRC_DIR)/..
SRC_DIR = .
XDC_DIR = $(COMMON_DIR)
BUILD_DIR = build

STATUS_FILE = status
XDC_FILE = $(XDC_DIR)/basys3_vga.xdc
TARGET = $(BUILD_DIR)/$(PROJ).bit
DEVICE = arty_a7_35t

SOURCES = $(COMMON_DIR)/prescaler.v $(COMMON_DIR)/hvsync_generator.v $(SRC_DIR)/$(PROJ).v $(PROJ_SOURCES)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SOURCES) $(XDC_FILE)
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
	rm -fr .Xil

