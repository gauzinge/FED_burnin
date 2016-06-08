#include <ctime>
#include <cstring>
#include "uhal/uhal.hpp"
#include "../Utils/Utilities.h"
#include "../HWDescription/PixFED.h"
#include "../HWInterface/PixFEDInterface.h"
#include "../System/SystemController.h"
#include "../AMC13/Amc13Controller.h"
#include "../Utils/Data.h"

int main (int argc, char* argv[] )
{
    const char* cHWFile = argv[1];
    std::cout << "HW Description File: " << cHWFile << std::endl;

    uhal::setLogLevelTo (uhal::Debug() );

    // instantiate System Controller
    SystemController cSystemController;
    Amc13Controller  cAmc13Controller;

    // initialize map of settings so I can know the proper number of acquisitions and TBMs
    cSystemController.InitializeSettings (cHWFile, std::cout);

    // initialize HWdescription from XML, beware, settings have to be read first
    cAmc13Controller.InitializeAmc13 ( cHWFile, std::cout );
    cSystemController.InitializeHw (cHWFile, std::cout);

    auto cSetting = cSystemController.fSettingsMap.find ("NAcq");
    int cNAcq = (cSetting != std::end (cSystemController.fSettingsMap) ) ? cSetting->second : 10;
    cSetting = cSystemController.fSettingsMap.find ("BlockSize");
    int cBlockSize = (cSetting != std::end (cSystemController.fSettingsMap) ) ? cSetting->second : 2;

    cSetting = cSystemController.fSettingsMap.find ("ChannelOfInterest");
    int cChannelOfInterest = (cSetting != std::end (cSystemController.fSettingsMap) ) ? cSetting->second : 0;

    cSetting = cSystemController.fSettingsMap.find ("ROCOfInterest");
    int cROCOfInterest = (cSetting != std::end (cSystemController.fSettingsMap) ) ? cSetting->second : 0;

    // configure the AMC13
    cAmc13Controller.ConfigureAmc13 ( std::cout );

    //configure FED & FITELS & SFP+
    cSystemController.ConfigureHw ( std::cout );

    // get the board info of all boards and start the acquistion
    for (auto& cFED : cSystemController.fPixFEDVector)
    {
        //cSystemController.fFEDInterface->getBoardInfo(cFED);
        cSystemController.fFEDInterface->findPhases (cFED, cChannelOfInterest);
        //cSystemController.fFEDInterface->Start (cFED);
    }

    //std::cout << "Monitoring Phases for selected Channel of Interest for 10 seconds ... " << std::endl << std::endl;
    //std::cout << BOLDGREEN << "FIBRE CTRL_RDY CNTVAL_Hi CNTVAL_Lo   pattern:                     S H1 L1 H0 L0   W R" << RESET << std::endl;
    //while (true)
    //{
    //for (auto& cFED : cSystemController.fPixFEDVector)
    //{
    //cSystemController.fFEDInterface->monitorPhases(cFED, cChannelOfInterest);
    //}
    //}

    uint32_t iAcq = 0;
    //bool running = true;
    std::cout << "FED Configured, SLink Enabled, pressing Enter will send an EC0 & start periodic L1As" << std::endl; 
    std::cout << "Pressing Enter again will stop the application" << std::endl;
    mypause();

    cAmc13Controller.fAmc13Interface->SendEC0();
    cAmc13Controller.fAmc13Interface->StartL1A();
    for (auto& cFED : cSystemController.fPixFEDVector)
    {
        //cSystemController.fFEDInterface->getBoardInfo(cFED);
        cSystemController.fFEDInterface->Start (cFED);
    }

    while ( !kbhit() )
    {
        //std::cout << cNAcq << " ##########################" << std::endl;
        for (auto& cFED : cSystemController.fPixFEDVector)
        {
            //cSystemController.fFEDInterface->ReadData(cFED, 0 );
            cSystemController.fFEDInterface->readFIFO1 (cFED);
            cSystemController.fFEDInterface->PrintSlinkStatus (cFED);
            usleep (1000000);
        }

        iAcq++;

        if (iAcq < cNAcq && cNAcq > 0 ) continue;
        else if (cNAcq == 0 ) continue;
        else break;
    }

    cAmc13Controller.fAmc13Interface->StopL1A();

    for (auto& cFED : cSystemController.fPixFEDVector)
        cSystemController.fFEDInterface->Stop (cFED);


    //    cSystemController.HaltHw();
        cAmc13Controller.HaltAmc13();
    exit (0);
}
