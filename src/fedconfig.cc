 #include <ctime>
#include <cstring>
#include "uhal/uhal.hpp"
#include "../Utils/Utilities.h"
#include "../HWDescription/PixFED.h"
#include "../HWInterface/PixFEDInterface.h"
#include "../System/SystemController.h"
#include "../Utils/Data.h"
void mypause()
{
        std::cout << "Press [Enter] to read FIFOs ...";
            std::cin.get();

}
int main(int argc, char* argv[] )
{

    const char* cHWFile = argv[1];
    std::cout << "HW Description File: " << cHWFile << std::endl;

    uhal::setLogLevelTo(uhal::Debug());

    // instantiate System Controller
    SystemController cSystemController;

    // initialize map of settings so I can know the proper number of acquisitions and TBMs
    cSystemController.InitializeSettings(cHWFile, std::cout);

    // initialize HWdescription from XML, beware, settings have to be read first
    cSystemController.InitializeHw(cHWFile, std::cout);

    // configure the HW
    cSystemController.ConfigureHw(std::cout );
    auto cSetting = cSystemController.fSettingsMap.find("NAcq");
    int cNAcq = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 10;

    // get the board info of all boards and start the acquistion
    for (auto& cFED : cSystemController.fPixFEDVector)
    {
        cSystemController.fFEDInterface->getBoardInfo(cFED);
        cSystemController.fFEDInterface->findPhases(cFED, 0);
    }

    mypause();

    for (auto& cFED : cSystemController.fPixFEDVector)
    {
        cSystemController.fFEDInterface->readTransparentFIFO(cFED);
        cSystemController.fFEDInterface->readSpyFIFO(cFED);
    }
    exit(0);
}   
