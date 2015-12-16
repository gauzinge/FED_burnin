Objs            = RegManager.o PixFEDFWInterface.o FpgaConfig.o CtaFpgaConfig.o Firmware.o PixFEDInterface.o MmcPipeInterface.o
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -w -Wall -pedantic -fPIC  -Wcpp
#DevFlags                   = -D__CBCDAQ_DEV__
DevFlags	=

IncludeDirs     =  /opt/cactus/include ../ .

IncludePaths            = $(IncludeDirs:%=-I%)

%.o: %.cc %.h
	$(CXX) -std=c++11  $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<

all: $(Objs) 
	$(CC) -std=c++11 -shared -o libPh1_Interface.so $(Objs) -pthread
	mv libPh1_Interface.so ../lib

clean:
	rm -f *.o
