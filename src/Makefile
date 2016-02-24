C             = gcc
CXX           = g++
CCFlags       = -g -O1 -w -Wall -pedantic -std=c++11 -fPIC
CCFlagsRoot   = `root-config --cflags --evelibs --glibs`
CRAZYFLAGS    = -g -Wall -DDEFAULTXMLFILENAME=\"Unknown\" -DDCUADDRESS=0x0   -DLASERDRIVER42 -DDCUTHREAD -DDCUFILTER -DI2C100 -DTRACKER  -DBT1003 -DLINUX -Wall -Werror -DCORETOOLS_VERSION=3.7.3 -Dx86 -DBUSUTCAFEC -DBUSVMECAENUSB -Dlinux -DLITTLE_ENDIAN__ 
Amc13Flag     = -D__AMC13__
DevFlags      =

IncludeDirs   =  /opt/cactus/include ../

LibraryDirs   = /opt/cactus/lib ../lib

ExternalObjects= $(LibraryPaths) -lpthread  -lcactus_extern_pugixml -lcactus_uhal_log -lcactus_uhal_grammars -lcactus_uhal_uhal -lboost_system -lPh1_Interface -lPh1_Description -lPh1_System -lPh1_Utils
binaries       = print run fpgaconfig fedconfig

ifneq ("$(wildcard $(AMC13DIR))","")
	ExternalObjects += -lcactus_amc13_amc13 -lPh1_Amc13
	AMC13INSTALLED = yes
	binaries += amc13config 
else
	AMC13INSTALLED = no
endif

ifneq ("$(wildcard $(FEC_SW_DIR))","")
	ExternalObjects += -lpthread -ldl -lxerces-c -lGenericHAL -lconfig -lxcept -lCAENLinuxBusAdapter -lCAENVME -lDeviceAccess -ltoolbox -llog4cplus -lasyncresolv -luuid -lPh1_TkFEC -lSockets
	FECSWINSTALLED = yes
	binaries += tkfecconfig 
	IncludeDirs += $(FEC_SW_DIR)/generic/include \
				   $(FEC_SW_DIR)/FecPciDeviceDriver/include \
				   ${XDAQ_ROOT}/include\
				   ${XDAQ_ROOT}/include/hal\
				   ../tools
	CCFLAGS += $(CRAZYFLAGS)
	LibraryDirs += $(XDAQ_ROOT)/lib \
				   $(FEC_SW_DIR)/lib
	else
	FECSWINSTALLED = no
endif

.PHONY: print clean $(binaries) 


LibraryPaths  = $(LibraryDirs:%=-L%)
IncludePaths  = $(IncludeDirs:%=-I%)

all: $(binaries)


print:
	@echo '*****************************'
	@echo 'Amc13 SW installed:' $(AMC13INSTALLED)
	@echo '*****************************'
	@echo '*****************************'
	@echo 'FEC SW installed:' $(FECSWINSTALLED)
	@echo '*****************************'

run: acquisition.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) 
	cp $@ ../bin

fpgaconfig: fpgaconfig.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) 
	cp $@ ../bin

amc13config: amc13test.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) 
	cp $@ ../bin

fedconfig: fedconfig.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) 
	cp $@ ../bin

tkfecconfig: tkfectest.cc
	$(CXX) -std=c++11 $(CCFlags) -o $@ $< $(IncludePaths) $(ExternalObjects) 
	cp $@ ../bin

clean:
	rm -f $(binaries) *.o
