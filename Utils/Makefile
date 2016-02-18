Objs                    = Exception.o Utilities.o argvparser.o Data.o
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -w -Wall -pedantic -fPIC -Wcpp
#DevFlags                   = -D__CBCDAQ_DEV__
DevFlags		=

IncludeDirs     =  /opt/cactus/include ../

IncludePaths            = $(IncludeDirs:%=-I%)

%.o: %.cc %.h 
	$(CXX) -std=c++11  $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<

.PHONY: clean print 

all:print $(Objs) 
	$(CC) -std=c++11 -shared -o libPh1_Utils.so $(Objs) -pthread
	mv libPh1_Utils.so ../lib

print:
	@echo '****************************************'
	@echo 'Building Utils'
	@echo '****************************************'

clean:
	rm -f *.o
