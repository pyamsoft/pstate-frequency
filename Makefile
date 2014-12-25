include config.mk

EXEC_NAME=pstate-frequency
SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst src/%.c,obj/%.o,$(SOURCES))
UDEV=10-pstate-frequency.rules
TARGET=bin/$(EXEC_NAME)

# The Target Build
all: options $(TARGET)

options:
	@echo "CFLAGS  = " $(CFLAGS)
	@echo "LDFLAGS = " $(LDFLAGS)
	@echo "CC      = " $(CC)

$(OBJECTS): | obj

obj:
	@mkdir -p $@

obj/%.o : src/%.c
	@echo "  CC  $@"
	@$(CC) -c $< -o $@ $(CFLAGS)

$(TARGET): dirs $(OBJECTS)
	@echo "  LD  $@"
	@$(CC) -o $@ $(OBJECTS) $(LDFLAGS) 

dirs:
	@mkdir -p obj
	@mkdir -p bin

clean:
	rm -rf $(TARGET) $(OBJECTS) bin obj

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
	@sed -i "s#pstate-frequency#$(PREFIX)/bin/pstate-frequency#g" $(DESTDIR)/$(ETCDIR)/udev/rules.d/$(UDEV)
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

