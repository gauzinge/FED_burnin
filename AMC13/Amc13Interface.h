#ifndef _AMC13_INTERFACE_H__
#define _AMC13_INTERFACE_H__

#include <string>
#include <iostream>
#include "Amc13Description.h"
#include "amc13/AMC13.hh"
#include "uhal/uhal.hpp"

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

private:
    amc13::AMC13* fAMC13;
    Amc13Description* fDescription;

    void setBit( uint32_t& pRegValue, uint8_t pPos, bool pValue )
    {
        pRegValue ^= ( -pValue ^ pRegValue ) & ( 1 << pPos - 1 );
    }
};


#endif
