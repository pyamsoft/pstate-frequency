include config.mk

.PHONY: all clean install uninstall options

all:
	@$(MAKE) -C project
	@echo
	@echo "Done."

options:
	@echo "CXXFLAGS  = " $(CXXFLAGS)
	@echo "LDFLAGS   = " $(LDFLAGS)
	@echo "MAKEFLAGS = " $(MAKEFLAGS)
	@echo "CXX       = " $(CXX)
	@echo
	@echo "Done."

clean:
	@$(MAKE) -C project clean
	@echo
	@echo "Done."

install: all
	@$(MAKE) -C project install
	@$(MAKE) -C assets install
	@echo
	@echo "Done."

uninstall:
	@$(MAKE) -C project uninstall
	@$(MAKE) -C assets uninstall
	@echo
	@echo "Done."
