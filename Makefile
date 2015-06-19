include config.mk

EXEC_NAME=pstate-frequency

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
	@echo "  INSTALL  $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/README.md"
	@install -d $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)
	@install -m 644 README.md $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)

uninstall:
	@$(MAKE) -C project uninstall
	@$(MAKE) -C assets uninstall
	@echo "  UNINSTALL  $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/README.md"
	@rm -rf $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)
