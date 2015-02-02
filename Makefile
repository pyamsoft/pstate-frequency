include config.mk

PROJECT_ROOT=project
EXEC_NAME=pstate-frequency
SOURCES=$(wildcard $(PROJECT_ROOT)/src/*.cpp)
OBJECTS=$(patsubst $(PROJECT_ROOT)/src/%.cpp,$(PROJECT_ROOT)/obj/%.o,$(SOURCES))
UDEV=99-$(EXEC_NAME).rules
TARGET=$(PROJECT_ROOT)/bin/$(EXEC_NAME)

# The Target Build
all: $(TARGET)

options:
	@echo "CXXFLAGS  = " $(CXXFLAGS)
	@echo "LDFLAGS   = " $(LDFLAGS)
	@echo "MAKEFLAGS = " $(MAKEFLAGS)
	@echo "CXX       = " $(CXX)

$(OBJECTS): | $(PROJECT_ROOT)/obj

$(PROJECT_ROOT)/obj:
	@mkdir -p $@

$(PROJECT_ROOT)/obj/%.o : $(PROJECT_ROOT)/src/%.cpp
	@echo "  CXX  $@"
	@$(CXX) -c $< -o $@ $(CXXFLAGS)

$(TARGET): dirs $(OBJECTS)
	@echo "  LD   $@"
	@$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

dirs:
	@mkdir -p $(PROJECT_ROOT)/obj
	@mkdir -p $(PROJECT_ROOT)/bin

clean:
	@rm -rf $(TARGET) $(OBJECTS) $(PROJECT_ROOT)/obj $(PROJECT_ROOT)/bin
	@echo "  CLEAN PROJECT"

install: all
	@echo "  INSTALL  $(DESTDIR)/$(PREFIX)/$(TARGET)"
	@install -d $(DESTDIR)/$(PREFIX)/bin
	@install -m $(BIN_PERMISSION) $(TARGET) $(DESTDIR)/$(PREFIX)/bin
ifeq ($(INCLUDE_BASH_COMPLETION), 1)
	@echo "  INSTALL  $(DESTDIR)/$(ETCDIR)/bash_completion/$(EXEC_NAME)"
	@install -d $(DESTDIR)/$(ETCDIR)/bash_completion.d
	@install -m 644 assets/shell/bash/bash_completion $(DESTDIR)/$(ETCDIR)/bash_completion.d/$(EXEC_NAME)
endif
ifeq ($(INCLUDE_ZSH_COMPLETION), 1)
	@echo "  INSTALL  $(DESTDIR)/$(PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)"
	@install -d $(DESTDIR)/$(PREFIX)/share/zsh/site-functions
	@install -m 644 assets/shell/zsh/zsh_completion $(DESTDIR)/$(PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)
endif
ifeq ($(INCLUDE_UDEV_RULE), 1)
	@echo "  INSTALL  $(DESTDIR)/$(ETCDIR)/udev/rules.d/$(UDEV)"
	@install -d $(DESTDIR)/$(ETCDIR)/udev/rules.d
	@install -m 644 assets/udev/$(UDEV) $(DESTDIR)/$(ETCDIR)/udev/rules.d/$(UDEV)
	@sed -i "s#$(EXEC_NAME)#$(PREFIX)/bin/$(EXEC_NAME)#g" $(DESTDIR)/$(ETCDIR)/udev/rules.d/$(UDEV)
endif

uninstall:
ifeq ($(INCLUDE_BASH_COMPLETION), 1)
	@echo "  UNINSTALL  $(DESTDIR)/$(ETCDIR)/bash_completion.d/$(EXEC_NAME)"
	@rm -f $(DESTDIR)/$(ETCDIR)/bash_completion.d/$(EXEC_NAME)
endif
ifeq ($(INCLUDE_ZSH_COMPLETION), 1)
	@echo "  UNINSTALL  $(DESTDIR)/$(PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)"
	@rm -f $(DESTDIR)/$(PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)
endif
ifeq ($(INCLUDE_UDEV_RULE), 1)
	@echo "  UNINSTALL  $(DESTDIR)/$(ETCDIR)/udev/rules.d/$(UDEV)"
	@rm -f $(DESTDIR)/$(ETCDIR)/udev/rules.d/$(UDEV)
endif
	@echo "  UNINSTALL  $(DESTDIR)/$(PREFIX)/bin/$(EXEC_NAME)"
	@rm -f $(DESTDIR)/$(PREFIX)/bin/$(EXEC_NAME)
	@echo "DONE"

