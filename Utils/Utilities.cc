/*

    FileName :                     Utilities.cc
    Content :                      Some objects that might come in handy
    Programmer :                   Nicolas PIERRE
    Version :                      1.0
    Date of creation :             10/06/14
    Support :                      mail to : nicolas.pierre@icloud.com

 */

#include "../Utils/Utilities.h"



//Get time took

long getTimeTook( struct timeval& pStart, bool pMili )
{
    struct timeval end;
    long seconds( 0 ), useconds( 0 );

    gettimeofday( &end, 0 );
    seconds = end.tv_sec - pStart.tv_sec;
    useconds = end.tv_usec - pStart.tv_usec;

    if ( pMili )
        return ( long )( seconds * 1e3 + useconds / 1000 );

    else
        return ( long )( seconds * 1e6 + useconds );
}

//--------------------------------------------------------------------------
//Press enter function

void myflush( std::istream& in )
{
    in.ignore( std::numeric_limits<std::streamsize>::max(), '\n' );
    in.clear();
}

void mypause()
{
    std::cout << "Press [Enter] to continue ...";
    std::cin.get();
}

int kbhit()
{
    struct timeval tv;
    fd_set fds;
    tv.tv_sec = 0;
    tv.tv_usec = 0;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds); //STDIN_FILENO is 0
    select(STDIN_FILENO+1, &fds, NULL, NULL, &tv);
    return FD_ISSET(STDIN_FILENO, &fds);
}


const std::string currentDateTime()
{
    time_t now = time( 0 );
    struct tm tstruct;
    char buf[80];
    tstruct = *localtime( &now );
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime( buf, sizeof( buf ), "_%d-%m-%y_%H:%M", &tstruct );

    return buf;
}

double MyErf( double* x, double* par )
{
    double x0 = par[0];
    double width = par[1];
    double fitval( 0 );
    // if ( x[0] < x0 ) fitval = 0.5 * TMath::Erfc( ( x0 - x[0] ) / width );
    // else fitval = 0.5 + 0.5 * TMath::Erf( ( x[0] - x0 ) / width );
    if ( x[0] < x0 ) fitval = 0.5 * erfc( ( x0 - x[0] ) / width );
    else fitval = 0.5 + 0.5 * erf( ( x[0] - x0 ) / width );
    return fitval;
}

uint32_t convertAnyInt( const char* pRegValue )
{
    if ( std::string( pRegValue ).find( "0x" ) != std::string::npos ) return static_cast<uint32_t>( strtoul( pRegValue , 0, 16 ) );
    else return static_cast<uint32_t>( strtoul( pRegValue , 0, 10 ) );

}

std::vector<uint64_t> expandto64(const std::vector<uint32_t>& pVector)
{
//first, pack the 32 bit words into 64 bit words
        std::vector<uint64_t> cSlinkData;

        for (uint32_t cIndex = 0; cIndex < floor (pVector.size() / 2.); cIndex++ )
{
 	    //std::cout << cIndex << " ### " << 2*cIndex << " ### " << 2*cIndex +1 <<std::hex << " : "  << pVector.at(2*cIndex) << " | " << pVector.at(2*cIndex+1) << " ############################ " <<  std::dec << std::endl;
            cSlinkData.push_back(((uint64_t)pVector.at (2 * cIndex)) << 32 | pVector.at (2 * cIndex + 1));
}
        return cSlinkData;
}

void verifyImageName( const std::string& strImage, const std::vector<std::string>& lstNames)
{
    if (lstNames.empty())
    {
        if (strImage.compare("1") != 0 && strImage.compare("2") != 0)
        {
            std::cout << "Error, invalid image name, should be 1 (golden) or 2 (user)" << std::endl;
            exit(1);
        }
    }
    else
    {
        bool bFound = false;
        for (int iName = 0; iName < lstNames.size(); iName++)
        {
            if (!strImage.compare(lstNames[iName]))
            {
                bFound = true;
                std::cout << "Found Image " << strImage << " in list of image names on this SD card!" << std::endl;
                break;
            }
            //else std::cout << strImage << "!=" << lstNames[iName] << std::endl;
        }
        if (!bFound)
        {
            std::cout << "Error, this image name: " << strImage << " is not available on SD card" << std::endl;
            exit(1);
        }
    }
}
