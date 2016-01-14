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

    // first reset the 2 boards
    std::cout << "Resetting T1, T2 & all counters!" << std::endl;
    fAMC13->reset(amc13::AMC13Simple::T1);
    fAMC13->reset(amc13::AMC13Simple::T2);
    // now reset the counters
    fAMC13->resetCounters();
    // now enable the AMC inputs as specified in the config File
    std::cout << "Enabling TTC links for the following AMCs: " << std::endl;
    for (auto& cAMC : fDescription->fAMCMask)
    {
        setBit(cMask, cAMC, true);
        std::cout << cAMC << ", ";
    }
    std::cout << std::endl;
    fAMC13->AMCInputEnable( cMask );

    //now configure the BGOs, loop through the list, get the properties and call the function
    int cIndex = 0;
    if(!fDescription->fBGOs.empty())
    {
    for (auto& cBGO : fDescription->fBGOs )
    {
        //fAMC13->configureBGOShort(cIndex, uint8_t(cBGO->fCommand), uint16_t(cBGO->fBX), uint16_t(cBGO->fPrescale), cBGO->fRepeat);
        //fAMC13->enableBGO(cIndex);
        std::cout << "Configured & enabling BGO Channel " << cIndex << " : Command: " << cBGO->fCommand << " BX: " << cBGO->fBX << " Prescale: " << cBGO->fPrescale << " Repetetive: " << cBGO->fRepeat << std::endl;
        cIndex++;
    }
    }

    // now configure the Trigger
    if(fDescription->fTrigger != nullptr)
    {
    fAMC13->configureLocalL1A(fDescription->fTrigger->fLocal, fDescription->fTrigger->fMode, uint32_t(fDescription->fTrigger->fBurst), uint32_t(fDescription->fTrigger->fRate), fDescription->fTrigger->fRules );
    std::cout << "Configuring local L1A: Mode: " << fDescription->fTrigger->fMode << " Rate: " << fDescription->fTrigger->fRate << " Burst: " << fDescription->fTrigger->fBurst << " Rules: " << fDescription->fTrigger->fRules << std::endl;
    }
    // if TTC simulator is enabled, the loopback fiber is required and no external TTC stream will be received, the Triggers are local by definition
    if (fDescription->fSimulate)
    {
        fAMC13->localTtcSignalEnable(fDescription->fSimulate);
        std::cout << RED << "AMC13 configured to use local TTC simulator - don't forget to plug the loopback fibre!" << RESET << std::endl;
    }

    //now need to iterate the two maps of Registers and write them
    for (auto& cReg : fDescription->fT1map)
        fAMC13->write(amc13::AMC13Simple::T1, cReg.first, cReg.second);

    for (auto& cReg : fDescription->fT2map)
        fAMC13->write(amc13::AMC13Simple::T2, cReg.first, cReg.second);

    std::cout << GREEN << "AMC13 successfully configured!" << RESET << std::endl;
}


void Amc13Interface::StartL1A()
{
    fAMC13->startContinuousL1A();
}

void Amc13Interface::StopL1A()
{
    fAMC13->stopContinuousL1A();
}

void Amc13Interface::BurstL1A()
{
    fAMC13->sendL1ABurst();
}

//void Amc13Interface::SendBGO()
//{
//fAMC13->sendBGO();
//}

void Amc13Interface::EnableBGO(int pChan)
{
    fAMC13->enableBGO( pChan );
}

void Amc13Interface::DisableBGO(int pChan)
{
    fAMC13->disableBGO( pChan );
}

void Amc13Interface::EnableTTCHistory()
{
    fAMC13->setTTCHistoryEna(true);
}

void Amc13Interface::DisableTTCHistory()
{
    fAMC13->setTTCHistoryEna(false);
}

void Amc13Interface::ConfigureTTCHistory(std::vector<std::pair<int, uint32_t>> pFilterConfig)
{
    // n = int in the pair  ... history item
    // filterVal = uint32_t ... filter Value
    for (auto& cPair : pFilterConfig)
        fAMC13->setTTCHistoryFilter(cPair.first, cPair.second);
}

void Amc13Interface::DumpHistory(int pNlastEntries)
{
    std::vector<uint32_t> cVec = fAMC13->getTTCHistory(pNlastEntries);
    fAMC13->setTTCHistoryEna(true);

    //now decode the Info in here!
    std::cout << BOLDRED << "TTC History showing the last " << pNlastEntries << " items!" << RESET << std::endl;
    for (int index = 0; index < cVec.size() / 4; index++)
    {
        // 4 32-bit words per command in the history
        uint32_t cCommand = cVec.at(index * 4 + 0);
        uint32_t cOrbit = cVec.at(index * 4 + 1);
        uint32_t cBX    = cVec.at(index * 4 + 2);
        uint32_t cEvent = cVec.at(index * 4 + 3);

        std::cout << "Command: " << (cCommand & 0xFF) << " - Orbit: " << cOrbit << " - BX: " << (cBX & 0x7FF) << " - Event Nr: " << (cEvent & 0x00FFFFFF) << std::endl;
    }
}

void Amc13Interface::DumpTriggers(int pNlastEntries)
{
    if ( pNlastEntries > 127 ) std::cerr << "Only last 128 Events available in L1A history buffer!" << std::endl;

    std::vector<uint32_t> cVec = fAMC13->getL1AHistory(pNlastEntries);

    //now decode the Info in here!
    std::cout << BOLDRED << "L1A History showing the last " << pNlastEntries << " items!" << RESET << std::endl;
    for (int index = 0; index < cVec.size() / 4; index++)
    {
        // 4 32-bit words per command in the history
        uint32_t cOrbit = cVec.at(index * 4 + 0);
        uint32_t cBunch = cVec.at(index * 4 + 1);
        uint32_t cEventNr = cVec.at(index * 4 + 2);
        uint32_t cFlags = cVec.at(index * 4 + 3);

        std::cout << "Orbit: " << cOrbit << " - Bunch: " << (cBunch & 0xFFF) << " - Event Nr: " << (cEventNr & 0xFFFFFF) << " - Flags: " << cFlags << std::endl;
    }
}

//void Amc13Interface::HaltAMC13(){
     
//}
