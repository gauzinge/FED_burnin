/*

        FileName :                    SystemController.cc
        Content :                     Controller of the System, overall wrapper of the framework
        Programmer :                  Nicolas PIERRE
        Version :                     1.0
        Date of creation :            10/08/14
        Support :                     mail to : nicolas.pierre@cern.ch

 */

#include "SystemController.h"

SystemController::SystemController()
{
}

SystemController::~SystemController()
{
}

void SystemController::InitializeHw( const std::string& pFilename, std::ostream& os )
{
    if ( pFilename.find( ".xml" ) != std::string::npos )
        parseHWxml( pFilename, os );
    else
        std::cerr << "Could not parse settings file " << pFilename << " - it is not .xml!" << std::endl;
}

void SystemController::InitializeSettings( const std::string& pFilename, std::ostream& os )
{
    if ( pFilename.find( ".xml" ) != std::string::npos )
        parseSettingsxml( pFilename, os );
    else
        std::cerr << "Could not parse settings file " << pFilename << " - it is not .xml!" << std::endl;
}

void SystemController::ConfigureHw( std::ostream& os )
{
    // write some code
    os << BOLDGREEN <<  "Configuring FEDs: " << RESET << std::endl;
    for (auto& cFED : fPixFEDVector)
    {
        fFEDInterface->ConfigureFED(cFED);

        if (!cFED->fFitelVector.empty())
        {
            for (auto& cFitel : cFED->fFitelVector)
            {
                fFEDInterface->ConfigureFitel(cFitel, true);
                os << "Configured Fitel Rx " << +cFitel->getFitelId() << " on FMC " << +cFitel->getFMCId() << std::endl;
            }
        }
        os << "Configured FED " << +cFED->getBeId() << std::endl;
    }
    os << BOLDGREEN << "All FEDs successfully configured!" << RESET << std::endl;
}

void SystemController::HaltHw( std::ostream& os )
{
    // write some code
    os << BOLDGREEN <<  "Halting FEDs: " << RESET << std::endl;
    for (auto& cFED : fPixFEDVector)
    {
        fFEDInterface->HaltFED(cFED);

        os << "Halted FED " << +cFED->getBeId() << std::endl;
    }
    os << BOLDGREEN << "All FEDs successfully Halted!" << RESET << std::endl;
}


void SystemController::parseHWxml( const std::string& pFilename, std::ostream& os )
{
    pugi::xml_document doc;
    uint32_t cBeId;
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
    os << BOLDRED << "HW SUMMARY: " << RESET << std::endl;
    for ( i = 0; i < 80; i++ )
        os << "*";
    os << "\n";


    // Iterate the BeBoard Node
    for ( pugi::xml_node cBeBoardNode = doc.child( "HwDescription" ).child( "PixFED" ); cBeBoardNode; cBeBoardNode = cBeBoardNode.next_sibling("PixFED") )
    {

        os << BOLDCYAN << cBeBoardNode.name() << "  " << cBeBoardNode.first_attribute().name() << " :" << cBeBoardNode.attribute( "Id" ).value() << RESET << std::endl;

        cBeId = cBeBoardNode.attribute( "Id" ).as_int();
        //std::cout << +cBeId << std::endl;
        PixFED* cPixFED = new PixFED( cBeId );

        // the connection ID is not used but built in RegManager constructor!
        pugi::xml_node cBeBoardConnectionNode = cBeBoardNode.child("connection");
        std::cout << BOLDBLUE <<  "|" << "----" << "Board Id: " << BOLDYELLOW << cBeBoardConnectionNode.attribute("id").value() << BOLDBLUE << " URI: " << BOLDYELLOW << cBeBoardConnectionNode.attribute("uri").value() << BOLDBLUE << " Address Table: " << BOLDYELLOW << cBeBoardConnectionNode.attribute("address_table").value() << RESET << std::endl;

        // Iterate the BeBoardRegister Nodes
        for ( pugi::xml_node cBeBoardRegNode = cBeBoardNode.child( "Register" ); cBeBoardRegNode; cBeBoardRegNode = cBeBoardRegNode.next_sibling("Register") )
        {
            std::string nodename = cBeBoardRegNode.name();
            if (nodename == "Register") os << BOLDCYAN << "|" << "----" << cBeBoardRegNode.name() << "  " << cBeBoardRegNode.first_attribute().name() << " :" << cBeBoardRegNode.attribute( "name" ).value() << " " << BOLDRED << convertAnyInt(cBeBoardRegNode.first_child().value()) << RESET << std:: endl;
            cPixFED->setReg( std::string( cBeBoardRegNode.attribute( "name" ).value() ), convertAnyInt( cBeBoardRegNode.first_child().value() ) );
        }

        // here I need to crate the Fitels and push them back in the fFitelVector of PixFED

        pugi::xml_node cFitelPathPrefixNode = cBeBoardNode.child( "Fitel_Files" );
        std::string cFilePrefix = std::string( cFitelPathPrefixNode.attribute( "path" ).value() );
        if ( !cFilePrefix.empty() ) os << GREEN << "|" << std::endl <<  "|" << "----" << "Fitel Files Path : " << cFilePrefix << RESET << std::endl;

        // Iterate the Fitel node
        cPixFED->fFitelVector.clear();
        for ( pugi::xml_node cFitelNode = cBeBoardNode.child( "Fitel" ); cFitelNode; cFitelNode = cFitelNode.next_sibling( "Fitel" ) )
        {
            os << BOLDCYAN << "|" << "----" << cFitelNode.name() << "  " << cFitelNode.first_attribute().name() << " :" << cFitelNode.attribute( "FMC" ).as_int() << " " <<  cFitelNode.attribute( "Id" ).name() << " " << cFitelNode.attribute( "Id" ).as_int() << ", File: " << cFitelNode.attribute( "file" ).value() << RESET << std:: endl;

            std::string cFileName;
            if ( !cFilePrefix.empty() )
                cFileName = cFilePrefix + cFitelNode.attribute( "file" ).value();
            else cFileName = cFitelNode.attribute( "file" ).value();
            Fitel* cFitel = new Fitel( cBeId, cFitelNode.attribute( "FMC" ).as_int(), cFitelNode.attribute( "Id" ).as_int(), cFileName );

            // parse the list of enabled channels from the config file to apply this after configuration
            if (std::string(cFitelNode.attribute("Enable").name()) == "Enable")
            {
                //for ( pugi::xml_node cNode = pNode.child( "AMCmask" ); cNode; cNode = cNode.next_sibling("AMCmask") )
                //{
                std::string cList = std::string(cFitelNode.attribute("Enable").value());
                std::string ctoken;
                std::stringstream cStr(cList);

                os <<  BOLDCYAN << "|    |" << "----" << "List of Enabled Channels: ";
                while (std::getline(cStr, ctoken, ','))
                {
                    cFitel->fChEnableVec.push_back(convertAnyInt( ctoken.c_str() ));
                    os << GREEN << ctoken << BOLDCYAN << ", ";
                }
                os << RESET << std::endl;
            }
            cPixFED->addFitel(cFitel);
        }
        fPixFEDVector.push_back(cPixFED);

        PixFEDFWInterface* cTmpFWInterface = new PixFEDFWInterface(cBeBoardConnectionNode.attribute( "id" ).value(), cBeBoardConnectionNode.attribute( "uri" ).value(), cBeBoardConnectionNode.attribute("address_table").value() );
        //////////////////////////////////

        auto cSetting = fSettingsMap.find("NTBM");
        uint32_t cNTBM = (cSetting != std::end(fSettingsMap)) ? cSetting->second : 8;
        cSetting = fSettingsMap.find("BlockSize");
        uint32_t cBlockSize = (cSetting != std::end(fSettingsMap)) ? cSetting->second : 256;

        if (cSetting == std::end(fSettingsMap)) std::cout << "Settings Map not yet initialized!, reverting to default value for Block Size (=256 Words)" << std::endl;
        cTmpFWInterface->setNTBM(cNTBM);
        cTmpFWInterface->setBlockSize( cBlockSize );
        /////////////////////////////////
        fFWMap[cPixFED->getBeId()] = cTmpFWInterface;
    }

    // create the actual FED Interface using the map I previously filled
    fFEDInterface = new PixFEDInterface(fFWMap);

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
}

void SystemController::parseSettingsxml( const std::string & pFilename, std::ostream & os )
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file( pFilename.c_str() );

    if ( !result )
    {
        os << "ERROR :\n Unable to open the file : " << pFilename << std::endl;
        os << "Error description : " << result.description() << std::endl;
        return;
    }

    for (int i = 0; i < 80; i++ )
        os << "*";
    os << "\n";
    for (int j = 0; j < 40; j++ )
        os << " ";
    os << BOLDRED << "SETTINGS: " << RESET << std::endl;
    for (int i = 0; i < 80; i++ )
        os << "*";
    std::cout << std::endl;

    for ( pugi::xml_node nSettings = doc.child( "Settings" ); nSettings; nSettings = nSettings.next_sibling() )
    {
        for ( pugi::xml_node nSetting = nSettings.child( "Setting" ); nSetting; nSetting = nSetting.next_sibling() )
        {
            fSettingsMap[nSetting.attribute( "name" ).value()] = convertAnyInt( nSetting.first_child().value() );
            os << RED << "Setting" << RESET << " --" << BOLDCYAN << nSetting.attribute( "name" ).value() << RESET << " : " << BOLDYELLOW << convertAnyInt( nSetting.first_child().value() ) << RESET << std:: endl;
        }
    }
}
