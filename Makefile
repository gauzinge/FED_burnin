#AMC13Flag = -D__AMC13__
#ExternalObjects = 
AMC13DIR=/opt/cactus/include/amc13

all: Utils HWDescription HWInterface System src
amc13: AMC13 Utils HWDescription HWInterface System src

#ifneq ("$(wildcard $(AMC13DIR))","")
#all: $(binariesAMC13)
#@echo yup
#else
#@echo nope
#all: $(binaries)
#endif

HWDescription::
	$(MAKE) -C $@
Utils::
	$(MAKE) -C $@
HWInterface::
	$(MAKE) -C $@
AMC13::
	$(MAKE) -C $@
System::
	$(MAKE) -C $@ 
src::
	$(MAKE) -C $@ 

clean:
	(cd System; make clean)
	(cd Utils; make clean)
	(cd HWInterface; make clean)
	(cd AMC13; make clean)
	(cd HWDescription; make clean)
	(cd src; make clean)
	(rm -f lib/* bin/*)

