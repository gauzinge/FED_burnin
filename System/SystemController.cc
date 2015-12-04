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
    std::cout << BOLDGREEN <<  "Configuring FEDs: " << RESET << std::endl;
    for (auto& cFED : fPixFEDVector)
    {
        fFEDInterface->ConfigureFED(cFED);
        std::cout << "Configured FED " << +cFED->getBeId() << std::endl;
    }
    std::cout << BOLDGREEN << "All FEDs successfully configured!" << RESET << std::endl;
}

// void SystemController::Run( BeBoard* pBeBoard, uint32_t pNthAcq )
// {
//  fBeBoardInterface->Start( pBeBoard );
//  fBeBoardInterface->ReadData( pBeBoard, pNthAcq, true );
//  fBeBoardInterface->Stop( pBeBoard, pNthAcq );
// }

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
    os << "\n";


    // Iterate the BeBoard Node
    for ( pugi::xml_node cBeBoardNode = doc.child( "HwDescription" ).child( "PixFED" ); cBeBoardNode; cBeBoardNode = cBeBoardNode.next_sibling() )
    {

        os << BOLDCYAN << cBeBoardNode.name() << "  " << cBeBoardNode.first_attribute().name() << " :" << cBeBoardNode.attribute( "Id" ).value() << RESET << std:: endl;

        cBeId = cBeBoardNode.attribute( "Id" ).as_int();
        PixFED* cPixFED = new PixFED( cBeId );

        // the connection ID is not used but built in RegManager constructor!

        // Iterate the BeBoardRegister Nodes
        for ( pugi::xml_node cBeBoardRegNode = cBeBoardNode.child( "Register" ); cBeBoardRegNode/* != cBeBoardNode.child( "Module" )*/; cBeBoardRegNode = cBeBoardRegNode.next_sibling() )
        {
            os << BOLDCYAN << "|" << "_____" << cBeBoardRegNode.name() << "  " << cBeBoardRegNode.first_attribute().name() << " :" << cBeBoardRegNode.attribute( "name" ).value() << " " << BOLDRED << atoi(cBeBoardRegNode.first_child().value()) << RESET << std:: endl;
            cPixFED->setReg( std::string( cBeBoardRegNode.attribute( "name" ).value() ), atoi( cBeBoardRegNode.first_child().value() ) );
        }
        fPixFEDVector.push_back(cPixFED);
        PixFEDFWInterface* cTmpFWInterface = new PixFEDFWInterface(doc.child("HwDescription").child("Connections").attribute("name").value(), cBeId);
        //FIXEM
        //////////////////////////////////
        auto cSetting = fSettingsMap.find("NTBM");
        uint32_t cNTBM = (cSetting != std::end(fSettingsMap)) ? cSetting->second : 8;
        if (cSetting == std::end(fSettingsMap)) std::cout << "Settings Map not yet initialized!, reverting to default value (8)" << std::endl;
        cTmpFWInterface->setNTBM(cNTBM);
        /////////////////////////////////
        fFWMap[cPixFED->getBeId()] = cTmpFWInterface;
    }

    // create the actual FED Interface using the map I previously filled
    fFEDInterface = new PixFEDInterface(fFWMap);

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

void SystemController::parseSettingsxml( const std::string& pFilename, std::ostream& os )
{
    pugi::xml_document doc;
    pugi::xml_parse_result result = doc.load_file( pFilename.c_str() );

    if ( !result )
    {
        os << "ERROR :\n Unable to open the file : " << pFilename << std::endl;
        os << "Error description : " << result.description() << std::endl;
        return;
    }
    for ( pugi::xml_node nSettings = doc.child( "Settings" ); nSettings; nSettings = nSettings.next_sibling() )
    {
        for ( pugi::xml_node nSetting = nSettings.child( "Setting" ); nSetting; nSetting = nSetting.next_sibling() )
        {
            fSettingsMap[nSetting.attribute( "name" ).value()] = convertAnyInt( nSetting.first_child().value() );
            os << RED << "Setting" << RESET << " --" << BOLDCYAN << nSetting.attribute( "name" ).value() << RESET << " : " << BOLDYELLOW << convertAnyInt( nSetting.first_child().value() ) << RESET << std:: endl;
        }
    }
}
