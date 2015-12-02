#include <cstring>
#include "uhal/uhal.hpp"
#include "../Utils/Utilities.h"
#include "../HWDescription/PixFED.h"
#include "../HWInterface/PixFEDInterface.h"
#include "../System/SystemController.h"


int main(int argc, char* argv[] )
{
    uhal::setLogLevelTo(uhal::Debug());
    SystemController cSystemController;
    cSystemController.InitializeHw("settings/HWDescription.xml");
    cSystemController.ConfigureHw(std::cout );
    for (auto& cFED : cSystemController.fPixFEDVector)
    {
        cSystemController.fFEDInterface->getBoardInfo(cFED);

    }
    std::cout << "Finished!" << std::endl;
}
