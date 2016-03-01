# The MIT License (MIT)
#
# Copyright (c) 2016 Peter Kenji Yamanaka
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.

include ../config.mk

EXEC_NAME=pstate-frequency

# Change this to 'build' if you wish to build into the app directory
# Otherwise this will build on the tmpfs mounted at /tmp by default
BUILD_DIR=/tmp/$(EXEC_NAME)
# BUILD_DIR=build

OBJECTS=$(patsubst src/%.c,$(BUILD_DIR)/obj/%.o,$(wildcard src/*.c))
TARGET=$(BUILD_DIR)/bin/$(EXEC_NAME)

RES_DIR=res
SYSTEM_PREFIX=/usr
BASH_COMP_DIR=share/bash-completion/completions
ZSH_COMP_DIR=share/zsh/site-functions
SYSTEMD_UNIT_DIR=lib/systemd/system
UDEV_RULES_DIR=lib/udev/rules.d

UDEV=99-$(EXEC_NAME).rules
SYSTEMD=$(EXEC_NAME).service
SYSTEMD_SLEEP=$(EXEC_NAME)-sleep.service
X86_NAME=x86_energy_perf_policy
X86_SERVICE=$(X86_NAME).service
X86_SLEEP=$(X86_NAME)-sleep.service

.PHONY: all dirs clean install-app uninstall-app install-res install-bash \
	install-zsh install-udev install-systemd install-systemd-pstate \
	install-systemd-x86 install-systemd-sleep-pstate \
	install-systemd-sleep-x86 uninstall-res uninstall-bash \
	uninstall-zsh uninstall-udev uninstall-systemd

all: $(TARGET)

$(OBJECTS): | $(BUILD_DIR)/obj

$(BUILD_DIR)/obj:
	@mkdir -p $@

$(BUILD_DIR)/obj/%.o: src/%.c
	@echo "  CC  $@"
	@$(CC) -c $< -o $@ $(CFLAGS)

$(TARGET): dirs $(OBJECTS)
	@echo "  LD  $@"
	@$(CC) -o $@ $(OBJECTS) $(LDFLAGS)

dirs:
	@mkdir -p $(BUILD_DIR)/obj
	@mkdir -p $(BUILD_DIR)/bin

clean:
	@rm -rf $(TARGET) $(OBJECTS) $(BUILD_DIR)/obj $(BUILD_DIR)/bin $(BUILD_DIR) $(BUILD_DIR)
	@echo "  CLEAN PROJECT"

install-app: all
	@echo "  INSTALL  $(DESTDIR)$(PREFIX)/bin/$(EXEC_NAME)"
	@install -Dm 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(EXEC_NAME)

uninstall-app:
	@echo "  UNINSTALL  $(DESTDIR)$(PREFIX)/bin/$(EXEC_NAME)"
	@rm -f $(DESTDIR)$(PREFIX)/bin/$(EXEC_NAME)

install-res:
ifeq ($(INCLUDE_BASH_COMPLETION), 1)
	@$(MAKE) install-bash
endif
ifeq ($(INCLUDE_ZSH_COMPLETION), 1)
	@$(MAKE) install-zsh
endif
ifeq ($(INCLUDE_UDEV_RULE), 1)
	@$(MAKE) install-udev
endif
ifeq ($(INCLUDE_SYSTEMD_UNIT), 1)
	@$(MAKE) install-systemd
endif

install-bash:
	@echo -n "  INSTALL  "
	@echo "$(DESTDIR)$(SYSTEM_PREFIX)/$(BASH_COMP_DIR)/$(EXEC_NAME)"
	@install -Dm 644 $(RES_DIR)/shell/bash/bash_completion \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(BASH_COMP_DIR)/$(EXEC_NAME)

install-zsh:
	@echo -n "  INSTALL  "
	@echo "$(DESTDIR)$(SYSTEM_PREFIX)/$(ZSH_COMP_DIR)/_$(EXEC_NAME)"
	@install -Dm 644 $(RES_DIR)/shell/zsh/zsh_completion \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(ZSH_COMP_DIR)/_$(EXEC_NAME)

install-udev:
	@echo "  INSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/$(UDEV_RULES_DIR)/$(UDEV)"
	@install -Dm 644 $(RES_DIR)/udev/$(UDEV) \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(UDEV_RULES_DIR)/$(UDEV)
	@sed -i "s#$(EXEC_NAME)#$(PREFIX)/bin/$(EXEC_NAME)#g" \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(UDEV_RULES_DIR)/$(UDEV)

install-systemd:
	@$(MAKE) install-systemd-pstate
	@$(MAKE) install-systemd-x86
	@$(MAKE) install-systemd-sleep-pstate
	@$(MAKE) install-systemd-sleep-x86

install-systemd-pstate:
	@echo "  INSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(SYSTEMD)"
	@install -Dm 644 $(RES_DIR)/systemd/$(SYSTEMD) \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(SYSTEMD)
	@sed -i "s#$(EXEC_NAME)#$(PREFIX)/bin/$(EXEC_NAME)#g" \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(SYSTEMD)

install-systemd-sleep-pstate:
	@echo "  INSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(SYSTEMD_SLEEP)"
	@install -Dm 644 $(RES_DIR)/systemd/$(SYSTEMD_SLEEP) \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(SYSTEMD_SLEEP)
	@sed -i "s#$(EXEC_NAME)#$(PREFIX)/bin/$(EXEC_NAME)#g" \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(SYSTEMD_SLEEP)

install-systemd-x86:
	@echo "  INSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SERVICE)"
	@install -Dm 644 $(RES_DIR)/systemd/$(X86_SERVICE) \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SERVICE)
# @sed -i "s#$(X86_NAME)#$(PREFIX)/bin/$(X86_NAME)#g" \
# 	$(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SERVICE)
ifeq ($(X86_ENERGY_PERF_POLICY), 0)
	@sed -i "s#-v normal#-v powersave#g" \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SERVICE)
else ifeq ($(X86_ENERGY_PERF_POLICY), 2)
	@sed -i "s#-v normal#-v performance#g" \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SERVICE)
endif

install-systemd-sleep-x86:
	@echo "  INSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SLEEP)"
	@install -Dm 644 $(RES_DIR)/systemd/$(X86_SLEEP) \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SLEEP)
	@sed -i "s#$(X86_NAME)#$(PREFIX)/bin/$(X86_NAME)#g" \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SLEEP)
ifeq ($(X86_ENERGY_PERF_POLICY), 0)
	@sed -i "s#-v normal#-v powersave#g" \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SLEEP)
else ifeq ($(X86_ENERGY_PERF_POLICY), 2)
	@sed -i "s#-v normal#-v performance#g" \
		$(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SLEEP)
endif

uninstall-res:
ifeq ($(INCLUDE_BASH_COMPLETION), 1)
	@$(MAKE) uninstall-bash
endif
ifeq ($(INCLUDE_ZSH_COMPLETION), 1)
	@$(MAKE) uninstall-zsh
endif
ifeq ($(INCLUDE_UDEV_RULE), 1)
	@$(MAKE) uninstall-udev
endif
ifeq ($(INCLUDE_SYSTEMD_UNIT), 1)
	@$(MAKE) uninstall-systemd
endif

uninstall-bash:
	@echo -n "  UNINSTALL  "
	@echo "$(DESTDIR)$(SYSTEM_PREFIX)/$(BASH_COMP_DIR)/$(EXEC_NAME)"
	@rm -f $(DESTDIR)$(SYSTEM_PREFIX)/$(BASH_COMP_DIR)/$(EXEC_NAME)

uninstall-zsh:
	@echo -n "  UNINSTALL  "
	@echo "$(DESTDIR)$(SYSTEM_PREFIX)/$(ZSH_COMP_DIR)/_$(EXEC_NAME)"
	@rm -f $(DESTDIR)$(SYSTEM_PREFIX)/$(ZSH_COMP_DIR)/_$(EXEC_NAME)

uninstall-udev:
	@echo "  UNINSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/$(UDEV_RULES_DIR)/$(UDEV)"
	@rm -f $(DESTDIR)$(SYSTEM_PREFIX)/$(UDEV_RULES_DIR)/$(UDEV)

uninstall-systemd:
	@echo "  UNINSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(SYSTEMD)"
	@rm -f $(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(SYSTEMD)
	@echo "  UNINSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(SYSTEMD_SLEEP)"
	@rm -f $(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(SYSTEMD_SLEEP)
	@echo "  UNINSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SERVICE)"
	@rm -f $(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SERVICE)
	@echo "  UNINSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SLEEP)"
	@rm -f $(DESTDIR)$(SYSTEM_PREFIX)/$(SYSTEMD_UNIT_DIR)/$(X86_SLEEP)

