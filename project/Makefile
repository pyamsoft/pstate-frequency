include config.mk

EXEC_NAME=pstate-frequency
OBJECTS=$(patsubst src/%.cpp,obj/%.o, $(wildcard src/*.cpp))
UDEV=99-$(EXEC_NAME).rules
SYSTEMD=$(EXEC_NAME).service
TARGET=bin/$(EXEC_NAME)

all: $(TARGET)

options:
	@echo "CXXFLAGS  = " $(CXXFLAGS)
	@echo "LDFLAGS   = " $(LDFLAGS)
	@echo "MAKEFLAGS = " $(MAKEFLAGS)
	@echo "CXX       = " $(CXX)

$(OBJECTS): | obj

obj:
	@mkdir -p $@

obj/%.o : src/%.cpp
	@echo "  CXX  $@"
	@$(CXX) -c $< -o $@ $(CXXFLAGS)

$(TARGET): dirs $(OBJECTS)
	@echo "  LD   $@"
	@$(CXX) -o $@ $(OBJECTS) $(LDFLAGS)

dirs:
	@mkdir -p obj
	@mkdir -p bin

clean:
	@rm -rf $(TARGET) $(OBJECTS) obj bin
	@echo "  CLEAN PROJECT"

install: all
	@echo "  INSTALL  $(DESTDIR)$(PREFIX)/$(TARGET)"
	@install -d $(DESTDIR)$(PREFIX)/bin
	@install -m $(BIN_PERMISSION) $(TARGET) $(DESTDIR)$(PREFIX)/bin
ifeq ($(INCLUDE_BASH_COMPLETION), 1)
	@echo -n "  INSTALL  "
	@echo "$(DESTDIR)/etc/bash_completion/$(EXEC_NAME)"
	@install -d $(DESTDIR)/etc/bash_completion.d
	@install -m 644 ../assets/shell/bash/bash_completion \
		$(DESTDIR)/etc/bash_completion.d/$(EXEC_NAME)
endif
ifeq ($(INCLUDE_ZSH_COMPLETION), 1)
	@echo -n "  INSTALL  "
	@echo "$(DESTDIR)$(PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)"
	@install -d $(DESTDIR)$(PREFIX)/share/zsh/site-functions
	@install -m 644 ../assets/shell/zsh/zsh_completion \
		$(DESTDIR)$(PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)
endif
ifeq ($(INCLUDE_UDEV_RULE), 1)
	@echo "  INSTALL  $(DESTDIR)/etc/udev/rules.d/$(UDEV)"
	@install -d $(DESTDIR)/etc/udev/rules.d
	@install -m 644 ../assets/udev/$(UDEV) \
		$(DESTDIR)/etc/udev/rules.d/$(UDEV)
	@sed -i "s#$(EXEC_NAME)#$(PREFIX)/bin/$(EXEC_NAME)#g" \
		$(DESTDIR)/etc/udev/rules.d/$(UDEV)
endif
ifeq ($(INCLUDE_SYSTEMD_UNIT), 1)
	@echo "  INSTALL  $(DESTDIR)/usr/lib/systemd/system/$(SYSTEMD)"
	@install -d $(DESTDIR)/usr/lib/systemd/system
	@install -m 644 ../assets/systemd/$(SYSTEMD) \
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

