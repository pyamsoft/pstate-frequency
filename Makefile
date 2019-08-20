# The GPLv2 License
#
#   Copyright (C) 2017  Peter Kenji Yamanaka
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License along
#   with this program; if not, write to the Free Software Foundation, Inc.,
#   51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

include config.mk

# Directory for script resources
RES_DIR=res

# certain system files will only properly work situated at /usr
SYSTEM_PREFIX=/usr

SCRIPT_INSTALL_SRC="pstate-frequency"
DOC_INSTALL_SRC="README.md"
LICENSE_INSTALL_SRC="LICENSE"
BASH_INSTALL_SRC="$(RES_DIR)/shell/bash/bash_completion"
ZSH_INSTALL_SRC="$(RES_DIR)/shell/zsh/zsh_completion"
UDEV_INSTALL_SRC="$(RES_DIR)/udev/99-pstate-frequency.rules"
SYSTEMD_SERVICE_INSTALL_SRC="$(RES_DIR)/systemd/pstate-frequency.service"
SYSTEMD_SERVICE_AT_INSTALL_SRC="$(RES_DIR)/systemd/pstate-frequency@.service"
SYSTEMD_SERVICE_SLEEP_INSTALL_SRC="$(RES_DIR)/systemd/pstate-frequency-sleep.service"
POWER_PLAN_INSTALL_SRCDIR="$(RES_DIR)/plans/"

SCRIPT_INSTALL_TARGET="$(DESTDIR)/$(PREFIX)/bin/$(NAME)"
DOC_INSTALL_TARGET="$(DESTDIR)/$(PREFIX)/share/doc/$(NAME)/README.md"
LICENSE_INSTALL_TARGET="$(DESTDIR)/$(PREFIX)/share/doc/$(NAME)/LICENSE"
BASH_INSTALL_TARGET="$(DESTDIR)/$(SYSTEM_PREFIX)/share/bash-completion/completions/$(NAME)"
ZSH_INSTALL_TARGET="$(DESTDIR)/$(SYSTEM_PREFIX)/share/zsh/site-functions/_$(NAME)"
UDEV_INSTALL_TARGET="$(DESTDIR)/$(SYSTEM_PREFIX)/lib/udev/rules.d/99-$(NAME).rules"
SYSTEMD_SERVICE_INSTALL_TARGET="$(DESTDIR)/$(SYSTEM_PREFIX)/lib/systemd/system/$(NAME).service"
SYSTEMD_SERVICE_AT_INSTALL_TARGET="$(DESTDIR)/$(SYSTEM_PREFIX)/lib/systemd/system/$(NAME)@.service"
SYSTEMD_SERVICE_SLEEP_INSTALL_TARGET="$(DESTDIR)/$(SYSTEM_PREFIX)/lib/systemd/system/$(NAME)-sleep.service"

# If you decide to change this location, also update the script POWER_PLAN_CONFIG_DIR
# or pstate-frequency will not look in the correct directory
POWER_PLAN_INSTALL_TARGETDIR="$(DESTDIR)/etc/pstate-frequency.d/"

.PHONY: all install uninstall edit \
	install-doc install-license install-script install-res \
	install-bash install-zsh install-udev install-systemd \
	install-systemd-pstate install-systemd-pstate-sleep \
	install-power-plans \
	uninstall-script uninstall-doc uninstall-license uninstall-res \
	uninstall-bash uninstall-zsh uninstall-udev uninstall-systemd \
	uninstall-systemd-pstate uninstall-systemd-pstate-sleep \
	uninstall-power-plans

all:
	@echo "Targets"
	@echo " install uninstall edit"
	@echo $(TARGET)

edit:
ifndef EDITOR
	@echo "No \$$EDITOR defined. Editing config.mk using vi"
	@vi -- config.mk
else
	@echo "Editing config.mk using \$$EDITOR ($(EDITOR))"
	@$(EDITOR) -- config.mk
endif

install:
	@echo "Installing..."
	@$(MAKE) install-script
	@$(MAKE) install-res
ifeq ($(INCLUDE_DOC), 1)
	@$(MAKE) install-doc
endif

ifeq ($(INCLUDE_LICENSE), 1)
	@$(MAKE) install-license
endif

install-script:
	@echo "  INSTALL  $(SCRIPT_INSTALL_TARGET)"
	@mkdir -p "$(shell dirname $(SCRIPT_INSTALL_TARGET))"
	@install -Dm 755 "$(SCRIPT_INSTALL_SRC)" "$(SCRIPT_INSTALL_TARGET)"

install-doc:
	@echo "  INSTALL  $(DOC_INSTALL_TARGET)"
	@mkdir -p "$(shell dirname $(DOC_INSTALL_TARGET))"
	@install -Dm 644 "$(DOC_INSTALL_SRC)" "$(DOC_INSTALL_TARGET)"

install-license:
	@echo "  INSTALL  $(LICENSE_INSTALL_TARGET)"
	@mkdir -p "$(shell dirname $(LICENSE_INSTALL_TARGET))"
	@install -Dm 644 "$(LICENSE_INSTALL_SRC)" "$(LICENSE_INSTALL_TARGET)"

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
ifeq ($(INCLUDE_POWER_PLANS), 1)
	@$(MAKE) install-power-plans
endif

install-bash:
	@echo "  INSTALL  $(BASH_INSTALL_TARGET)"
	@mkdir -p "$(shell dirname $(BASH_INSTALL_TARGET))"
	@install -Dm 644 "$(BASH_INSTALL_SRC)" "$(BASH_INSTALL_TARGET)"

install-zsh:
	@echo "  INSTALL  $(ZSH_INSTALL_TARGET)"
	@mkdir -p "$(shell dirname $(ZSH_INSTALL_TARGET))"
	@install -Dm 644 "$(ZSH_INSTALL_SRC)" "$(ZSH_INSTALL_TARGET)"

install-udev:
	@echo "  INSTALL  $(UDEV_INSTALL_TARGET)"
	@mkdir -p "$(shell dirname $(UDEV_INSTALL_TARGET))"
	@install -Dm 644 "$(UDEV_INSTALL_SRC)" "$(UDEV_INSTALL_TARGET)"

install-systemd:
	@$(MAKE) install-systemd-pstate
	@$(MAKE) install-systemd-pstate-at
	@$(MAKE) install-systemd-pstate-sleep

install-systemd-pstate:
	@echo "  INSTALL  $(SYSTEMD_SERVICE_INSTALL_TARGET)"
	@mkdir -p "$(shell dirname $(SYSTEMD_SERVICE_INSTALL_TARGET))"
	@install -Dm 644 "$(SYSTEMD_SERVICE_INSTALL_SRC)" "$(SYSTEMD_SERVICE_INSTALL_TARGET)"

install-systemd-pstate-at:
	@echo "  INSTALL  $(SYSTEMD_SERVICE_AT_INSTALL_TARGET)"
	@mkdir -p "$(shell dirname $(SYSTEMD_SERVICE_AT_INSTALL_TARGET))"
	@install -Dm 644 "$(SYSTEMD_SERVICE_AT_INSTALL_SRC)" "$(SYSTEMD_SERVICE_AT_INSTALL_TARGET)"

install-systemd-pstate-sleep:
	@echo "  INSTALL  $(SYSTEMD_SERVICE_SLEEP_INSTALL_TARGET)"
	@mkdir -p "$(shell dirname $(SYSTEMD_SERVICE_SLEEP_INSTALL_TARGET))"
	@install -Dm 644 "$(SYSTEMD_SERVICE_SLEEP_INSTALL_SRC)" "$(SYSTEMD_SERVICE_SLEEP_INSTALL_TARGET)"

install-power-plans:
	@echo "  INSTALL  DIR: $(POWER_PLAN_INSTALL_TARGETDIR)"
	@install -d "$(POWER_PLAN_INSTALL_TARGETDIR)"
	@install -Dm 644 "$(POWER_PLAN_INSTALL_SRCDIR)/00-auto.plan" "$(POWER_PLAN_INSTALL_TARGETDIR)"
	@install -Dm 644 "$(POWER_PLAN_INSTALL_SRCDIR)/01-powersave.plan" "$(POWER_PLAN_INSTALL_TARGETDIR)"
	@install -Dm 644 "$(POWER_PLAN_INSTALL_SRCDIR)/02-balanced.plan" "$(POWER_PLAN_INSTALL_TARGETDIR)"
	@install -Dm 644 "$(POWER_PLAN_INSTALL_SRCDIR)/03-performance.plan" "$(POWER_PLAN_INSTALL_TARGETDIR)"
	@install -Dm 644 "$(POWER_PLAN_INSTALL_SRCDIR)/04-max.plan" "$(POWER_PLAN_INSTALL_TARGETDIR)"

uninstall:
	@echo "Uninstalling..."
	@$(MAKE) uninstall-script
	@$(MAKE) uninstall-res
ifeq ($(INCLUDE_DOC), 1)
	@$(MAKE) uninstall-doc
endif
ifeq ($(INCLUDE_LICENSE), 1)
	@$(MAKE) uninstall-license
endif

uninstall-script:
	@echo "  UNINSTALL  $(SCRIPT_INSTALL_TARGET)"
	@rm -f "$(SCRIPT_INSTALL_TARGET)"

uninstall-doc:
	@echo "  UNINSTALL  $(DOC_INSTALL_TARGET)"
	@rm -f "$(DOC_INSTALL_TARGET)"
	@rmdir --ignore-fail-on-non-empty "$(shell dirname $(DOC_INSTALL_TARGET))"

uninstall-license:
	@echo "  UNINSTALL  $(LICENSE_INSTALL_TARGET)"
	@rm -f "$(LICENSE_INSTALL_TARGET)"
	@rmdir --ignore-fail-on-non-empty "$(shell dirname $(LICENSE_INSTALL_TARGET))"

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
ifeq ($(INCLUDE_POWER_PLANS), 1)
	@$(MAKE) uninstall-power-plans
endif

uninstall-bash:
	@echo "  UNINSTALL  $(BASH_INSTALL_TARGET)"
	@rm -f "$(BASH_INSTALL_TARGET)"

uninstall-zsh:
	@echo "  UNINSTALL  $(ZSH_INSTALL_TARGET)"
	@rm -f "$(ZSH_INSTALL_TARGET)"

uninstall-udev:
	@echo "  UNINSTALL  $(UDEV_INSTALL_TARGET)"
	@rm -f "$(UDEV_INSTALL_TARGET)"

uninstall-systemd:
	@$(MAKE) uninstall-systemd-pstate
	@$(MAKE) uninstall-systemd-pstate-at
	@$(MAKE) uninstall-systemd-pstate-sleep

uninstall-systemd-pstate:
	@echo "  UNINSTALL  $(SYSTEMD_SERVICE_INSTALL_TARGET)"
	@rm -f "$(SYSTEMD_SERVICE_INSTALL_TARGET)"

uninstall-systemd-pstate-at:
	@echo "  UNINSTALL  $(SYSTEMD_SERVICE_AT_INSTALL_TARGET)"
	@rm -f "$(SYSTEMD_SERVICE_AT_INSTALL_TARGET)"

uninstall-systemd-pstate-sleep:
	@echo "  UNINSTALL  $(SYSTEMD_SERVICE_SLEEP_INSTALL_TARGET)"
	@rm -f "$(SYSTEMD_SERVICE_SLEEP_INSTALL_TARGET)"

uninstall-power-plans:
	@echo "  UNINSTALL  DIR: $(POWER_PLAN_INSTALL_TARGETDIR)"
	@rm -f "$(POWER_PLAN_INSTALL_TARGETDIR)/00-auto.plan"
	@rm -f "$(POWER_PLAN_INSTALL_TARGETDIR)/01-powersave.plan"
	@rm -f "$(POWER_PLAN_INSTALL_TARGETDIR)/02-balanced.plan"
	@rm -f "$(POWER_PLAN_INSTALL_TARGETDIR)/03-performance.plan"
	@rm -f "$(POWER_PLAN_INSTALL_TARGETDIR)/04-max.plan"
	@rmdir --ignore-fail-on-non-empty "$(POWER_PLAN_INSTALL_TARGETDIR)"

