include ../config.mk

EXEC_NAME=pstate-frequency
OBJECTS=$(patsubst src/%.cpp,obj/%.o,$(wildcard src/*.cpp))
TARGET=bin/$(EXEC_NAME)

.PHONY: all dirs clean install uninstall

all: $(TARGET)

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
	@install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin

uninstall:
	@echo "  UNINSTALL  $(DESTDIR)$(PREFIX)/bin/$(EXEC_NAME)"
	@rm -f $(DESTDIR)$(PREFIX)/bin/$(EXEC_NAME)
	@echo "DONE"

