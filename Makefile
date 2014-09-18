include config.mk
PREFIX?=/usr/local
ETCDIR?=/etc
EXEC_NAME=pstate-frequency
SOURCES=$(wildcard src/**/*.c src/*.c)
OBJECTS=$(patsubst src/%.c,obj/%.o,$(SOURCES))
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
	@install -dm 755 $(DESTDIR)/$(PREFIX)/bin
	@install -m $(BIN_PERMISSION) $(TARGET) $(DESTDIR)/$(PREFIX)/bin
	@install -dm 755 $(DESTDIR)/$(ETCDIR)/bash_completion.d
	@install -m 644 bash_completion $(DESTDIR)/$(ETCDIR)/bash_completion.d/$(EXEC_NAME)

