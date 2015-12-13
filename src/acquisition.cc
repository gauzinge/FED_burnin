#include <cstring>
#include "uhal/uhal.hpp"
#include "../Utils/Utilities.h"
#include "../HWDescription/PixFED.h"
#include "../HWInterface/PixFEDInterface.h"
#include "../System/SystemController.h"
#include "../Utils/Data.h"

int main(int argc, char* argv[] )
{
    int tbm_index_error_ctr = 0;
    int tbm_core_error_ctr = 0;
    int payload_error_ctr = 0;

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
        Data cData;
        for (auto& cFED : cSystemController.fPixFEDVector)
        {
            std::cout << BOLDGREEN << "Data for FED " << +cFED->getBeId() << RESET << std::endl;
            cData.add(iAcq, cSystemController.fFEDInterface->ReadData(cFED));
        }
        iAcq++;
        if (iAcq % 1000 == 0)
        {
            cData.check();
            tbm_index_error_ctr += cData.getTBM_index_errors();
            tbm_core_error_ctr += cData.getTBM_core_errors();
            payload_error_ctr += cData.getPayload_errors();
            std::cout << "ERROR summary: " << std::endl
                      << "TBM index errors: " << tbm_index_error_ctr << std::endl
                      << "TBM core errors:  " << tbm_core_error_ctr << std::endl
                      << "Payload errors:   " << payload_error_ctr << std::endl;
        }
    }
    for (auto& cFED : cSystemController.fPixFEDVector)
    {
        cSystemController.fFEDInterface->Stop(cFED);
        std::cout << "Finished reading Data!" << std::endl;
    }
}
