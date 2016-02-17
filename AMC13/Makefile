Objs            = Amc13Description.o Amc13Interface.o Amc13Controller.o
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -w -Wall -pedantic -fPIC -Wcpp
#DevFlags                   = -D__CBCDAQ_DEV__

AMC13DIR=/opt/cactus/include/amc13

IncludeDirs     =  /opt/cactus/include ../ ../lib
IncludePaths            = $(IncludeDirs:%=-I%)

LibraryDirs = /opt/cactus/lib ../ ../lib
LibraryPaths = $(LibraryDirs:%=-L%)

ExternalObjects = $(LibraryPaths) -lcactus_amc13_amc13 

ifneq ("$(wildcard $(AMC13DIR))","")
	AMC13INSTALLED = 'Building AMC13 related Objects'
else
	AMC13INSTALLED = 'This feature can only be built if the AMC13 libraries are installed with cactus!' 
endif

.PHONY: clean print

%.o: %.cc %.h
	$(CXX) -std=c++0x  $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) $(ExternalObjects) -c -o $@ $<

all:print $(Objs) 
	$(CC) -std=c++0x -shared -o libPh1_Amc13.so $(Objs)
	mv libPh1_Amc13.so ../lib

print:
	@echo '****************************************'
	@echo $(AMC13INSTALLED)
	@echo '****************************************'

clean:
	rm -f *.o

