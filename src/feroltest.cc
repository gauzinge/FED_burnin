#include <ctime>
#include <cstring>
#include "uhal/uhal.hpp"
#include "../Utils/Utilities.h"
#include "../HWDescription/PixFED.h"
#include "../HWInterface/PixFEDInterface.h"
#include "../System/SystemController.h"
#include "../AMC13/Amc13Controller.h"
#include "../Utils/Data.h"

int main(int argc, char* argv[] )
{
    const char* cHWFile = argv[1];
    std::cout << "HW Description File: " << cHWFile << std::endl;

    uhal::setLogLevelTo(uhal::Debug());

    // instantiate System Controller
    SystemController cSystemController;
    Amc13Controller  cAmc13Controller;

    // initialize map of settings so I can know the proper number of acquisitions and TBMs
    cSystemController.InitializeSettings(cHWFile, std::cout);

    // initialize HWdescription from XML, beware, settings have to be read first
    cAmc13Controller.InitializeAmc13( cHWFile, std::cout );
    cSystemController.InitializeHw(cHWFile, std::cout);

    // configure the HW
    cAmc13Controller.ConfigureAmc13( std::cout );
    cSystemController.ConfigureHw(std::cout );


    auto cSetting = cSystemController.fSettingsMap.find("NAcq");
    int cNAcq = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 10;
    cSetting = cSystemController.fSettingsMap.find("BlockSize");
    int cBlockSize = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 2;

    cSetting = cSystemController.fSettingsMap.find("ChannelOfInterest");
    int cChannelOfInterest = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 0;

    cSetting = cSystemController.fSettingsMap.find("ROCOfInterest");
    int cROCOfInterest = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 0;

    cSetting = cSystemController.fSettingsMap.find("NEventsCommissioningMode");
    int cNEventsCommMode = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 1;

    // get the board info of all boards and start the acquistion
    for (auto& cFED : cSystemController.fPixFEDVector)
    {
      cSystemController.fFEDInterface->InitSlink(cFED);
      cSystemController.fFEDInterface->getBoardInfo(cFED);
      cSystemController.fFEDInterface->findPhases(cFED, cChannelOfInterest);
      cSystemController.fFEDInterface->Start(cFED);
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
    bool running = true;
    mypause();
    cAmc13Controller.fAmc13Interface->StartL1A();
    //cSystemController.fFEDInterface->Start();
    while ( true )
    {
//std::cout << cNAcq << " ##########################" << std::endl;
         for (auto& cFED : cSystemController.fPixFEDVector)
         {

            //cSystemController.fFEDInterface->WriteBoardReg(cFED, "fe_ctrl_regs.decode_reg_reset", 1);
            // mypause();
             //cSystemController.fFEDInterface->readTransparentFIFO(cFED);
//             cSystemController.fFEDInterface->readSpyFIFO(cFED);
//            cSystemController.fFEDInterface->readFIFO1(cFED);
//             cSystemController.fFEDInterface->readOSDWord(cFED, cROCOfInterest, cChannelOfInterest);
//	   cSystemController.fFEDInterface->ReadData(cFED, 0 );
	   cSystemController.fFEDInterface->PrintSlinkStatus(cFED);
	   usleep(10000);
//           mypause();
//             cSystemController.fFEDInterface->ReadNEvents(cFED, cNEventsCommMode );
         }
        iAcq++;
        if (iAcq < cNAcq && cNAcq > 0 )running = true;
        else if (cNAcq == 0 ) running = true;
        else running = false;
 }   
    cAmc13Controller.fAmc13Interface->StopL1A();

//    cSystemController.HaltHw();
//    cAmc13Controller.HaltAmc13();
    exit(0);
}
