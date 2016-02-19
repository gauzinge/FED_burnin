C             = gcc
CXX           = g++
CCFlags       = -g -O1 -w -Wall -pedantic -std=c++11 -fPIC
CCFlagsRoot   = `root-config --cflags --evelibs --glibs`
Amc13Flag     = -D__AMC13__
DevFlags      =

#AMC13DIR=/opt/cactus/include/amc13
IncludeDirs   =  /opt/cactus/include ../
IncludePaths  = $(IncludeDirs:%=-I%)

LibraryDirs   = /opt/cactus/lib ../lib
LibraryPaths  = $(LibraryDirs:%=-L%)

ExternalObjects= $(LibraryPaths) -lpthread  -lcactus_extern_pugixml -lcactus_uhal_log -lcactus_uhal_grammars -lcactus_uhal_uhal -lboost_system -lPh1_Interface -lPh1_Description -lPh1_System -lPh1_Utils
binaries       = print run fpgaconfig fedconfig

ifneq ("$(wildcard $(AMC13DIR))","")
	ExternalObjects += -lcactus_amc13_amc13 -lPh1_Amc13
	AMC13INSTALLED = yes
	binaries += amc13config 
else
	AMC13INSTALLED = no
endif

ifneq ("$(wildcard $(FEC_SW_DIR))","")
	ExternalObjects += -lPh1_TkFEC
	FECSWINSTALLED = yes
	binaries += tkfecconfig 
else
	FECSWINSTALLED = no
endif

.PHONY: print clean $(binaries) 
	#run fpgaconfig amc13test

#binaries=print run fpgaconfig amc13test fedconfig


all: $(binaries)


print:
	@echo '*****************************'
	@echo 'Amc13 SW installed:' $(AMC13INSTALLED)
	@echo '*****************************'
	@echo '*****************************'
	@echo 'FEC SW installed:' $(FECSWINSTALLED)
	@echo '*****************************'

run: acquisition.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

fpgaconfig: fpgaconfig.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

amc13config: amc13test.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

fedconfig: fedconfig.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

tkfecconfig: tkfectest.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

clean:
	rm -f $(binaries) *.o
