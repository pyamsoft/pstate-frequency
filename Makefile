include config.mk

PROJECT_ROOT=project
EXEC_NAME=pstate-frequency
SOURCES=$(wildcard $(PROJECT_ROOT)/src/*.cpp)
OBJECTS=$(patsubst $(PROJECT_ROOT)/src/%.cpp,$(PROJECT_ROOT)/obj/%.o,$(SOURCES))
UDEV=99-$(EXEC_NAME).rules
SYSTEMD=$(EXEC_NAME).service
TARGET=$(PROJECT_ROOT)/bin/$(EXEC_NAME)

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
	@echo "  INSTALL  $(DESTDIR)$(PREFIX)/$(TARGET)"
	@install -d $(DESTDIR)$(PREFIX)/bin
	@install -m $(BIN_PERMISSION) $(TARGET) $(DESTDIR)$(PREFIX)/bin
ifeq ($(INCLUDE_BASH_COMPLETION), 1)
	@echo -n "  INSTALL  "
	@echo "$(DESTDIR)/etc/bash_completion/$(EXEC_NAME)"
	@install -d $(DESTDIR)/etc/bash_completion.d
	@install -m 644 assets/shell/bash/bash_completion \
		$(DESTDIR)/etc/bash_completion.d/$(EXEC_NAME)
endif
ifeq ($(INCLUDE_ZSH_COMPLETION), 1)
	@echo -n "  INSTALL  "
	@echo "$(DESTDIR)$(PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)"
	@install -d $(DESTDIR)$(PREFIX)/share/zsh/site-functions
	@install -m 644 assets/shell/zsh/zsh_completion \
		$(DESTDIR)$(PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)
endif
ifeq ($(INCLUDE_UDEV_RULE), 1)
	@echo "  INSTALL  $(DESTDIR)/etc/udev/rules.d/$(UDEV)"
	@install -d $(DESTDIR)/etc/udev/rules.d
	@install -m 644 assets/udev/$(UDEV) \
		$(DESTDIR)/etc/udev/rules.d/$(UDEV)
	@sed -i "s#$(EXEC_NAME)#$(PREFIX)/bin/$(EXEC_NAME)#g" \
		$(DESTDIR)/etc/udev/rules.d/$(UDEV)
endif
ifeq ($(INCLUDE_SYSTEMD_UNIT), 1)
	@echo "  INSTALL  $(DESTDIR)/usr/lib/systemd/system/$(SYSTEMD)"
	@install -d $(DESTDIR)/usr/lib/systemd/system
	@install -m 644 assets/systemd/$(SYSTEMD) \
		$(DESTDIR)/usr/lib/systemd/system/$(SYSTEMD)
	@sed -i "s#$(EXEC_NAME)#$(PREFIX)/bin/$(EXEC_NAME)#g" \
		$(DESTDIR)/usr/lib/systemd/system/$(SYSTEMD)
endif

uninstall:
ifeq ($(INCLUDE_BASH_COMPLETION), 1)
	@echo -n "  UNINSTALL  "
	@echo "$(DESTDIR)/etc/bash_completion.d/$(EXEC_NAME)"
	@rm -f $(DESTDIR)/etc/bash_completion.d/$(EXEC_NAME)
endif
ifeq ($(INCLUDE_ZSH_COMPLETION), 1)
	@echo -n "  UNINSTALL  "
	@echo "$(DESTDIR)$(PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)"
	@rm -f $(DESTDIR)$(PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)
endif
ifeq ($(INCLUDE_UDEV_RULE), 1)
	@echo "  UNINSTALL  $(DESTDIR)/etc/udev/rules.d/$(UDEV)"
	@rm -f $(DESTDIR)/etc/udev/rules.d/$(UDEV)
endif
ifeq ($(INCLUDE_SYSTEMD_UNIT), 1)
	@echo "  UNINSTALL  $(DESTDIR)/usr/lib/systemd/system/$(SYSTEMD)"
	@rm -f $(DESTDIR)/usr/lib/systemd/system/$(SYSTEMD)
endif
	@echo "  UNINSTALL  $(DESTDIR)$(PREFIX)/bin/$(EXEC_NAME)"
	@rm -f $(DESTDIR)$(PREFIX)/bin/$(EXEC_NAME)
	@echo "DONE"
