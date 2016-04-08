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
SYSTEMD_SERVICE_SLEEP_INSTALL_SRC="$(RES_DIR)/systemd/pstate-frequency-sleep.service"

SCRIPT_INSTALL_TARGET="$(DESTDIR)/$(PREFIX)/bin/$(NAME)"
DOC_INSTALL_TARGET="$(DESTDIR)/$(PREFIX)/share/doc/$(NAME)/README.md"
LICENSE_INSTALL_TARGET="$(DESTDIR)/$(PREFIX)/share/doc/$(NAME)/LICENSE"
BASH_INSTALL_TARGET="$(DESTDIR)/$(SYSTEM_PREFIX)/share/bash-completion/completions/$(NAME)"
ZSH_INSTALL_TARGET="$(DESTDIR)/$(SYSTEM_PREFIX)/share/zsh/site-functions/_$(NAME)"
UDEV_INSTALL_TARGET="$(DESTDIR)/$(SYSTEM_PREFIX)/lib/udev/rules.d/99-$(NAME).rules"
SYSTEMD_SERVICE_INSTALL_TARGET="$(DESTDIR)/$(SYSTEM_PREFIX)/lib/systemd/system/$(NAME).service"
SYSTEMD_SERVICE_SLEEP_INSTALL_TARGET="$(DESTDIR)/$(SYSTEM_PREFIX)/lib/systemd/system/$(NAME)-sleep.service"

.PHONY: all install uninstall edit \
	install-doc install-license install-script install-res \
	install-bash install-zsh install-udev install-systemd \
	install-systemd-pstate install-systemd-pstate-sleep \
	uninstall-script uninstall-doc uninstall-license uninstall-res \
	uninstall-bash uninstall-zsh uninstall-udev uninstall-systemd \
	uninstall-systemd-pstate uninstall-systemd-pstate-sleep

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
	@$(MAKE) install-systemd-pstate-sleep

install-systemd-pstate:
	@echo "  INSTALL  $(SYSTEMD_SERVICE_INSTALL_TARGET)"
	@mkdir -p "$(shell dirname $(SYSTEMD_SERVICE_INSTALL_TARGET))"
	@install -Dm 644 "$(SYSTEMD_SERVICE_INSTALL_SRC)" "$(SYSTEMD_SERVICE_INSTALL_TARGET)"

install-systemd-pstate-sleep:
	@echo "  INSTALL  $(SYSTEMD_SERVICE_SLEEP_INSTALL_TARGET)"
	@mkdir -p "$(shell dirname $(SYSTEMD_SERVICE_SLEEP_INSTALL_TARGET))"
	@install -Dm 644 "$(SYSTEMD_SERVICE_SLEEP_INSTALL_SRC)" "$(SYSTEMD_SERVICE_SLEEP_INSTALL_TARGET)"

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
	@$(MAKE) uninstall-systemd-pstate-sleep

uninstall-systemd-pstate:
	@echo "  UNINSTALL  $(SYSTEMD_SERVICE_INSTALL_TARGET)"
	@rm -f "$(SYSTEMD_SERVICE_INSTALL_TARGET)"

uninstall-systemd-pstate-sleep:
	@echo "  UNINSTALL  $(SYSTEMD_SERVICE_SLEEP_INSTALL_TARGET)"
	@rm -f "$(SYSTEMD_SERVICE_SLEEP_INSTALL_TARGET)"

