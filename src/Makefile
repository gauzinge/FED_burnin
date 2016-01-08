C              = gcc
CXX             = g++
CCFlags         = -g -O1 -w -Wall -pedantic -std=c++11 -fPIC
CCFlagsRoot	= `root-config --cflags --evelibs --glibs`
ROOTVERSION := $(shell root-config --has-http)
HttpFlag = -D__HTTP__

#DevFlags                    = -D__CBCDAQ_DEV__
DevFlags                   =

IncludeDirs     =  /opt/cactus/include ../

IncludePaths            = $(IncludeDirs:%=-I%)

LibraryDirs = /opt/cactus/lib ../lib
RootLibraryDirs = /usr/local/lib/root

LibraryPaths = $(LibraryDirs:%=-L%)
RootLibraryPaths = $(RootLibraryDirs:%=-L%)


ExternalObjects= $(LibraryPaths) -lpthread  -lcactus_extern_pugixml -lcactus_uhal_log -lcactus_uhal_grammars -lcactus_uhal_uhal \
				 -lboost_system -lPh1_Interface -lPh1_Description -lPh1_System -lPh1_Utils

binaries=print run clt fpgaconfig

all: clean $(binaries)

rootflags:
	$(eval CCFlags += $(CCFlagsRoot))
	$(eval ExternalObjects += $(ExtObjectsRoot))

noroot: clean

print:
	@echo 'Root Has Http:' $(ROOTVERSION)

run: acquisition.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

clt: commandlinetool.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin

fpgaconfig: fpgaconfig.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects)
	cp $@ ../bin
clean:
	rm -f $(binaries) *.o
