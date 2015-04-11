all: pstate-frequency

pstate-frequency:
	$(MAKE) -C project

clean:
	$(MAKE) -C project clean

install:
	$(MAKE) -C project install

uninstall:
	$(MAKE) -C project uninstall

options:
	$(MAKE) -C project options
