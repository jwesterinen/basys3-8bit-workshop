SHELL=/bin/sh

.PHONY: all clean clean-all

all:
	$(MAKE) -C build-test
	$(MAKE) -C calc
	$(MAKE) -C synthesizer

clean:
	$(MAKE) -C build-test clean
	$(MAKE) -C calc clean
	$(MAKE) -C synthesizer clean
	
clean-all:
	$(MAKE) -C build-test clean-all
	$(MAKE) -C calc clean-all
	$(MAKE) -C synthesizer clean-all
	

