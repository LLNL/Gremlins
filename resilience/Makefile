include Makefile.config

all:
	cd src && $(MAKE) $@

clean:
	cd src && $(MAKE) $@
	rm -f $(CURDIR)/lib/*

install: 
	install -d $(CURDIR)/lib
	install $(CURDIR)/src/fault_injector/*.so $(CURDIR)/lib/

.PHONY: all clean install
