/*!

        Filename :                              Fitel.cc
        Content :                               Fitel Description class, config of the Fitels
        Programmer :                    Lorenzo BIDEGAIN
        Version :               1.0
        Date of Creation :              25/06/14
        Support :                               mail to : lorenzo.bidegain@gmail.com

 */

#include "Fitel.h"
#include <fstream>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <string.h>
#include <iomanip>



// C'tors which take ShelveID, BeId, FMCId, FeID, FitelId

Fitel::Fitel( uint8_t pBeId, uint8_t pFMCId, uint8_t pFitelId, const std::string& filename ) : fBeId(pBeId), fFMCId(pFMCId), fFitelId( pFitelId )

{
    loadfRegMap( filename );
}

// Copy C'tor

Fitel::Fitel( const Fitel& Fitelobj ) :
    fBeId(Fitelobj.fBeId),
    fFMCId(Fitelobj.fFMCId),
    fFitelId( Fitelobj.fFitelId ),
    fRegMap( Fitelobj.fRegMap )
{
}


// D'Tor

Fitel::~Fitel()
{

}

//load fRegMap from file

void Fitel::loadfRegMap( const std::string& filename )
{
    std::ifstream file( filename.c_str(), std::ios::in );

    if ( file )
    {
        std::string line, fName, fAddress_str, fDefValue_str, fValue_str, fPermission_str;
        FitelRegItem fRegItem;

        while ( getline( file, line ) )
        {
            if ( line.find_first_not_of( " \t" ) == std::string::npos ) continue;
            if ( line.at( 0 ) == '#' || line.at( 0 ) == '*' ) continue;
            std::istringstream input( line );
            input >> fName >> fAddress_str >> fDefValue_str >> fValue_str >> fPermission_str;

            fRegItem.fAddress = strtoul(fAddress_str.c_str(), 0, 16);
            fRegItem.fDefValue = strtoul( fDefValue_str.c_str(), 0, 16 );
            fRegItem.fValue = strtoul( fValue_str.c_str(), 0, 16 );
            fRegItem.fPermission = fPermission_str.c_str()[0];

            //std::cout << fName << " "<< +fRegItem.fAddress << " " << +fRegItem.fDefValue << " " << +fRegItem.fValue << std::endl;
            fRegMap[fName] = fRegItem;
        }

        file.close();
    }
    else
        std::cerr << "The Fitel Settings File " << filename << " could not be opened!" << std::endl;
}


uint8_t Fitel::getReg( const std::string& pReg ) const
{
    FitelRegMap::const_iterator i = fRegMap.find( pReg );
    if ( i == fRegMap.end() )
    {
        std::cout << "The Fitel object: " << +fFitelId << " doesn't have " << pReg << std::endl;
        return 0;
    }
    else
        return i->second.fValue;
}


void Fitel::setReg( const std::string& pReg, uint8_t psetValue )
{
    FitelRegMap::iterator i = fRegMap.find( pReg );
    if ( i == fRegMap.end() )
        std::cout << "The Fitel object: " << +fFitelId << " doesn't have " << pReg << std::endl;
    else
        i->second.fValue = psetValue;
}

FitelRegItem Fitel::getRegItem( const std::string& pReg )
{
    FitelRegItem cItem;
    FitelRegMap::iterator i = fRegMap.find( pReg );
    if ( i != std::end( fRegMap ) ) return ( i->second );
    else
    {
        std::cerr << "Error, no Register " << pReg << " found in the RegisterMap of Fitel " << +fFitelId << "!" << std::endl;
        throw Exception( "Fitel: no matching register found" );
        return cItem;
    }
}


//Write RegValues in a file

void Fitel::saveRegMap( const std::string& filename )
{

    std::ofstream file( filename.c_str(), std::ios::out | std::ios::trunc );

    if ( file )
    {
        file << "* RegName";
        for ( int j = 0; j < 48; j++ )
            file << " ";
        file.seekp( -strlen( "* RegName" ), std::ios_base::cur );

        file << "Page\tDefval\tValue" << std::endl;
        file << "*--------------------------------------------------------------------------------" << std::endl;

        std::set<FitelRegPair, RegItemComparer> fSetRegItem;

        for ( auto& it : fRegMap )
            fSetRegItem.insert( {it.first, it.second} );

        for ( const auto& v : fSetRegItem )
        {

            file << v.first;
            for ( int j = 0; j < 48; j++ )
                file << " ";
            file.seekp( -v.first.size(), std::ios_base::cur );


            file << "0x" << std::setfill( '0' ) << std::setw( 2 ) << std::hex << std::uppercase << std::setfill( '0' ) << std::setw( 2 ) << std::hex << std::uppercase << int( v.second.fAddress ) << "\t0x" << std::setfill( '0' ) << std::setw( 2 ) << std::hex << std::uppercase << int( v.second.fDefValue ) << "\t0x" << std::setfill( '0' ) << std::setw( 2 ) << std::hex << std::uppercase << int( v.second.fValue ) << std::endl;

        }
        file.close();
    }
    else
        std::cerr << "Error opening file" << std::endl;
}




bool FitelComparer::operator()( const Fitel& fitel1, const Fitel& fitel2 ) const
{
    if ( fitel1.getBeId() != fitel2.getBeId() ) return fitel1.getBeId() < fitel2.getBeId();
    else if (fitel1.getFMCId() != fitel2.getFMCId()) return fitel1.getFMCId() < fitel2.getFMCId();
    else return fitel1.getFitelId() < fitel2.getFitelId();
}


bool RegItemComparer::operator()( const FitelRegPair& pRegItem1, const FitelRegPair& pRegItem2 ) const
{
    return pRegItem1.second.fAddress < pRegItem2.second.fAddress;
}
