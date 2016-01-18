#ifndef _AMC13_CONTROLLER_H__
#define _AMC13_CONTROLLER_H__

#include "../Utils/pugixml.hpp"
#include "../Utils/ConsoleColor.h"
#include "Amc13Interface.h"
#include "Amc13Description.h"
#include <iostream>
#include <map>
#include <string>

class Amc13Controller
{
public:
    Amc13Controller();
    ~Amc13Controller();

    void InitializeAmc13( const std::string& pFilename, std::ostream& os = std::cout );
    void ConfigureAmc13( std::ostream& os = std::cout );
    void HaltAmc13( std::ostream& os = std::cout );
public:

    Amc13Interface* fAmc13Interface;
    Amc13Description* fAmc13;

private:
    // Private methods for parsing!
    void parseAmc13xml(const std::string& pFilename, std::ostream& os = std::cout);

    // low level helpers to parse specific xml TAGS
    std::vector<int> parseAMCMask(pugi::xml_node pNode, std::ostream& os = std::cout);
    BGO* parseBGO(pugi::xml_node pNode, std::ostream& os = std::cout);
    Trigger* parseTrigger( pugi::xml_node pNode , std::ostream& os = std::cout);

    // a little helper to deal with strings
    uint32_t convertAnyInt( const char* pRegValue )
    {
        if ( std::string( pRegValue ).find( "0x" ) != std::string::npos ) return static_cast<uint32_t>( strtoul( pRegValue , 0, 16 ) );
        else return static_cast<uint32_t>( strtoul( pRegValue , 0, 10 ) );

    }
};

#endif
