#ifndef _AMC13_PARSER_H__
#define _AMC13_PARSER_H__

#include "../Utils/pugixml.hpp"
#include "../Utils/ConsoleColor.h"
#include "Amc13Interface.h"
#include "Amc13Description.h"
#include <iostream>
#include <map>
#include <string>

class Amc13Parser
{
public:

    void parseAmc13XML(const std::string& pFilename, std::ostream& os = std::cout);

protected:

    Amc13Interface* fAmc13Interface;
    Amc13Description* fAmc13;

private:

    std::vector<int> parseAMCMask(pugi::xml_node pNode, std::ostream& os = std::cout);
    BGO* parseBGO(pugi::xml_node pNode, std::ostream& os = std::cout);
    Trigger* parseTrigger( pugi::xml_node pNode , std::ostream& os = std::cout);

    uint32_t convertAnyInt( const char* pRegValue )
    {
        if ( std::string( pRegValue ).find( "0x" ) != std::string::npos ) return static_cast<uint32_t>( strtoul( pRegValue , 0, 16 ) );
        else return static_cast<uint32_t>( strtoul( pRegValue , 0, 10 ) );

    }
};

#endif
