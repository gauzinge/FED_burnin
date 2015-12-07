Objs                    = Fitel.o PixFED.o
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -Wall -fPIC
# -fPIC `root-config --cflags --glibs --evelibs`
#DevFlags                   = -D__CBCDAQ_DEV__
DevFlags               =

IncludeDirs     =  /opt/cactus/include ../

IncludePaths            = $(IncludeDirs:%=-I%)

%.o: %.cc %.h
	$(CXX) -std=c++11 $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<


all: $(Objs)
	$(CC) -shared -o libPh1_Description.so $(Objs)
	mv libPh1_Description.so ../lib

clean:
	rm -f *.o
