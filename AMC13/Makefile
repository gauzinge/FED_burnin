Objs            = Amc13Description.o Amc13Interface.o Amc13Controller.o
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -w -Wall -pedantic -fPIC -Wcpp
#DevFlags                   = -D__CBCDAQ_DEV__
DevFlags                =

IncludeDirs     =  /opt/cactus/include ../ ../lib
IncludePaths            = $(IncludeDirs:%=-I%)

LibraryDirs = /opt/cactus/lib ../ ../lib
LibraryPaths = $(LibraryDirs:%=-L%)

ExternalObjects = $(LibraryPaths) -lcactus_amc13_amc13 

.PHONY: clean

%.o: %.cc %.h
	$(CXX) -std=c++11  $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) $(ExternalObjects) -c -o $@ $<

all: $(Objs)
	$(CC) -std=c++11 -shared -o libPh1_Amc13.so $(Objs)
	mv libPh1_Amc13.so ../lib

clean:
	rm -f *.o

