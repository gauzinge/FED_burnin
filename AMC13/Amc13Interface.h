#ifndef _AMC13_INTERFACE_H__
#define _AMC13_INTERFACE_H__

#include <string>
#include <iostream>
#include "amc13/AMC13.hh"

class Amc13Interface
{
public:
    Amc13Interface( const std::string& uriT1, const std::string& addressT1, const std::string& uriT2, const std::string& addressT2 );
    ~Amc13Interface();

    // a long list of methods wrapping the BU AMC13 methods!


private:
    amc13::AMC13* fAMC13;
};


#endif
