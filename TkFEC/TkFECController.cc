#include "TkFECController.h"

TkFECController::TkFECController()
{
    fCCUVector.clear();
}

TkFECController::~TkFECController()
{
    fCCUVector.clear();
}

void TkFECController::InitializeTkFEC(const std::string& pFilename, std::ostream& os )
{
std::cout << pFilename << std::endl;
    if ( pFilename.find(".xml") != std::string::npos )
        parseTkFECxml( pFilename, os );
    else std::cerr << "Could not parse settings file " << pFilename << " - it is not .xml!" << std::endl;
}

void TkFECController::parseTkFECxml(const std::string& pFilename, std::ostream& os)
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
    os << BOLDRED << "TkFEC Settings " << RESET << std::endl;
    for ( i = 0; i < 80; i++ )
        os << "*";
    os << "\n";

    // no clue why I have to loop but that's what it is!
    for ( pugi::xml_node cTkFECNode = doc.child( "HwDescription" ).child( "TkFEC" ); cTkFECNode; cTkFECNode = cTkFECNode.next_sibling("TkFEC") )
    {
        os << BOLDCYAN << cTkFECNode.name() << RESET << std::endl;        // now create a new AMC13 Description Object
        //if ( fAmc13 != nullptr ) delete fAmc13;
        //fAmc13 = new Amc13Description();

        std::string cId;
        std::string cUri;
        std::string cAddress;

        pugi::xml_node cTkFECConnection = cTkFECNode.child("connection");

        cId = cTkFECConnection.attribute("id").value();
        cUri = cTkFECConnection.attribute("uri").value();
        cAddress = cTkFECConnection.attribute("address_table").value();

        std::cout << BOLDBLUE <<  "|" << "----" << "Board Id: " << BOLDYELLOW << cTkFECConnection.attribute("id").value() << BOLDBLUE << " URI: " << BOLDYELLOW << cTkFECConnection.attribute("uri").value() << BOLDBLUE << " Address Table: " << BOLDYELLOW << cTkFECConnection.attribute("address_table").value() << RESET << std::endl;


        // here loop over the CCU nodes
        for ( pugi::xml_node cCCUNode = cTkFECNode.child( "CCU" ); cCCUNode; cCCUNode = cCCUNode.next_sibling("CCU") )
        {
            CCU* cCCU = parseCCU( cCCUNode, os );

            if (cCCU != nullptr)
            {
                // here loop over the i2c nodes and parse 1 after the other
                for ( pugi::xml_node ci2cNode = cCCUNode.child( "i2c" ); ci2cNode; ci2cNode = ci2cNode.next_sibling("i2c") )
                {
                    cCCU->fChannelVector.push_back( parseI2C( ci2cNode, os ) );
                }
                fCCUVector.push_back( cCCU );
            }
        }

        // here create the tkFEC interface, whatever it might be
    }

}


CCU* TkFECController::parseCCU(pugi::xml_node pNode, std::ostream& os)
{
    CCU* cCCU = nullptr;
    if ( std::string (pNode.name()) == "CCU")
    {
        std::string address = pNode.attribute( "address" ).value();
        std::string ring    = pNode.attribute( "ring" ).value();
        cCCU = new CCU(convertAnyInt(address.c_str()), convertAnyInt(ring.c_str()));

        os << BOLDCYAN << "|" << "----" << "CCU : address " << address << " ring " << ring << RESET << std::endl;
    }
    else
        std::cout << "Error, no CCU node found!" << std::endl;
    return cCCU;
}

i2c* TkFECController::parseI2C(pugi::xml_node pNode, std::ostream& os)
{
    i2c* ci2c = nullptr;
    if ( std::string (pNode.name()) == "i2c")
    {
        std::string channel = pNode.attribute( "channel" ).value();
        std::string address = pNode.attribute( "address" ).value();
        std::string value   = pNode.attribute( "value"   ).value();

        ci2c = new i2c(convertAnyInt(channel.c_str()), convertAnyInt(address.c_str()), convertAnyInt(value.c_str()));

        os << BOLDCYAN << "|    |" << "----" << "I2C : channel " << channel << " address " << address << " value " << value << RESET << std::endl;
    }
    else
        std::cout << "Error, no I2C node found!" << std::endl;
    return ci2c;
}
