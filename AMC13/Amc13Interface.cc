#include "Amc13Interface.h"

Amc13Interface::Amc13Interface( const std::string& uriT1, const std::string& addressT1, const std::string& uriT2, const std::string& addressT2 )
{
    // Log level
    //uhal::disableLogging();
    uhal::setLogLevelTo(uhal::Error());

    // this is the way if i want to keep the syntax
    uhal::ConnectionManager cm( "file://HWInterface/dummy.xml" );
    uhal::HwInterface T1( cm.getDevice( "T1", uriT1, addressT1 ) );
    uhal::HwInterface T2( cm.getDevice( "T2", uriT2, addressT2 ) );
    fAMC13 = new amc13::AMC13(T1, T2);

    //this would be the other way!
    //fAMC13 = new amc13::AMC13(uriT1, addressT1, uriT2, addressT2);
    //whatever else I need to do here!
}

Amc13Interface::~Amc13Interface()
{
    delete fAMC13;
}

void Amc13Interface::ConfigureAmc13()
{
    // first start with enabling AMCs!
    uint32_t cMask = 0;
    for (auto& cAMC : fDescription->fAMCMask)
        setBit(cMask, cAMC, true);

    // first reset the 2 boards
    fAMC13->reset(amc13::AMC13Simple::T1);
    fAMC13->reset(amc13::AMC13Simple::T2);
    // now reset the counters
    fAMC13->resetCounters();
    // now enable the AMC inputs as specified in the config File
    fAMC13->AMCInputEnable( cMask );
    std::cout << "HERE" << std::endl;
}
