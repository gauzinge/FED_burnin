
#include <ctime>
#include <cstring>
#include "uhal/uhal.hpp"
#include "../Utils/Utilities.h"
#include "../HWDescription/PixFED.h"
#include "../HWInterface/PixFEDInterface.h"
#include "../System/SystemController.h"
#include "../AMC13/Amc13Controller.h"
#include "../Utils/Data.h"
void mypause()
{
    std::cout << "Press [Enter] to read FIFOs ...";
    std::cin.get();

}
int main (int argc, char* argv[] )
{

    const char* cHWFile = argv[1];
    std::cout << "HW Description File: " << cHWFile << std::endl;

    uhal::setLogLevelTo (uhal::Debug() );

    // instantiate System Controller
    SystemController cSystemController;

    // initialize map of settings so I can know the proper number of acquisitions and TBMs
    cSystemController.InitializeSettings (cHWFile, std::cout);

    // initialize HWdescription from XML, beware, settings have to be read first
    cSystemController.InitializeHw (cHWFile, std::cout);

    //auto cSetting = cSystemController.fSettingsMap.find ("NAcq");
    //int cNAcq = (cSetting != std::end (cSystemController.fSettingsMap) ) ? cSetting->second : 10;
    //cSetting = cSystemController.fSettingsMap.find ("BlockSize");
    //int cBlockSize = (cSetting != std::end (cSystemController.fSettingsMap) ) ? cSetting->second : 2;

    auto cSetting = cSystemController.fSettingsMap.find ("ChannelOfInterest");
    int cChannelOfInterest = (cSetting != std::end (cSystemController.fSettingsMap) ) ? cSetting->second : 0;


    //configure FED & FITELS & SFP+
    cSystemController.ConfigureHw (std::cout );

    // get the board info of all boards and start the acquistion

    for (auto& cFED : cSystemController.fPixFEDVector)
    {
        for (auto& cFitel : cFED->fFitelVector)
            for (uint32_t cChannel = 0; cChannel < 12; cChannel++)
                cSystemController.fFEDInterface->ReadADC (cFitel, cChannel, true);

        cSystemController.fFEDInterface->getBoardInfo (cFED);

        cSystemController.fFEDInterface->findPhases (cFED, cChannelOfInterest);
    }

    exit (0);
}
