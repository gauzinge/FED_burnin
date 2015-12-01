/*!

        Filename :                              Cbc.cc
        Content :                               Cbc Description class, config of the Cbcs
        Programmer :                    Lorenzo BIDEGAIN
        Version :               1.0
        Date of Creation :              25/06/14
        Support :                               mail to : lorenzo.bidegain@gmail.com

 */

#include "Cbc.h"
#include <fstream>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <string.h>
#include <iomanip>
#include "Definition.h"


namespace Ph2_HwDescription
{
	// C'tors with object FE Description

	Cbc::Cbc( const FrontEndDescription& pFeDesc, uint8_t pCbcId, const std::string& filename ): FrontEndDescription( pFeDesc ),
		fCbcId( pCbcId )

	{
		loadfRegMap( filename );
	}

	// C'tors which take ShelveID, BeId, FMCId, FeID, CbcId

	Cbc::Cbc( uint8_t pShelveId, uint8_t pBeId, uint8_t pFMCId, uint8_t pFeId, uint8_t pCbcId, const std::string& filename ) : FrontEndDescription( pShelveId, pBeId, pFMCId, pFeId ), fCbcId( pCbcId )

	{
		loadfRegMap( filename );
	}

	// Copy C'tor

	Cbc::Cbc( const Cbc& cbcobj ) : FrontEndDescription( cbcobj ),
		fCbcId( cbcobj.fCbcId ),
		fRegMap( cbcobj.fRegMap )
	{
	}


	// D'Tor

	Cbc::~Cbc()
	{

	}

	//load fRegMap from file

	void Cbc::loadfRegMap( const std::string& filename )
	{
		std::ifstream file( filename.c_str(), std::ios::in );

		if ( file )
		{
			std::string line, fName, fPage_str, fAddress_str, fDefValue_str, fValue_str;
			CbcRegItem fRegItem;

			while ( getline( file, line ) )
			{
				if ( line.find_first_not_of( " \t" ) == std::string::npos ) continue;
				if ( line.at( 0 ) == '#' || line.at( 0 ) == '*' ) continue;
				std::istringstream input( line );
				input >> fName >> fPage_str >> fAddress_str >> fDefValue_str >> fValue_str;

				fRegItem.fPage = strtoul( fPage_str.c_str(), 0, 16 );
				fRegItem.fAddress = strtoul( fAddress_str.c_str(), 0, 16 );
				fRegItem.fDefValue = strtoul( fDefValue_str.c_str(), 0, 16 );
				fRegItem.fValue = strtoul( fValue_str.c_str(), 0, 16 );

				fRegMap[fName] = fRegItem;
			}

			file.close();
		}
		else
			std::cerr << "The CBC Settings File " << filename << " could not be opened!" << std::endl;
	}


	uint8_t Cbc::getReg( const std::string& pReg ) const
	{
		CbcRegMap::const_iterator i = fRegMap.find( pReg );
		if ( i == fRegMap.end() )
		{
			std::cout << "The Cbc object: " << +fCbcId << " doesn't have " << pReg << std::endl;
			return 0;
		}
		else
			return i->second.fValue;
	}


	void Cbc::setReg( const std::string& pReg, uint8_t psetValue )
	{
		CbcRegMap::iterator i = fRegMap.find( pReg );
		if ( i == fRegMap.end() )
			std::cout << "The Cbc object: " << +fCbcId << " doesn't have " << pReg << std::endl;
		else
			i->second.fValue = psetValue;
	}

	CbcRegItem Cbc::getRegItem( const std::string& pReg )
	{
		CbcRegItem cItem;
		CbcRegMap::iterator i = fRegMap.find( pReg );
		if ( i != std::end( fRegMap ) ) return ( i->second );
		else
		{
			std::cerr << "Error, no Register " << pReg << " found in the RegisterMap of CBC " << +fCbcId << "!" << std::endl;
			throw Exception( "Cbc: no matching register found" );
			return cItem;
		}
	}


	//Write RegValues in a file

	void Cbc::saveRegMap( const std::string& filename )
	{

		std::ofstream file( filename.c_str(), std::ios::out | std::ios::trunc );

		if ( file )
		{
			file << "* RegName";
			for ( int j = 0; j < 48; j++ )
				file << " ";
			file.seekp( -strlen( "* RegName" ), std::ios_base::cur );

			file << "Page\tAddr\tDefval\tValue" << std::endl;
			file << "*--------------------------------------------------------------------------------" << std::endl;

			std::set<CbcRegPair, RegItemComparer> fSetRegItem;

			for ( auto& it : fRegMap )
				fSetRegItem.insert( {it.first, it.second} );

			for ( const auto& v : fSetRegItem )
			{

				file << v.first;
				for ( int j = 0; j < 48; j++ )
					file << " ";
				file.seekp( -v.first.size(), std::ios_base::cur );


				file << "0x" << std::setfill( '0' ) << std::setw( 2 ) << std::hex << std::uppercase << int( v.second.fPage ) << "\t0x" << std::setfill( '0' ) << std::setw( 2 ) << std::hex << std::uppercase << int( v.second.fAddress ) << "\t0x" << std::setfill( '0' ) << std::setw( 2 ) << std::hex << std::uppercase << int( v.second.fDefValue ) << "\t0x" << std::setfill( '0' ) << std::setw( 2 ) << std::hex << std::uppercase << int( v.second.fValue ) << std::endl;

			}
			file.close();
		}
		else
			std::cerr << "Error opening file" << std::endl;
	}




	bool CbcComparer::operator()( const Cbc& cbc1, const Cbc& cbc2 ) const
	{
		if ( cbc1.getShelveId() != cbc2.getShelveId() ) return cbc1.getShelveId() < cbc2.getShelveId();
		else if ( cbc1.getBeId() != cbc2.getBeId() ) return cbc1.getBeId() < cbc2.getBeId();
		else if ( cbc1.getFMCId() != cbc2.getFMCId() ) return cbc1.getFMCId() < cbc2.getFMCId();
		else if ( cbc1.getFeId() != cbc2.getFeId() ) return cbc1.getFeId() < cbc2.getFeId();
		else return cbc1.getCbcId() < cbc2.getCbcId();
	}


	bool RegItemComparer::operator()( const CbcRegPair& pRegItem1, const CbcRegPair& pRegItem2 ) const
	{
		if ( pRegItem1.second.fPage != pRegItem2.second.fPage )
			return pRegItem1.second.fPage < pRegItem2.second.fPage;
		else return pRegItem1.second.fAddress < pRegItem2.second.fAddress;
	}

}
