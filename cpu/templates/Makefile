# make file for a CPU project from the 8-bit Workshop on the Basys3 board

PROJ = <project name>
PROJ_SOURCES = $(SRC_DIR)/<project-specific file 1>.v ... $(SRC_DIR)/<project-specific file n>.v

COMMON_DIR = $(SRC_DIR)/..
SRC_DIR = .
XDC_DIR = $(COMMON_DIR)
BUILD_DIR = build

STATUS_FILE = status
TARGET = $(BUILD_DIR)/$(PROJ).bit
DEVICE = arty_a7_35t

SOURCES = $(COMMON_DIR)/prescaler.v $(SRC_DIR)/$(PROJ).v $(PROJ_SOURCES)

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

