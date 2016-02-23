Objs            = TkFECInterface.o TkFECController.o 
CC              = gcc
CXX             = g++
CCFlags         = -g -O1 -w -Wall -pedantic -fPIC -Wcpp
CRAZYFLAGS=-g -Wall -DDEFAULTXMLFILENAME=\"Unknown\" -DDCUADDRESS=0x0   -DLASERDRIVER42 -DDCUTHREAD -DDCUFILTER -DI2C100 -DTRACKER  -DBT1003 -DLINUX -Wall -Werror -DCORETOOLS_VERSION=3.7.3 -Dx86 -DBUSUTCAFEC -DBUSVMECAENUSB -Dlinux -DLITTLE_ENDIAN__ 
#DevFlags                   = -D__CBCDAQ_DEV__

#FEC_SW_DIR=/opt/cactus/include/amc13

IncludeDirs     =  /opt/cactus/include ../ 

LibraryDirs = /opt/cactus/lib ../ ../lib
Libraries =

ifneq ("$(wildcard $(FEC_SW_DIR))","")
	FECSWINSTALLED = 'Building TkFEC related Objects'
	IncludeDirs += $(FEC_SW_DIR)/generic/include \
				   $(FEC_SW_DIR)/FecPciDeviceDriver/include \
				   ${XDAQ_ROOT}/include\
				   ${XDAQ_ROOT}/include/hal\
				   ../tools
	CCFLAGS += $(CRAZYFLAGS)
	LibraryDirs += $(XDAQ_ROOT)/lib \
				   $(FEC_SW_DIR)/lib
	Libraries += -lpthread -ldl -lxerces-c -lGenericHAL -lconfig -lxcept -lCAENLinuxBusAdapter -lCAENVME -lDeviceAccess -ltoolbox -llog4cplus -lasyncresolv -luuid 
	#-lcactus_extern_pugixml -lcactus_uhal_log -lcactus_uhal_grammars -lcactus_uhal_uhal
	Objs += ServerAccess.o 
else
	FECSWINSTALLED = 'This feature can only be built if the uTCA compliant FED SW libraries are installed! ' 
endif
IncludePaths = $(IncludeDirs:%=-I%)

LibraryPaths = $(LibraryDirs:%=-L%)

ExternalObjects = $(LibraryPaths) 

.PHONY: clean print

%.o: %.cc %.h
	$(CXX) -std=c++0x  $(DevFlags) $(CCFlags) $(UserCCFlags) $(CCDefines) $(IncludePaths) $(ExternalObjects) $(Libraries) -c -o $@ $<

all:print $(Objs) 
	$(CC) -std=c++0x -shared -o libPh1_TkFEC.so $(Objs)
	mv libPh1_TkFEC.so ../lib

#ccu: ccu.cc ServerAccess.o ../lib/libSockets.so
#$(CC) ccu.cc -std=c++0x ServerAccess.o -o ccu $(IncludePaths) $(LibraryPaths) $(Libraries) -lSockets

print:
	@echo '****************************************'
	@echo $(FECSWINSTALLED)
	@echo '****************************************'

clean:
	rm -f *.o

#INCLUDES=-I ../\
	#-I $(FEC_SW_DIR)/generic/include \
	#-I $(FEC_SW_DIR)/FecPciDeviceDriver/include \
	#-I ${XDAQ_ROOT}/include\
	#-I ${XDAQ_ROOT}/include/hal\
	#-I ../tools/include\
	#-I ${CACTUSINCLUDE} 
#         -I $(FEC_SW_DIR)/ThirdParty/APIConsoleDebugger/include \


#LIBDIRS=\ -L$(XDAQ_ROOT)/lib \
	#-L$(FEC_SW_DIR)/lib \
	#-L$(CACTUSLIB)

#Libraries = -lpthread -ldl -lxerces-c -lGenericHAL -lconfig -lxcept -lCAENLinuxBusAdapter -lCAENVME -lDeviceAccess -ltoolbox -llog4cplus -lasyncresolv -luuid -lcactus_extern_pugixml -lcactus_uhal_log -lcactus_uhal_grammars -lcactus_uhal_uhal





