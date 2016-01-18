Objs            = SystemController.o
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -w -Wall -pedantic -fPIC -Wcpp
#DevFlags                   = -D__CBCDAQ_DEV__
DevFlags                =
AMC13Flags		= -D__AMC13__
IncludeDirs     =  /opt/cactus/include ../ ../lib
IncludePaths            = $(IncludeDirs:%=-I%)

LibraryDirs = /opt/cactus/include ../ ../lib
LibraryPaths = $(LibraryDirs:%=-L%)

#ExternalObjects = $(LibraryPaths) -lPh1_Interface -lPh1_Description
LIBPATH=../lib/libPh1_Amc13.so

#ifneq ("$(wildcard $(LIBPATH))","")
ExternalObjects += $(LibraryPaths) -lPh1_Interface -lPh1_Description -lPh1_Amc13
#@echo yes
#else
#@echo no
#endif

%.o: %.cc %.h
	$(CXX) -std=c++11  $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) $(ExternalObjects) -c -o $@ $<

all: $(Objs)
	$(CC) -std=c++11 -shared -o libPh1_System.so $(Objs)
	mv libPh1_System.so ../lib

clean:
	rm -f *.o

