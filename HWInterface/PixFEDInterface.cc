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

//////////////////////////
//FITEL METHODS
/////////////////////////

void PixFEDInterface::ConfigureFitel( const Fitel* pFitel, bool pVerifLoop )
{
    setBoard( pFitel->getBeId() );

    FitelRegMap cFitelRegMap = pFitel->getRegMap();
    FitelRegMap::iterator cIt = cFitelRegMap.begin();

    while ( cIt != cFitelRegMap.end() )
    {
        std::vector<uint32_t> cVecWrite;
        std::vector<uint32_t> cVecRead;

        uint32_t cCounter = 0;

        for ( cIt; cIt != cFitelRegMap.end(); cIt++ )
        {
            if (cIt->second.fPermission == 'w')
            {
                EncodeFitelReg( cIt->second, pFitel->getFMCId(), pFitel->getFitelId(), cVecWrite );

                if ( pVerifLoop )
                {
                    FitelRegItem cItem = cIt->second;
                    cItem.fValue = 0;

                    EncodeFitelReg( cItem, pFitel->getFMCId(), pFitel->getFitelId(), cVecRead );
                }
#ifdef COUNT_FLAG
                fRegisterCount++;
#endif
                cCounter++;

            }
        }

        fFEDFW->WriteFitelBlockReg(  cVecWrite );
        //usleep(20000);
#ifdef COUNT_FLAG
        fTransactionCount++;
#endif
        if ( pVerifLoop )
        {
            uint8_t cFMCId = pFitel->getFMCId();
            uint8_t cFitelId = pFitel->getFitelId();

            fFEDFW->ReadFitelBlockReg( cVecRead );

            // only if I have a mismatch will i decode word by word and compare
            if ( cVecWrite != cVecRead )
            {
                bool cAllgood = false;
                int cIterationCounter = 1;
                while ( !cAllgood )
                {
                    if ( cAllgood ) break;

                    std::vector<uint32_t> cWrite_again;
                    std::vector<uint32_t> cRead_again;

                    auto cMismatchWord = std::mismatch( cVecWrite.begin(), cVecWrite.end(), cVecRead.begin() );

                    while ( cMismatchWord.first != cVecWrite.end() )
                    {


                        FitelRegItem cRegItemWrite;
                        DecodeFitelReg( cRegItemWrite, cFMCId, cFitelId, *cMismatchWord.first );
                        FitelRegItem cRegItemRead;
                        DecodeFitelReg( cRegItemRead, cFMCId, cFitelId, *cMismatchWord.second );

                        if ( cIterationCounter == 5 )
                        {
                            std::cout << RED << "\nERROR !!!\nReadback value not the same after 5 Iteration for Register @ Address: 0x" << std::hex << int( cRegItemWrite.fAddress ) << "\n" << "Written Value : 0x" << int( cRegItemWrite.fValue ) << "\nReadback Value : 0x" << int( cRegItemRead.fValue ) << std::dec << std::endl;
                            std::cout << "Fitel Id : " << int( pFitel->getFitelId() ) << RESET << std::endl << std::endl;
                            std::cout << BOLDRED << "Failed to write register in " << cIterationCounter << " trys! Giving up!" << RESET << std::endl;
                            std::cout << "---<-FMC<-fi---------<-a-----<-v" << std::endl;
                            std::cout << static_cast<std::bitset<32> >( *cMismatchWord.first ) << std::endl << static_cast<std::bitset<32> >( *cMismatchWord.second ) << std::endl << std::endl;
                        }

                        cMismatchWord = std::mismatch( ++cMismatchWord.first, cVecWrite.end(), ++cMismatchWord.second );

                        EncodeFitelReg( cRegItemWrite, cFMCId, cFitelId, cWrite_again );
                        cRegItemRead.fValue = 0;
                        EncodeFitelReg( cRegItemRead, cFMCId, cFitelId, cRead_again );
                    }

                    fFEDFW->WriteFitelBlockReg( cWrite_again );
                    fFEDFW->ReadFitelBlockReg( cRead_again );

                    if ( cWrite_again != cRead_again )
                    {
                        if ( cIterationCounter == 5 )
                        {
                            std::cout << "Failed to configure FITEL in " << cIterationCounter << " Iterations!" << std::endl;
                            break;
                        }
                        cVecWrite.clear();
                        cVecWrite = cWrite_again;
                        cVecRead.clear();
                        cVecRead = cRead_again;
                        cIterationCounter++;
                    }
                    else
                    {
                        std::cout << BLUE << "Managed to write all registers correctly in " << cIterationCounter << " Iteration(s)!" << RESET << std::endl;
                        cAllgood = true;
                    }
                }
            }
        }
    }
}



bool PixFEDInterface::WriteFitelReg(Fitel* pFitel, const std::string& pRegNode, uint8_t pValue, bool pVerifLoop)
{
    FitelRegItem cRegItem = pFitel->getRegItem( pRegNode );
    std::vector<uint32_t> cVecWrite;
    std::vector<uint32_t> cVecRead;

    cRegItem.fValue = pValue;

    setBoard( pFitel->getBeId() );

    EncodeFitelReg( cRegItem, pFitel->getFMCId(), pFitel->getFitelId(), cVecWrite );

    fFEDFW->WriteFitelBlockReg( cVecWrite );

#ifdef COUNT_FLAG
    fRegisterCount++;
    fTransactionCount++;
#endif

    pFitel->setReg( pRegNode, pValue );

    if ( pVerifLoop )
    {
        uint8_t cFMCId = pFitel->getFMCId();
        uint8_t cFitelId = pFitel->getFitelId();

        cRegItem.fValue = 0;

        EncodeFitelReg( cRegItem, pFitel->getFMCId(), pFitel->getFitelId(), cVecRead );

        fFEDFW->ReadFitelBlockReg( cVecRead );

        bool cAllgood = false;

        if ( cVecWrite != cVecRead )
        {
            int cIterationCounter = 1;
            while ( !cAllgood )
            {
                if ( cAllgood ) break;

                std::vector<uint32_t> cWrite_again;
                std::vector<uint32_t> cRead_again;

                FitelRegItem cReadItem;
                FitelRegItem cWriteItem;
                DecodeFitelReg( cWriteItem, cFMCId, cFitelId, cVecWrite.at( 0 ) );
                DecodeFitelReg( cReadItem, cFMCId, cFitelId, cVecRead.at( 0 ) );
                // pFitel->setReg( pRegNode, cReadItem.fValue );

                if ( cIterationCounter == 5 )
                {
                    std::cout << RED <<  "ERROR !!!\nReadback Value still different after 5 iterations for Register : " << pRegNode << "\n" << std::hex << "Written Value : 0x" << +pValue << "\nReadback Value : 0x" << int( cRegItem.fValue ) << std::dec << std::endl;
                    std::cout << "Register Adress : " << int( cRegItem.fAddress ) << std::endl;
                    std::cout << "Fitel Id : " << +cFitelId << RESET << std::endl << std::endl;
                    std::cout << BOLDRED << "Failed to write register in " << cIterationCounter << " trys! Giving up!" << RESET << std::endl;
                }

                EncodeFitelReg( cWriteItem, cFMCId, cFitelId, cWrite_again );
                cReadItem.fValue = 0;
                EncodeFitelReg( cReadItem, cFMCId, cFitelId, cRead_again );

                fFEDFW->WriteFitelBlockReg( cWrite_again );
                fFEDFW->ReadFitelBlockReg( cRead_again );

                if ( cWrite_again != cRead_again )
                {
                    if ( cIterationCounter == 5 ) break;
                    cVecWrite.clear();
                    cVecWrite = cWrite_again;
                    cVecRead.clear();
                    cVecRead = cRead_again;
                    cIterationCounter++;
                }
                else
                {
                    std::cout << BLUE << "Managed to write register correctly in " << cIterationCounter << " Iteration(s)!" << RESET << std::endl;
                    cAllgood = true;
                }
            }
        }
        else cAllgood = true;
        if ( cAllgood ) return true;
        else return false;
    }
    else return true;
}

uint8_t PixFEDInterface::ReadFitelReg( Fitel* pFitel, const std::string& pRegNode )
{
    FitelRegItem cRegItem = pFitel->getRegItem( pRegNode );
    std::vector<uint32_t> cVecReq;

    setBoard( pFitel->getBeId() );

    EncodeFitelReg( cRegItem, pFitel->getFMCId(), pFitel->getFitelId(), cVecReq );

    fFEDFW->ReadFitelBlockReg( cVecReq );

    DecodeFitelReg( cRegItem, pFitel->getFMCId(), pFitel->getFitelId(), cVecReq[0] );

    pFitel->setReg( pRegNode, cRegItem.fValue );
    return cRegItem.fValue;
}

std::vector<uint32_t> PixFEDInterface::ReadBlockBoardReg( PixFED* pFED, const std::string& pRegNode, uint32_t pSize )
{
    setBoard( pFED->getBeId() );
    return fFEDFW->ReadBlockRegValue( pRegNode, pSize );
}

///////////////
// Startup  Methods
//////////////

void PixFEDInterface::getBoardInfo( const PixFED* pFED )
{
    setBoard( pFED->getBeId() );
    fFEDFW->getBoardInfo();
}

void PixFEDInterface::enableFMCs( const PixFED* pFED )
{
    setBoard( pFED->getBeId() );
    fFEDFW->enableFMCs();
}

void PixFEDInterface::disableFMCs( const PixFED* pFED )
{
    setBoard( pFED->getBeId() );
    fFEDFW->disableFMCs();
}

void PixFEDInterface::ConfigureFED( const PixFED* pFED )
{
    setBoard( pFED->getBeId() );
    fFEDFW->ConfigureBoard( pFED );
}

///////////////
// Setup  Methods
//////////////

void PixFEDInterface::findPhases( const PixFED* pFED, uint32_t pScopeFIFOCh )
{
    setBoard( pFED->getBeId() );
    fFEDFW->findPhases(pScopeFIFOCh);
}


std::vector<uint32_t> PixFEDInterface::readTransparentFIFO( const PixFED* pFED )
{
    setBoard( pFED->getBeId() );
    return fFEDFW->readTransparentFIFO();
}

std::vector<uint32_t> PixFEDInterface::readSpyFIFO( const PixFED* pFED )
{
    setBoard( pFED->getBeId() );
    return fFEDFW->readSpyFIFO();
}

std::string PixFEDInterface::readFIFO1( const PixFED* pFED )
{
    setBoard( pFED->getBeId() );
    return fFEDFW->readFIFO1();
}

///////////////
// Readout  Methods
//////////////

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


std::vector<uint32_t> PixFEDInterface::ReadData( PixFED* pFED )
{
    setBoard( pFED->getBeId() );
    return fFEDFW->ReadData( pFED );
}


///////////////
// Auxillary  Methods
//////////////

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
//  setBoard( pFED->getBeId() );
//  return fFEDFW->getConfiguringFpga();
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
