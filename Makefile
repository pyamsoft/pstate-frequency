include config.mk

EXEC_NAME=pstate-frequency

.PHONY: all clean install install-doc uninstall uninstall-doc options

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
	@$(MAKE) install-doc

install-doc:
	@echo "  INSTALL  $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/README.md"
	@install -d $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)
	@install -m 644 README.md $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)

uninstall:
	@$(MAKE) -C project uninstall
	@$(MAKE) -C assets uninstall
	@$(MAKE) uninstall-doc

uninstall-doc:
	@echo "  UNINSTALL  $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/README.md"
	@rm -f $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/README.md
	@rmdir $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME) --ignore-fail-on-non-empty

