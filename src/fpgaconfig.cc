#include <cstring>
#include "../Utils/Utilities.h"
#include "../HWInterface/PixFEDInterface.h"
#include "../HWDescription/PixFED.h"
//#include "../HWInterface/FpgaConfig.h"
#include "../Utils/Timer.h"
//#include <inttypes.h>
#include "../Utils/argvparser.h"
#include "../Utils/ConsoleColor.h"
#include "../System/SystemController.h"



using namespace CommandLineProcessing;
using namespace std;



int main( int argc, char* argv[] )
{


    SystemController cSystemController;
    ArgvParser cmd;

    // init
    cmd.setIntroductoryDescription( "CMS Ph2_ACF  Data acquisition test and Data dump" );
    // error codes
    cmd.addErrorCode( 0, "Success" );
    cmd.addErrorCode( 1, "Error" );
    // options
    cmd.setHelpOption( "h", "help", "Print this help page" );



    cmd.defineOption( "list", "Print the list of available firmware images on SD card (works only with CTA boards)" );
    cmd.defineOptionAlternative( "list", "l" );

    cmd.defineOption( "delete", "Delete a firmware image on SD card (works only with CTA boards)", ArgvParser::OptionRequiresValue );
    cmd.defineOptionAlternative( "delete", "d" );

    cmd.defineOption( "file", "FPGA Bitstream (*.mcs format for GLIB or *.bit/*.bin format for CTA boards)", ArgvParser::OptionRequiresValue /*| ArgvParser::OptionRequired*/ );
    cmd.defineOptionAlternative( "file", "f" );

    cmd.defineOption( "download", "Download an FPGA configuration from SD card to file (only for CTA boards)", ArgvParser::OptionRequiresValue /*| ArgvParser::OptionRequired*/ );
    cmd.defineOptionAlternative( "download", "o" );

    cmd.defineOption( "config", "Hw Description File . Default value: settings/HWDescription.xml", ArgvParser::OptionRequiresValue /*| ArgvParser::OptionRequired*/ );
    cmd.defineOptionAlternative( "config", "c" );

    cmd.defineOption( "image", "Load to image 1 (golden) or 2 (user) or named image for CTA boards, jump to the given image if no file is specified", ArgvParser::OptionRequiresValue);
    cmd.defineOptionAlternative("image", "i");

    int result = cmd.parse( argc, argv );

    if ( result != ArgvParser::NoParserError )
    {
        std::cout << cmd.parseErrorDescription( result );
        exit( 1 );
    }
    std::string cHWFile = ( cmd.foundOption( "config" ) ) ? cmd.optionValue( "config" ) : "settings/FWUpload.xml";
    cSystemController.InitializeHw( cHWFile );
    //cSystemController.ConfigureHw( std::cout);

    //TODO: fix me and introduce a loop
    PixFED* pBoard = cSystemController.fPixFEDVector.at(0);
    //for ( auto& pBoard : cSystemController.fPixFEDVector )
    //{
    vector<string> lstNames = cSystemController.fFEDInterface->getFpgaConfigList(pBoard);
    std::string cFWFile;
    std::string strImage("1");
    if (cmd.foundOption("list"))
    {
        std::cout << lstNames.size() << " firmware images on SD card:" << std::endl;
        for (auto &name : lstNames)
            std::cout << " - " << name << std::endl;

        exit(0);
    }
    else if (cmd.foundOption("file"))
    {
        cFWFile = cmd.optionValue("file");
        if (lstNames.size() == 0 && cFWFile.find(".mcs") == std::string::npos)
        {
            std::cout << "Error, the specified file is not a .mcs file" << std::endl;
            //exit(1);
        }
        else if (lstNames.size() > 0 && cFWFile.compare(cFWFile.length() - 4, 4, ".bit") && cFWFile.compare(cFWFile.length() - 4, 4, ".bin"))
        {
            std::cout << "Error, the specified file is neither a .bit nor a .bin file" << std::endl;
            exit(1);
        }
    }
    else if (cmd.foundOption("delete") && !lstNames.empty())
    {
        strImage = cmd.optionValue("delete");
        verifyImageName(strImage, lstNames);
        cSystemController.fFEDInterface->DeleteFpgaConfig(pBoard, strImage);
        exit(0);
    }
    else if (!cmd.foundOption("image"))
    {
        cFWFile = "";
        std::cout << "Error, no FW image specified" << std::endl;
        exit(1);
    }

    if (cmd.foundOption("image"))
    {
        strImage = cmd.optionValue("image");
        if (!cmd.foundOption("file"))
            verifyImageName(strImage, lstNames);
    }
    else if (!lstNames.empty())
        strImage = "GoldenImage.bin";

    Timer t;
    t.start();


    t.stop();
    t.show( "Time to Initialize/configure the system: " );

    if (!cmd.foundOption("file") && !cmd.foundOption("download"))
    {
        cSystemController.fFEDInterface->JumpToFpgaConfig(pBoard, strImage);
        exit(0);
    }

    bool cDone = 0;


    if (cmd.foundOption("download"))
        cSystemController.fFEDInterface->DownloadFpgaConfig(pBoard, strImage, cmd.optionValue("download"));
    else
        cSystemController.fFEDInterface->FlashProm(pBoard, strImage, cFWFile.c_str());

    uint32_t progress;

    while (cDone == 0)
    {
        progress = cSystemController.fFEDInterface->getConfiguringFpga(pBoard)->getProgressValue();

        if (progress == 100)
        {
            cDone = 1;
            std::cout << "\n 100% Done" << std::endl;
        }
        else
        {
            std::cout << progress << "%  " << cSystemController.fFEDInterface->getConfiguringFpga(pBoard)->getProgressString() << "                 \r" << flush;
            sleep(1);
        }
    }


    t.stop();
    t.show( "Time elapsed:" );

    //}
}
