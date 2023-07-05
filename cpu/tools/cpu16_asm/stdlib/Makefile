TARGET  = hack_stdlib.ho
HEADER  = hack_stdlib.h
DEFS    = hackdefs.h

DEPS = keyboard.ho display.ho io.ho math.ho sys.ho

all: $(TARGET)

$(TARGET): $(DEPS)
	cat $^ > $@

clean:
	rm -f $(TARGET)

install:
	/usr/bin/install -m 644 $(TARGET) $(INST_LIB_DIR)
	/usr/bin/install -m 644 $(HEADER) $(INST_INCL_DIR)
	/usr/bin/install -m 644 $(DEFS) $(INST_INCL_DIR)

uninstall:
	rm -f $(INST_LIB_DIR)/$(TARGET)
	rm -f $(INST_INCL_DIR)/$(HEADER)
	rm -f $(INST_INCL_DIR)/$(DEFS)

.PHONY: clean install uninstall
	
