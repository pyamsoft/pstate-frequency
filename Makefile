include config.mk

.PHONY: all clean install uninstall options

all:
	@$(MAKE) -C project

options:
	@echo "CXXFLAGS  = " $(CXXFLAGS)
	@echo "LDFLAGS   = " $(LDFLAGS)
	@echo "MAKEFLAGS = " $(MAKEFLAGS)
	@echo "CXX       = " $(CXX)

clean:
	@$(MAKE) -C project clean

install: all
	@$(MAKE) -C project install
	@$(MAKE) -C assets install

uninstall:
	@$(MAKE) -C project uninstall
	@$(MAKE) -C assets uninstall
