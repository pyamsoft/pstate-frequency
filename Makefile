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
	@echo $(CC) -c $< -o $@
	@$(CC) -c $< -o $@ $(CFLAGS)

$(TARGET): dirs $(OBJECTS)
	@echo $(CC) -o $@
	@$(CC) -o $@ $(OBJECTS) $(LDFLAGS) 

dirs:
	@mkdir -p obj
	@mkdir -p bin

clean:
	rm -rf $(TARGET) $(OBJECTS) bin obj

install: all
	@echo "pstate-frequency will be installed to " $(DESTDIR)/$(PREFIX)/$(TARGET)
	@install -dm 755 $(DESTDIR)/$(PREFIX)/bin
	@install -m $(BIN_PERMISSION) $(TARGET) $(DESTDIR)/$(PREFIX)/bin
ifeq ($(INCLUDE_BASH_COMPLETION), 1)
		@echo "bash completion will be installed to " $(DESTDIR)/etc/bash_completion/$(EXEC_NAME)
		@install -dm 755 $(DESTDIR)/etc/bash_completion.d
		@install -m 644 assets/bash/bash_completion $(DESTDIR)/etc/bash_completion.d/$(EXEC_NAME)
endif
ifeq ($(INCLUDE_UDEV_RULE), 1)
		@echo "udev rule will be installed to " $(DESTDIR)/etc/udev/rules.d/$(UDEV)
		@install -dm 755 $(DESTDIR)/etc/udev/rules.d
		@install -m 644 assets/udev/$(UDEV) $(DESTDIR)/etc/udev/rules.d/$(UDEV)
endif

uninstall:
	@echo "Removing udev rule file."
	@rm -f $(DESTDIR)/etc/udev/rules.d/$(UDEV)
	@echo "Removing bash completion file."
	@rm -f $(DESTDIR)/etc/bash_completion.d/$(EXEC_NAME)
	@echo "Removing binary."
	@rm -f $(DESTDIR)/$(PREFIX)/bin/$(EXEC_NAME)
	@echo "Done"

