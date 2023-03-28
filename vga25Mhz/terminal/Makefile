# make file for terminal project

PROJ = terminal

SRC_DIR = .
XDC_DIR = .
BUILD_DIR = build

STATUS_FILE = status
XDC_FILE = $(XDC_DIR)/basys3_$(PROJ).xdc
TARGET = $(BUILD_DIR)/$(PROJ).bit
DEVICE = arty_a7_35t

SOURCES = $(SRC_DIR)/$(PROJ).v $(SRC_DIR)/vgachar.v $(SRC_DIR)/prescaler.v $(SRC_DIR)/hvsync_generator.v $(SRC_DIR)/font437_rom.v $(SRC_DIR)/display_buffer.v

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

