CC=		g++
FLAGS=		 -g -Wall -I./include -fPIC -Wno-write-strings
#FLAGS=		 -g -Wall -DDEBUG -I./include 
LDLAGS= -m32 
SOFLAGS= -shared
OBJECTDIR=objects
SRCDIR=src
INCLUDEDIR=include
PIXELLIB=$(BASE_DIR)/lib

OBJECTS=Getline.o MultiplexingServer.o SimpleCommand.o SimpleConnection.o VMELock.o 

.PHONY: print clean

all: print lib 

print:
	@echo '*****************************'
	@echo 'Building (BPixel) auxillary tools' 
	@echo '*****************************'

lib:	$(OBJECTS)
	$(CC) -shared -o libSockets.so $(LDFLAGS) $(OBJECTS)
	rm -f $(PIXELLIB)/libSockets.so
	mv libSockets.so  ../lib/libSockets.so

#%.o: %.cc %.h 
#	$(CXX) -std=c++11  $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) -c -o $@ $<

Getline.o:		
	$(CC) $(FLAGS) -c Getline.c -o Getline.o

VMELock.o:		VMELock.cc
	$(CC) $(FLAGS) -c VMELock.cc -o VMELock.o

MultiplexingServer.o: MultiplexingServer.cc MultiplexingServer.h SimpleCommand.o SimpleConnection.o Getline.o
	$(CC) $(FLAGS) -c MultiplexingServer.cc -o MultiplexingServer.o	

SimpleCommand.o:	SimpleCommand.cc SimpleCommand.h
	$(CC) $(FLAGS) -c SimpleCommand.cc -o SimpleCommand.o

SimpleConnection.o:	SimpleConnection.cc SimpleConnection.h
	$(CC) $(FLAGS) -c SimpleConnection.cc -o SimpleConnection.o


clean:
	rm -f $(OBJECTS); rm -f lib/libSockets.so

