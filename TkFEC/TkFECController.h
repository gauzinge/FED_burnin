#ifndef _TKFEC_CONTROLLER_H__
#define _TKFEC_CONTROLLER_H__


#include "../Utils/pugixml.hpp"
#include "../Utils/ConsoleColor.h"
#include "CCU.h"
#include <iostream>
#include <map>
#include <string>
//#include "TkFECInterface.h"

class TkFECController
{
public:
    //Constructor & Destructor
    TkFECController();
    ~TkFECController();

    // Initialize Method to parse xml file
    void InitializeTkFEC(const std::string& pFilename, std::ostream& os = std::cout);
    // no need to configure the TkFEC, only need to configure the ring!
    void ConfigureRing();
    void RunInteracitve();

    //Interface to contain the wrapper to the FEC software (to be seen if used)
    //TkFECInterface* fTkFECInterface;
    // vector of CCUs to contain the i2c values
    std::vector<CCU*> fCCUVector;

private:
    //Helpers for xml file parsing
    void parseTkFECxml(const std::string& pFilename, std::ostream& os = std::cout);
    CCU* parseCCU(pugi::xml_node pNode, std::ostream& os = std::cout);
    i2c* parseI2C(pugi::xml_node pNode, std::ostream& os = std::cout);


    // a little helper to deal with strings
    uint32_t convertAnyInt( const char* pRegValue )
    {
        if ( std::string( pRegValue ).find( "0x" ) != std::string::npos ) return static_cast<uint32_t>( strtoul( pRegValue , 0, 16 ) );
        else return static_cast<uint32_t>( strtoul( pRegValue , 0, 10 ) );

    }
};

#endif
