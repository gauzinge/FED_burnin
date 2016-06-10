C             = gcc
CXX           = g++
CCFlags       = -g -O1 -w -Wall -pedantic -std=c++11 -fPIC
CCFlagsRoot   = `root-config --cflags --evelibs --glibs`
Amc13Flag     = -D__AMC13__
DevFlags      =

AMC13DIR=/opt/cactus/include/amc13
IncludeDirs   =  /opt/cactus/include ../
IncludePaths  = $(IncludeDirs:%=-I%)

LibraryDirs   = /opt/cactus/lib ../lib
LibraryPaths  = $(LibraryDirs:%=-L%)

ifneq ("$(wildcard $(AMC13DIR))","")
	ExternalObjects= $(LibraryPaths) -lpthread  -lcactus_extern_pugixml -lcactus_uhal_log -lcactus_uhal_grammars -lcactus_uhal_uhal -lboost_system -lcactus_amc13_amc13 -lPh1_Interface -lPh1_Description -lPh1_System -lPh1_Utils -lPh1_Amc13
	AMC13INSTALLED = yes
	binaries       = print run fpgaconfig amc13test fedconfig
else
	ExternalObjects= $(LibraryPaths) -lpthread  -lcactus_extern_pugixml -lcactus_uhal_log -lcactus_uhal_grammars -lcactus_uhal_uhal -lboost_system -lPh1_Interface -lPh1_Description -lPh1_System -lPh1_Utils
	AMC13INSTALLED = no
	binaries       = print run fpgaconfig fedconfig
endif

.PHONY: clean $(binaries) 
	#run fpgaconfig amc13test

binaries=print run fpgaconfig amc13test fedconfig feroltest clt rssiread


all: $(binaries)

print:
	@echo '*****************************'
	@echo 'Amc13 SW installed:' $(AMC13INSTALLED)
	@echo '*****************************'

run: acquisition.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

clt: commandlinetool.cc commandlinetool.hh
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

amc13test: amc13test.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

fedconfig: fedconfig.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

feroltest: feroltest.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

fpgaconfig: fpgaconfig.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

rssiread: rssiread.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin
clean:
	rm -f $(binaries) *.o
