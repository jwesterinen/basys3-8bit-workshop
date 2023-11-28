app ?= 
TARGET = $(app).hack

OBJ = $(app).hasm

%.hasm: %.c
	hcc $<

%.hack: %.hasm
	hasm $<

all: $(TARGET)

$(TARGET): $(OBJ)

clean:
	rm -f $(TARGET) *.hasm *.ho

install:
	cp $(TARGET) ../../hack

.PHONY: clean install uninstall
	
