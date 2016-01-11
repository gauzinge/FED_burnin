#include "Amc13Parser.h"

void Amc13Parser::parseAmc13XML(const std::string& pFilename, std::ostream& os)
{
    pugi::xml_document doc;
    int i, j;

    pugi::xml_parse_result result = doc.load_file( pFilename.c_str() );

    if ( !result )
    {
        os << "ERROR :\n Unable to open the file : " << pFilename << std::endl;
        os << "Error description : " << result.description() << std::endl;
        return;
    }

    os << "\n";
    for ( i = 0; i < 80; i++ )
        os << "*";
    os << "\n";
    for ( j = 0; j < 40; j++ )
        os << " ";
    os << BOLDRED << " AMC13 Settings " << RESET << std::endl;
    for ( i = 0; i < 80; i++ )
        os << "*";
    os << "\n";
    os << "\n";

    pugi::xml_node cAmc13node = doc.child( "HWDescription" ).child( "AMC13" );

    os << BOLDCYAN << cAmc13node.name() << std::endl;
    // now create a new AMC13 Description Object
    fAmc13 = new Amc13Description();
    // now get all the properties and add to the Amc13Description object
    fAmc13->setAMCMask( parseAMCMask( cAmc13node.child( "AMCmask" ) ) );
    fAmc13->setTrigger( parseTrigger( cAmc13node.child( "Trigger" ) ) );

    for (pugi::xml_node cBGOnode = cAmc13node.child( "BGO" ); cBGOnode; cBGOnode = cBGOnode.next_sibling())
    {
        fAmc13->addBGO( parseBGO( cBGOnode ) );
    }

    // here loop over the Register nodes and add them to the AMC13Description object!
    for ( pugi::xml_node Amc13RegNode = cAmc13node.child( "Register" ); Amc13RegNode; Amc13RegNode = Amc13RegNode.next_sibling() )
    {
        std::string nodename = Amc13RegNode.name();
        std::string regname = std::string(Amc13RegNode.attribute( "name" ).value());

        if (nodename == "Register") os << BOLDCYAN << "|" << "----" << Amc13RegNode.name() << "  " << Amc13RegNode.attribute( "tounge" ).name() << " : " << atoi(Amc13RegNode.attribute( "tounge" ).value()) << " - " << regname << " " << BOLDRED << atoi(Amc13RegNode.first_child().value()) << RESET << std:: endl;
        fAmc13->setReg( atoi(Amc13RegNode.attribute( "tounge" ).value()),  regname, convertAnyInt( Amc13RegNode.first_child().value() ) );
    }

    std::string cUri1, cUri2;
    std::string cAddressT1, cAddressT2;
    // finally find the connection nodes and construct the proper Amc13FWInterface
    for ( pugi::xml_node Amc13Connection = cAmc13node.child( "connection" ); Amc13Connection; Amc13Connection = Amc13Connection.next_sibling() )
    {
        if ( Amc13Connection.attribute( "id" ).value() == "T1")
        {
            cUri1 = Amc13Connection.attribute( "uri" ).value();
            cAddressT1 = Amc13Connection.attribute( "address_table" ).value();
        }
        else if ( Amc13Connection.attribute( "id" ).value() == "T2" )
        {

            cUri2 = Amc13Connection.attribute( "uri" ).value();
            cAddressT2 = Amc13Connection.attribute( "address_table" ).value();
        }

        std::cout << BOLDBLUE <<  "|" << "----" << "Board Id: " << BOLDYELLOW << Amc13Connection.attribute("id").value() << BOLDBLUE << " URI: " << BOLDYELLOW << Amc13Connection.attribute("uri").value() << BOLDBLUE << " Address Table: " << BOLDYELLOW << Amc13Connection.attribute("address_table").value() << RESET << std::endl;
    }

    fAmc13Interface = new Amc13Interface(cUri1, cAddressT1, cUri2, cAddressT2);

    os << "\n";
    os << "\n";
    for ( i = 0; i < 80; i++ )
        os << "*";
    os << "\n";
    for ( j = 0; j < 40; j++ )
        os << " ";
    os << BOLDRED << "END OF HW SUMMARY: " << RESET << std::endl;
    for ( i = 0; i < 80; i++ )
        os << "*";
    os << "\n";
    os << "\n";
}

std::vector<int> Amc13Parser::parseAMCMask(pugi::xml_node pNode,  std::ostream& os)
{
    std::vector<int> cVec;
    if ( pNode.name() == "AMCmask" )
    {
        std::string cList = std::string(pNode.attribute("enable").value());
        std::string ctoken;
        std::stringstream cStr(cList);

        os <<  BOLDCYAN << "|" << "----" << "List of Enabled AMCs: ";
        while (std::getline(cStr, ctoken, ','))
        {
            cVec.push_back(convertAnyInt( ctoken.c_str() ));
            os << GREEN << ctoken << BOLDCYAN << ", ";
        }
        os << RESET << std::endl;
    }
    return cVec;
}

BGO* Amc13Parser::parseBGO(pugi::xml_node pNode,  std::ostream& os)
{
    BGO* cBGO = nullptr;
    if ( pNode.name() == "BGO")
    {
        cBGO = new BGO( atoi(pNode.attribute( "channel" ).value()), bool(pNode.attribute( "enable" ).value()), atoi(pNode.attribute( "prescale" ).value() ), atoi(pNode.attribute( "bx" ).value()) );
        os <<  BOLDCYAN << "|" << "----" << "BGO : channel " << cBGO->fChannel << " status[" << cBGO->fStatus << "] Prescale " << cBGO->fPrescale << " start BX " << cBGO->fBX << RESET << std::endl;
    }
    return cBGO;
}

Trigger* Amc13Parser::parseTrigger( pugi::xml_node pNode, std::ostream& os )
{
    Trigger* cTrg = nullptr;
    if ( pNode.name() == "Trigger" )
    {
        cTrg = new Trigger( atoi(pNode.attribute( "type" ).value()), atoi( pNode.attribute( "rate" ).value()), atoi( pNode.attribute( "rules" ).value() ) );
        os <<  BOLDGREEN << "|" << "----" << "Trigger Config : Type " << cTrg->fType << " Rate " << cTrg->fRate << " Rules " << cTrg->fRules << RESET << std::endl;

    }
    return cTrg;
}

//std::pair<std::string, uint32_t> parseRegister( pugi::xml_node pNode )
//{

//}
