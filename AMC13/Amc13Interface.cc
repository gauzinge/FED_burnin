#include "Amc13Interface.h"

Amc13Interface::Amc13Interface( const std::string& uriT1, const std::string& addressT1, const std::string& uriT2, const std::string& addressT2 )
{
    fAMC13 = new amc13::AMC13(uriT1, addressT1, uriT2, addressT2);
    //whatever else I need to do here!
}
