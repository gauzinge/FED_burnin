Objs            = TkFECController.o 
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -w -Wall -pedantic -fPIC -Wcpp
#DevFlags                   = -D__CBCDAQ_DEV__

#FEC_SW_DIR=/opt/cactus/include/amc13

IncludeDirs     =  /opt/cactus/include ../ 
IncludePaths            = $(IncludeDirs:%=-I%)

LibraryDirs = /opt/cactus/lib ../ ../lib
LibraryPaths = $(LibraryDirs:%=-L%)

ExternalObjects = $(LibraryPaths) 

ifneq ("$(wildcard $(FEC_SW_DIR))","")
	FECSWINSTALLED = 'Building TkFEC related Objects'
else
	FECSWINSTALLED = 'This feature can only be built if the uTCA compliant FED SW libraries are installed! ' 
endif

.PHONY: clean print

%.o: %.cc %.h
	$(CXX) -std=c++0x  $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) $(ExternalObjects) -c -o $@ $<

all:print $(Objs) 
	$(CC) -std=c++0x -shared -o libPh1_TkFEC.so $(Objs)
	mv libPh1_TkFEC.so ../lib

print:
	@echo '****************************************'
	@echo $(FECSWINSTALLED)
	@echo '****************************************'

clean:
	rm -f *.o

