/*

        FileName :                    PixFEDInterface.cc
        Content :                     User Interface to the Boards
        Programmer :                  Lorenzo BIDEGAIN, Nicolas PIERRE
        Version :                     1.0
        Date of creation :            31/07/14
        Support :                     mail to : lorenzo.bidegain@gmail.com nico.pierre@icloud.com

 */

#include "PixFEDInterface.h"


PixFEDInterface::PixFEDInterface( const PixFEDFWMap& pFWMap ) :
	fFWMap( pFWMap ),
	fFEDFW( nullptr ),
	prevBoardIdentifier( 65535 )

{
}



PixFEDInterface::~PixFEDInterface()
{

}

void PixFEDInterface::setBoard( uint16_t pBoardIdentifier )
{
	if ( prevBoardIdentifier != pBoardIdentifier )
	{
		PixFEDFWMap::iterator i = fFWMap.find( pBoardIdentifier );
		if ( i == fFWMap.end() )
			std::cout << "The FED: " << +pBoardIdentifier  <<  "  doesn't exist" << std::endl;
		else
		{
			fFEDFW = i->second;
			prevBoardIdentifier = pBoardIdentifier;
		}
	}
}

void PixFEDInterface::WriteBoardReg( PixFED* pFED, const std::string& pRegNode, const uint32_t& pVal )
{
	setBoard( pFED->getBeId() );

	fFEDFW->WriteReg( pRegNode, pVal );
	pFED->setReg( pRegNode, pVal );
}

void PixFEDInterface::WriteBlockBoardReg( PixFED* pFED, const std::string& pRegNode, const std::vector<uint32_t>& pValVec )
{
	setBoard( pFED->getBeId() );
	fFEDFW->WriteBlockReg( pRegNode, pValVec );
}


void PixFEDInterface::WriteBoardMultReg( PixFED* pFED, const std::vector < std::pair< std::string , uint32_t > >& pRegVec )
{
	setBoard( pFED->getBeId() );

	fFEDFW->WriteStackReg( pRegVec );

	for ( const auto& cReg : pRegVec )
	{
		// fFEDFW->WriteReg( cReg.first, cReg.second );
		pFED->setReg( cReg.first, cReg.second );
	}
}


uint32_t PixFEDInterface::ReadBoardReg( PixFED* pFED, const std::string& pRegNode )
{
	setBoard( pFED->getBeId() );
	uint32_t cRegValue = static_cast<uint32_t>( fFEDFW->ReadReg( pRegNode ) );
	pFED->setReg( pRegNode,  cRegValue );
	return cRegValue;
}

void PixFEDInterface::ReadBoardMultReg( PixFED* pFED, std::vector < std::pair< std::string , uint32_t > >& pRegVec )
{
	setBoard( pFED->getBeId() );

	for ( auto& cReg : pRegVec )
	{
		cReg.second = static_cast<uint32_t>( fFEDFW->ReadReg( cReg.first ) );
		pFED->setReg( cReg.first, cReg.second );
	}
}

std::vector<uint32_t> PixFEDInterface::ReadBlockBoardReg( PixFED* pFED, const std::string& pRegNode, uint32_t pSize )
{
	setBoard( pFED->getBeId() );
	return fFEDFW->ReadBlockRegValue( pRegNode, pSize );
}

void PixFEDInterface::getBoardInfo( const PixFED* pFED )
{
	setBoard( pFED->getBeId() );
	fFEDFW->getBoardInfo();
}

void PixFEDInterface::ConfigureFED( const PixFED* pFED )
{
	setBoard( pFED->getBeId() );
	fFEDFW->ConfigureBoard( pFED );
}


void PixFEDInterface::Start( PixFED* pFED )
{
	setBoard( pFED->getBeId() );
	fFEDFW->Start();
}


void PixFEDInterface::Stop( PixFED* pFED )
{
	setBoard( pFED->getBeId() );
	fFEDFW->Stop();
}


void PixFEDInterface::Pause( PixFED* pFED )
{
	setBoard( pFED->getBeId() );
	fFEDFW->Pause();
}


void PixFEDInterface::Resume( PixFED* pFED )
{
	setBoard( pFED->getBeId() );
	fFEDFW->Resume();
}


uint32_t PixFEDInterface::ReadData( PixFED* pFED )
{
	setBoard( pFED->getBeId() );
	fFEDFW->ReadData( pFED );
}

// const Event* PixFEDInterface::GetNextEvent( const PixFED* pFED )
// {
// 	setBoard( pFED->getBeId() );
// 	fFEDFW->GetNextEvent( pFED );
// }

// const Event* PixFEDInterface::GetEvent( const PixFED* pFED, int i )
// {
// 	setBoard( pFED->getBeId() );
// 	return fFEDFW->GetEvent( pFED, i );
// }
// const std::vector<Event*>& PixFEDInterface::GetEvents( const PixFED* pFED )
// {
// 	setBoard( pFED->getBeId() );
// 	return fFEDFW->GetEvents( pFED );
// }

const uhal::Node& PixFEDInterface::getUhalNode( const PixFED* pFED, const std::string& pStrPath )
{
	setBoard( pFED->getBeId() );
	return fFEDFW->getUhalNode( pStrPath );
}

uhal::HwInterface* PixFEDInterface::getHardwareInterface( const PixFED* pFED )
{
	setBoard( pFED->getBeId() );
	return fFEDFW->getHardwareInterface();
}


void PixFEDInterface::FlashProm( PixFED* pFED, const std::string& strConfig, const char* pstrFile )
{
	setBoard( pFED->getBeId() );
	fFEDFW->FlashProm( strConfig, pstrFile );
}

void PixFEDInterface::JumpToFpgaConfig( PixFED* pFED, const std::string& strConfig )
{
	setBoard( pFED->getBeId() );
	fFEDFW->JumpToFpgaConfig( strConfig );
}

// const FpgaConfig* PixFEDInterface::getConfiguringFpga( PixFED* pFED )
// {
// 	setBoard( pFED->getBeId() );
// 	return fFEDFW->getConfiguringFpga();
// }

std::vector<std::string> PixFEDInterface::getFpgaConfigList( PixFED* pFED )
{
	setBoard( pFED->getBeId() );
	return fFEDFW->getFpgaConfigList();
}

void PixFEDInterface::DeleteFpgaConfig( PixFED* pFED, const std::string& strId )
{
	setBoard( pFED->getBeId() );
	fFEDFW->DeleteFpgaConfig( strId );
}
