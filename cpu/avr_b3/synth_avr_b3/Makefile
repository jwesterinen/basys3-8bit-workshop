# make file for avr_b3

PROJ = avr_b3

PROJ_SOURCES = top.v avr_core.v flash.v ram.v prescaler.v basic_io_b3.v display_io_b3.v keypad_io_b3.v PmodKYPD.v rom.mem

PROJ_XDC_FILE = $(PROJ).xdc

BUILD_DIR = build

STATUS_FILE = status
TARGET = $(BUILD_DIR)/$(PROJ).bit
DEVICE = arty_a7_35t

SOURCES = $(PROJ_SOURCES) $(PROJ_XDC_FILE)

.PHONY: all
all: $(TARGET)

$(TARGET): $(SOURCES)
	mkdir -p build
	/tools/Xilinx/Vivado/2019.1/bin/vivado -mode tcl < $(PROJ).tcl > $(BUILD_DIR)/$(STATUS_FILE)

install: $(TARGET)
	openFPGALoader -b $(DEVICE) $<

flash: $(TARGET)
	openFPGALoader -b $(DEVICE) --write-flash $<

.PHONY: clean clean-all
clean:
	rm -f *.html *.xml *.jou *.log build/*
	
clean-all:
	rm -f *.html *.xml *.jou *.log build/*
	rm -fr .Xil
	$(MAKE) -C devel clean-all
	

