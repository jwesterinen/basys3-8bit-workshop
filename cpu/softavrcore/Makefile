SHELL=/bin/sh

.PHONY: all clean

all:
	$(MAKE) -C build-test
	$(MAKE) -C build-freertos
	$(MAKE) -C synth

clean:
	$(MAKE) -C build-test clean
	$(MAKE) -C build-freertos clean
	$(MAKE) -C build-freertos clean-dummy
	$(MAKE) -C synth clean
	$(MAKE) -C synth clean-dummy

