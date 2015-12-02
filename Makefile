all: Utils HWDescription HWInterface System src

HWDescription::
	$(MAKE) -C $@
Utils::
	$(MAKE) -C $@
HWInterface::
	$(MAKE) -C $@
System::
	$(MAKE) -C $@
src::
	$(MAKE) -C $@

clean:
	(cd System; make clean)
	(cd Utils; make clean)
	(cd HWInterface; make clean)
	(cd HWDescription; make clean)
	(cd src; make clean)
	(rm -f lib/* bin/*)

