AMC13DIR=/opt/cactus/include/amc13

ifneq ("$(wildcard $(AMC13DIR))","")
	SUBDIRS = AMC13 Utils HWDescription HWInterface System src
	AMC13INSTALLED = yes
else
	SUBDIRS = Utils HWDescription HWInterface System src
	AMC13INSTALLED = no
endif

.PHONY: print subdirs $(SUBDIRS) clean

subdirs: print $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) -C $@

print:
	@echo '*****************************'
	@echo 'Amc13 SW installed:' $(AMC13INSTALLED)
	@echo '*****************************'

System: HWDescription HWInterface Utils

src: System

clean:
	(cd System; make clean)
	(cd Utils; make clean)
	(cd HWInterface; make clean)
	(cd AMC13; make clean)
	(cd HWDescription; make clean)
	(cd src; make clean)
	(rm -f lib/* bin/*)

