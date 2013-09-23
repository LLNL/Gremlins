all:
	cd fault_injector && $(MAKE) $@
	cd lulesh && patch_code.sh && $(MAKE) $@

clean:
	cd fault_injector && $(MAKE) $@
	cd lulesh && $(MAKE) $@

.PHONY: all clean
