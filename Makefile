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
ifeq ($(INCLUDE_SRC), 1)
	@$(MAKE) install-src
endif
ifeq ($(INCLUDE_DOC), 1)
	@$(MAKE) install-doc
endif

install-doc:
	@echo "  INSTALL  $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/README.md"
	@install -d $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)
	@install -m 644 README.md $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)

install-src:
	@echo "  INSTALL  $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/project"
	@install -d $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/project
	@cp -r --no-preserve=mode project/include $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/project
	@cp -r --no-preserve=mode project/src $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/project

uninstall:
	@$(MAKE) -C project uninstall
	@$(MAKE) -C assets uninstall
ifeq ($(INCLUDE_SRC), 1)
	@$(MAKE) uninstall-src
endif
ifeq ($(INCLUDE_DOC), 1)
	@$(MAKE) uninstall-doc
endif

uninstall-doc:
	@echo "  UNINSTALL  $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/README.md"
	@rm -f $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/README.md
	@rmdir $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME) --ignore-fail-on-non-empty

uninstall-src:
	@echo "  UNINSTALL  $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/project"
	@rm -rf $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME)/project
	@rmdir $(DESTDIR)$(PREFIX)/share/doc/$(EXEC_NAME) --ignore-fail-on-non-empty

