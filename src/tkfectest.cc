#include <cstring>
#include "../Utils/Utilities.h"
//#include "../HWInterface/PixFEDInterface.h"
//#include "../HWDescription/PixFED.h"
//#include "../HWInterface/FpgaConfig.h"
#include "../Utils/Timer.h"
//#include <inttypes.h>
#include "../Utils/argvparser.h"
#include "../Utils/ConsoleColor.h"
//#include "../System/TkFECController.h"
#include "../TkFEC/TkFECController.h"



using namespace CommandLineProcessing;
using namespace std;



int main( int argc, char* argv[] )
{


    TkFECController cTkFECController;
    ArgvParser cmd;

    // init
    cmd.setIntroductoryDescription( "Ph1 TkFEC Configuration tool" );
    // error codes
    cmd.addErrorCode( 0, "Success" );
    cmd.addErrorCode( 1, "Error" );
    // options
    cmd.setHelpOption( "h", "help", "Print this help page" );

    cmd.defineOption( "interactive", "Run FEC SW Wrapper in Interactive mode." );
    cmd.defineOptionAlternative( "interactive", "i" );

    cmd.defineOption( "file", "HWDescription file", ArgvParser::OptionRequiresValue /*| ArgvParser::OptionRequired*/ );
    cmd.defineOptionAlternative( "file", "f" );

    int result = cmd.parse( argc, argv );

    if ( result != ArgvParser::NoParserError )
    {
        std::cout << cmd.parseErrorDescription( result );
        exit( 1 );
    }
    std::string cHWFile = ( cmd.foundOption( "file" ) ) ? cmd.optionValue( "file" ) : "settings/HwDescription.xml";
    cTkFECController.InitializeTkFEC( cHWFile );

}
