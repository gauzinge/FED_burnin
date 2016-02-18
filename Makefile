#AMC13DIR=/opt/cactus/include/amc13

SUBDIRS= 

ifneq ("$(wildcard $(AMC13DIR))","")
	SUBDIRS += AMC13
	AMC13INSTALLED = yes
else
	AMC13INSTALLED = no
endif

ifneq ("$(wildcard $(FEC_SW_DIR))","")
	SUBDIRS += TkFEC
	FECSWINSTALLED = yes
else
	FECSWINSTALLED = no
endif

SUBDIRS += Utils HWDescription HWInterface System src

.PHONY: print subdirs $(SUBDIRS) clean

subdirs: print $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

print:
	@echo '*****************************'
	@echo 'Amc13 SW installed:' $(AMC13INSTALLED)
	@echo '*****************************'
	@echo 'FEC SW installed:' $(FECSWINSTALLED)
	@echo '*****************************'

System: HWDescription HWInterface Utils

src: System

clean:
	(cd System; make clean)
	(cd Utils; make clean)
	(cd HWInterface; make clean)
	(cd AMC13; make clean)
	(cd TkFEC; make clean)
	(cd HWDescription; make clean)
	(cd src; make clean)
	(rm -f lib/* bin/*)

