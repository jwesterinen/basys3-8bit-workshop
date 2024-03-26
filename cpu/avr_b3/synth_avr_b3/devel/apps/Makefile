SHELL=/bin/sh

.PHONY: all clean clean-all

all:
	$(MAKE) -C build-test
	$(MAKE) -C calc
	$(MAKE) -C synthesizer
	$(MAKE) -C ram_test

clean:
	$(MAKE) -C build-test clean
	$(MAKE) -C calc clean
	$(MAKE) -C synthesizer clean
	$(MAKE) -C ram_test clean
	
clean-all:
	$(MAKE) -C build-test clean-all
	$(MAKE) -C calc clean-all
	$(MAKE) -C synthesizer clean-all
	$(MAKE) -C ram_test clean-all
	$(MAKE) -C mmio_test clean-all
	

