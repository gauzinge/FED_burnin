#include <cstring>
#include "uhal/uhal.hpp"
#include "../Utils/Utilities.h"
#include "../HWDescription/PixFED.h"
#include "../HWInterface/PixFEDInterface.h"
#include "../System/SystemController.h"


int main(int argc, char* argv[] )
{
    uhal::setLogLevelTo(uhal::Debug());

    // instantiate System Controller
    SystemController cSystemController;

    // initialize map of settings so I can know the proper number of acquisitions and TBMs
    cSystemController.InitializeSettings("settings/HWDescription.xml", std::cout);

    // initialize HWdescription from XML, beware, settings have to be read first
    cSystemController.InitializeHw("settings/HWDescription.xml", std::cout);

    // configure the HW
    cSystemController.ConfigureHw(std::cout );
    auto cSetting = cSystemController.fSettingsMap.find("NAcq");
    uint32_t cNAcq = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 10;

    // get the board info of all boards and start the acquistion
    for (auto& cFED : cSystemController.fPixFEDVector)
    {
        cSystemController.fFEDInterface->getBoardInfo(cFED);
        cSystemController.fFEDInterface->Start(cFED);
    }

    // loop over the number of acquisitions
    uint32_t iAcq = 0;
    while ( iAcq < cNAcq)
    {
        std::cout << std::endl << BOLDRED << "Acquisition: " << iAcq << RESET << std::endl;
        for (auto& cFED : cSystemController.fPixFEDVector)
        {
            std::cout << BOLDGREEN << "Data for FED " << +cFED->getBeId() << RESET << std::endl;
            cSystemController.fFEDInterface->ReadData(cFED);

        }
        iAcq++;
    }
    for (auto& cFED : cSystemController.fPixFEDVector)
    {
        cSystemController.fFEDInterface->Stop(cFED);
        std::cout << "Finished reading Data!" << std::endl;
    }
}
