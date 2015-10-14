include config.mk

EXEC_NAME=pstate-frequency
PROJECT_DIR=app
DOC_DIR=share/doc

.PHONY: all clean install install-src install-doc install-license uninstall \
	uninstall-src uninstall-doc uninstall-license options bin

all:
	@echo " -- Makefile targets --"
	@echo
	@echo "  bin       - Build the program using options specified in the"
	@echo "              Makefile"
	@echo "  options   - List the build environment options including flags"
	@echo "  clean     - Clean the build to start from scratch"
	@echo "  install   - Install the program using options specified in the"
	@echo "              Makefile"
	@echo "  uninstall - Uninstall the program using options specified in the"
	@echo "              Makefile"

bin:
	@$(MAKE) -C $(PROJECT_DIR)

options:
	@echo "CFLAGS  = " $(CFLAGS)
	@echo "LDFLAGS   = " $(LDFLAGS)
	@echo "MAKEFLAGS = " $(MAKEFLAGS)
	@echo "CC       = " $(CC)

clean:
	@$(MAKE) -C $(PROJECT_DIR) clean

install: all
	@$(MAKE) -C $(PROJECT_DIR) install-app
	@$(MAKE) -C $(PROJECT_DIR) install-res
ifeq ($(INCLUDE_SRC), 1)
	@$(MAKE) install-src
endif
ifeq ($(INCLUDE_DOC), 1)
	@$(MAKE) install-doc
endif

ifeq ($(INCLUDE_LICENSE), 1)
	@$(MAKE) install-license
endif

install-doc:
	@echo "  INSTALL  $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/README.md"
	@install -Dm 644 README.md $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)

install-license:
	@echo "  INSTALL  $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/LICENSE"
	@install -Dm 644 LICENSE $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)

install-src:
	@echo "  INSTALL  $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/src"
	@install -d $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)
	@cp -r --no-preserve=mode $(PROJECT_DIR)/src $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/src

uninstall:
	@$(MAKE) -C $(PROJECT_DIR) uninstall-app
	@$(MAKE) -C $(PROJECT_DIR) uninstall-res
ifeq ($(INCLUDE_SRC), 1)
	@$(MAKE) uninstall-src
endif
ifeq ($(INCLUDE_DOC), 1)
	@$(MAKE) uninstall-doc
endif
ifeq ($(INCLUDE_LICENSE), 1)
	@$(MAKE) uninstall-license
endif

uninstall-doc:
	@echo "  UNINSTALL  $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/README.md"
	@rm -f $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/README.md
	@rmdir $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME) --ignore-fail-on-non-empty

uninstall-license:
	@echo "  UNINSTALL  $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/LICENSE"
	@rm -f $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/LICENSE
	@rmdir $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME) --ignore-fail-on-non-empty

uninstall-src:
	@echo "  UNINSTALL  $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/src"
	@rm -rf $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/src
	@rmdir $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME) --ignore-fail-on-non-empty

