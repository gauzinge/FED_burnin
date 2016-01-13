#ifndef _AMC13_INTERFACE_H__
#define _AMC13_INTERFACE_H__

#include <string>
#include <iostream>
#include "Amc13Description.h"
#include "amc13/AMC13.hh"
#include "uhal/uhal.hpp"
#include "../Utils/ConsoleColor.h"

class Amc13Interface
{
public:
    Amc13Interface( const std::string& uriT1, const std::string& addressT1, const std::string& uriT2, const std::string& addressT2 );
    ~Amc13Interface();

    void setAmc13Description(Amc13Description* pDescription)
    {
        fDescription = pDescription;
    }
    // a long list of methods wrapping the BU AMC13 methods!
    void ConfigureAmc13();

    // Start / Stop L1A
    void StartL1A();
    void StopL1A();
    void BurstL1A();

    // Start / Stop Run Mode for BGOs
    void StartRun();
    void StopRun();
    //void SendBGO();
    void EnableBGO(int pChan);
    void DisableBGO(int pChan);

    // TTC History methods!
    void ConfigureTTCHistory(std::vector<std::pair<int, uint32_t>> pFilterConfig);
    void EnableTTCHistory();
    void DisableTTCHistory();
    void DumpHistory(int pNlastEntries);
    void DumpTriggers(int pNlastEntries);
private:
    amc13::AMC13* fAMC13;
    Amc13Description* fDescription;

    void setBit( uint32_t& pRegValue, uint8_t pPos, bool pValue )
    {
        pRegValue ^= ( -pValue ^ pRegValue ) & ( 1 << pPos - 1 );
    }
};


#endif
