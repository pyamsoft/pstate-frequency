include ../config.mk

PROJECT_ROOT=project
EXEC_NAME=pstate-frequency
UDEV=99-$(EXEC_NAME).rules
SYSTEMD=$(EXEC_NAME).service
SYSTEMD_SLEEP=$(EXEC_NAME)-sleep.service
SYSTEM_PREFIX=/usr

.PHONY: install uninstall

install:
ifeq ($(INCLUDE_BASH_COMPLETION), 1)
	@echo -n "  INSTALL  "
	@echo "$(DESTDIR)$(SYSTEM_PREFIX)/share/bash-completion/completions/$(EXEC_NAME)"
	@install -d $(DESTDIR)$(SYSTEM_PREFIX)/share/bash-completion/completions
	@install -m 644 shell/bash/bash_completion \
		$(DESTDIR)$(SYSTEM_PREFIX)/share/bash-completion/completions/$(EXEC_NAME)
endif
ifeq ($(INCLUDE_ZSH_COMPLETION), 1)
	@echo -n "  INSTALL  "
	@echo "$(DESTDIR)$(SYSTEM_PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)"
	@install -d $(DESTDIR)$(SYSTEM_PREFIX)/share/zsh/site-functions
	@install -m 644 shell/zsh/zsh_completion \
		$(DESTDIR)$(SYSTEM_PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)
endif
ifeq ($(INCLUDE_UDEV_RULE), 1)
	@echo "  INSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/lib/udev/rules.d/$(UDEV)"
	@install -d $(DESTDIR)$(SYSTEM_PREFIX)/lib/udev/rules.d
	@install -m 644 udev/$(UDEV) \
		$(DESTDIR)$(SYSTEM_PREFIX)/lib/udev/rules.d/$(UDEV)
	@sed -i "s#$(EXEC_NAME)#$(PREFIX)/bin/$(EXEC_NAME)#g" \
		$(DESTDIR)$(SYSTEM_PREFIX)/lib/udev/rules.d/$(UDEV)
endif
ifeq ($(INCLUDE_SYSTEMD_UNIT), 1)
	@echo "  INSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/lib/systemd/system/$(SYSTEMD)"
	@install -d $(DESTDIR)$(SYSTEM_PREFIX)/lib/systemd/system
	@install -m 644 systemd/$(SYSTEMD) \
		$(DESTDIR)$(SYSTEM_PREFIX)/lib/systemd/system/$(SYSTEMD)
	@sed -i "s#$(EXEC_NAME)#$(PREFIX)/bin/$(EXEC_NAME)#g" \
		$(DESTDIR)$(SYSTEM_PREFIX)/lib/systemd/system/$(SYSTEMD)
	@echo "  INSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/lib/systemd/system/$(SYSTEMD_SLEEP)"
	@install -m 644 systemd/$(SYSTEMD_SLEEP) \
		$(DESTDIR)$(SYSTEM_PREFIX)/lib/systemd/system/$(SYSTEMD_SLEEP)
	@sed -i "s#$(EXEC_NAME)#$(PREFIX)/bin/$(EXEC_NAME)#g" \
		$(DESTDIR)$(SYSTEM_PREFIX)/lib/systemd/system/$(SYSTEMD_SLEEP)
endif

uninstall:
ifeq ($(INCLUDE_BASH_COMPLETION), 1)
	@echo -n "  UNINSTALL  "
	@echo "$(DESTDIR)$(SYSTEM_PREFIX)/share/bash-completion/completions/$(EXEC_NAME)"
	@rm -f $(DESTDIR)$(SYSTEM_PREFIX)/share/bash-completion/completions/$(EXEC_NAME)
endif
ifeq ($(INCLUDE_ZSH_COMPLETION), 1)
	@echo -n "  UNINSTALL  "
	@echo "$(DESTDIR)$(SYSTEM_PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)"
	@rm -f $(DESTDIR)$(SYSTEM_PREFIX)/share/zsh/site-functions/_$(EXEC_NAME)
endif
ifeq ($(INCLUDE_UDEV_RULE), 1)
	@echo "  UNINSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/lib/udev/rules.d/$(UDEV)"
	@rm -f $(DESTDIR)$(SYSTEM_PREFIX)/lib/udev/rules.d/$(UDEV)
endif
ifeq ($(INCLUDE_SYSTEMD_UNIT), 1)
	@echo "  UNINSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/lib/systemd/system/$(SYSTEMD)"
	@rm -f $(DESTDIR)$(SYSTEM_PREFIX)/lib/systemd/system/$(SYSTEMD)
	@echo "  UNINSTALL  $(DESTDIR)$(SYSTEM_PREFIX)/lib/systemd/system/$(SYSTEMD_SLEEP)"
	@rm -f $(DESTDIR)$(SYSTEM_PREFIX)/lib/systemd/system/$(SYSTEMD_SLEEP)
endif

