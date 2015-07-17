include config.mk

EXEC_NAME=pstate-frequency
PROJECT_DIR=app
DOC_DIR=share/doc

.PHONY: all clean install install-src install-doc uninstall uninstall-src \
	uninstall-doc options

all:
	@$(MAKE) -C $(PROJECT_DIR)

options:
	@echo "CXXFLAGS  = " $(CXXFLAGS)
	@echo "LDFLAGS   = " $(LDFLAGS)
	@echo "MAKEFLAGS = " $(MAKEFLAGS)
	@echo "CXX       = " $(CXX)

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

install-doc:
	@echo "  INSTALL  $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/README.md"
	@install -d $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)
	@install -m 644 README.md $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)

install-src:
	@echo "  INSTALL  $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/src"
	@install -d $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/src
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

uninstall-doc:
	@echo "  UNINSTALL  $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/README.md"
	@rm -f $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/README.md
	@rmdir $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME) --ignore-fail-on-non-empty

uninstall-src:
	@echo "  UNINSTALL  $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/src"
	@rm -rf $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME)/src
	@rmdir $(DESTDIR)$(PREFIX)/$(DOC_DIR)/$(EXEC_NAME) --ignore-fail-on-non-empty

