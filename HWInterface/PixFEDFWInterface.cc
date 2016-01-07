/*

        FileName :                    PixFEDFWInterface.cc
        Content :                     PixFEDFWInterface base class
        Programmer :                  G.Auzinger
        Version :                     1.0
        Date of creation :            30/11/15
        Support :                     mail to : georg.auzinger@SPAMNOTcern.ch

 */



#include "PixFEDFWInterface.h"
#include "FpgaConfig.h"
#include <iomanip>

#define DEV_FLAG         0

//Constructor, makes the board map
PixFEDFWInterface::PixFEDFWInterface( const char* puHalConfigFileName, uint32_t pBoardId ) :
    RegManager( puHalConfigFileName, pBoardId ),
    fNumAcq( 0 )
    //fData( nullptr )
{
}
//Constructor, makes the board map
PixFEDFWInterface::PixFEDFWInterface( const char* pId, const char* pUri, const char* pAddressTable ) :
    RegManager( pId, pUri, pAddressTable ),
    fNumAcq( 0 )
{
}
/////////////////////////////////////////////
// CONFIG / START / STOP METHODS
////////////////////////////////////////////

std::string PixFEDFWInterface::getBoardType()
{
    // adapt me!
    std::string cBoardTypeString;

    uhal::ValWord<uint32_t> cBoardType = ReadReg( "board_id" );

    char cChar = ( ( cBoardType & 0xFF000000 ) >> 24 );
    cBoardTypeString.push_back( cChar );

    cChar = ( ( cBoardType & 0x00FF0000 ) >> 16 );
    cBoardTypeString.push_back( cChar );

    cChar = ( ( cBoardType & 0x0000FF00 ) >> 8 );
    cBoardTypeString.push_back( cChar );

    cChar = ( cBoardType & 0x000000FF );
    cBoardTypeString.push_back( cChar );

    return cBoardTypeString;

}

void PixFEDFWInterface::getFEDNetworkParameters()
{
    std::cout << "MAC & IP Source: " << ReadReg("mac_ip_source") << std::endl;

    std::cout << "MAC Address: " << std::hex << ReadReg("mac_b5") << ":" << ReadReg("mac_b4") << ":" << ReadReg("mac_b3") << ":" << ReadReg("mac_b2") << ":" << ReadReg("mac_b1") << ":" << ReadReg("mac_b0") << std::dec << std::endl;
}

void PixFEDFWInterface::getBoardInfo()
{
    std::cout << std::endl << "Board Type: " << getBoardType() << std::endl;
    getFEDNetworkParameters();
    std::string cBoardTypeString;

    uhal::ValWord<uint32_t> cBoardType = ReadReg( "pixfed_stat_regs.user_ascii_code_01to04" );

    char cChar = ( ( cBoardType & 0xFF000000 ) >> 24 );
    cBoardTypeString.push_back( cChar );

    cChar = ( ( cBoardType & 0x00FF0000 ) >> 16 );
    cBoardTypeString.push_back( cChar );

    cChar = ( ( cBoardType & 0x0000FF00 ) >> 8 );
    cBoardTypeString.push_back( cChar );

    cChar = ( cBoardType & 0x000000FF );
    cBoardTypeString.push_back( cChar );

    cBoardType = ReadReg( "pixfed_stat_regs.user_ascii_code_05to08" );
    cChar = ( ( cBoardType & 0xFF000000 ) >> 24 );
    cBoardTypeString.push_back( cChar );

    cChar = ( ( cBoardType & 0x00FF0000 ) >> 16 );
    cBoardTypeString.push_back( cChar );

    cChar = ( ( cBoardType & 0x0000FF00 ) >> 8 );
    cBoardTypeString.push_back( cChar );

    cChar = ( ( cBoardType & 0x00000000 ) );
    cBoardTypeString.push_back( cChar );

    std::cout << "Board Use: " << cBoardTypeString << std::endl;

    std::cout << "FW version IPHC : " << ReadReg( "pixfed_stat_regs.user_iphc_fw_id.fw_ver_nb" ) << "." << ReadReg( "pixfed_stat_regs.user_iphc_fw_id.archi_ver_nb" ) << "; Date: " << ReadReg( "pixfed_stat_regs.user_iphc_fw_id.fw_ver_day" ) << "." << ReadReg( "pixfed_stat_regs.user_iphc_fw_id.fw_ver_month" ) << "." << ReadReg( "pixfed_stat_regs.user_iphc_fw_id.fw_ver_year" ) <<  std::endl;
    std::cout << "FW version HEPHY : " << ReadReg( "pixfed_stat_regs.user_hephy_fw_id.fw_ver_nb" ) << "." << ReadReg( "pixfed_stat_regs.user_hephy_fw_id.archi_ver_nb" ) << "; Date: " << ReadReg( "pixfed_stat_regs.user_hephy_fw_id.fw_ver_day" ) << "." << ReadReg( "pixfed_stat_regs.user_hephy_fw_id.fw_ver_month" ) << "." << ReadReg( "pixfed_stat_regs.user_hephy_fw_id.fw_ver_year" ) << std::endl;


    std::cout << "FMC 8 Present : " << ReadReg( "status.fmc_l8_present" ) << std::endl;
    std::cout << "FMC 12 Present : " << ReadReg( "status.fmc_l12_present" ) << std::endl << std::endl;
}

bool PixFEDFWInterface::ConfigureBoard( const PixFED* pPixFED )
{
    std::vector< std::pair<std::string, uint32_t> > cVecReg;

    std::chrono::milliseconds cPause( 200 );
    WriteReg( "pixfed_ctrl_regs.PC_CONFIG_OK", 0 );
    //Primary Configuration
    cVecReg.push_back( {"pixfed_ctrl_regs.PC_CONFIG_OK", 0} );
    cVecReg.push_back( {"pixfed_ctrl_regs.INT_TRIGGER_EN", 0} );
    cVecReg.push_back( {"pixfed_ctrl_regs.rx_index_sel_en", 0} );

    cVecReg.push_back( {"pixfed_ctrl_regs.DDR0_end_readout", 0} );
    cVecReg.push_back( {"pixfed_ctrl_regs.DDR1_end_readout", 0} );

    cVecReg.push_back( {"pixfed_ctrl_regs.CMD_START_BY_PC", 0} );

    // fitel I2C bus reset & fifo TX & RX reset
    cVecReg.push_back({"pixfed_ctrl_regs.fitel_i2c_cmd_reset", 1});


    // the FW needs to be aware of the true 32 bit workd Block size for some reason! This is the Packet_nb_true in the python script?!
    computeBlockSize();
    cVecReg.push_back( {"pixfed_ctrl_regs.PACKET_NB", ( fBlockSize32 - 1 )  } );

    WriteStackReg( cVecReg );

    PixFEDRegMap cPixFEDRegMap = pPixFED->getPixFEDRegMap();
    for ( auto const& it : cPixFEDRegMap )
    {
        cVecReg.push_back( {it.first, it.second} );
    }

    WriteStackReg( cVecReg );

    cVecReg.clear();

    cVecReg.push_back({"pixfed_ctrl_regs.fitel_i2c_cmd_reset", 0});
    cVecReg.push_back({"pixfed_ctrl_regs.fitel_config_req", 0});
    cVecReg.push_back({"pixfed_ctrl_regs.fitel_i2c_addr", 0x4d});
    cVecReg.push_back( {"pixfed_ctrl_regs.PC_CONFIG_OK", 1} );
    WriteStackReg( cVecReg );

    cVecReg.clear();


    std::this_thread::sleep_for( cPause );

    // read FITEL I2C address
    std::cout << "FITEL I2C address: " << ReadReg("pixfed_ctrl_regs.fitel_i2c_addr") << std::endl;
    // Read back the DDR3 calib done flag
    bool cDDR3calibrated = ( ReadReg( "pixfed_stat_regs.ddr3_init_calib_done" ) & 0x00000001 );
    if ( cDDR3calibrated ) std::cout << "DDR3 calibrated, board configured!" << std::endl;
    return cDDR3calibrated;

}

void PixFEDFWInterface::Start()
{
    //set fNthAcq to 0 since I am starting from scratch and thus start with DDR0
    fNthAcq = 0;
    std::vector< std::pair<std::string, uint32_t> > cVecReg;
    cVecReg.push_back( {"pixfed_ctrl_regs.CMD_START_BY_PC", 1} );
    cVecReg.push_back( {"pixfed_ctrl_regs.INT_TRIGGER_EN", 1} );

    WriteStackReg( cVecReg );
}

void PixFEDFWInterface::Stop()
{
    //Stop the DAQ
    std::vector< std::pair<std::string, uint32_t> > cVecReg;
    cVecReg.push_back( {"pixfed_ctrl_regs.CMD_START_BY_PC", 0} );
    cVecReg.push_back( {"pixfed_ctrl_regs.INT_TRIGGER_EN", 0} );
    cVecReg.push_back( {"pixfed_ctrl_regs.PC_CONFIG_OK", 0} );
    cVecReg.push_back( {"pixfed_ctrl_regs.rx_index_sel_en", 0} );

    WriteStackReg( cVecReg );
    cVecReg.clear();

    std::chrono::milliseconds cWait( 100 );

    //Wait for the selected SRAM to be full then empty it
    //Select SRAM
    uhal::ValWord<uint32_t> cVal;

    SelectDaqDDR( fNthAcq );
    do
    {
        cVal = ReadReg( fStrFull );

        if ( cVal == 1 )
            std::this_thread::sleep_for( cWait );

    }
    while ( cVal == 1 );

    WriteReg( fStrReadout, 0 );
    fNthAcq++;
}


void PixFEDFWInterface::Pause()
{
    WriteReg( "pixfed_ctrl_regs.INT_TRIGGER_EN", 0 );
}


void PixFEDFWInterface::Resume()
{
    WriteReg( "pixfed_ctrl_regs.INT_TRIGGER_EN", 1 );
}

std::vector<uint32_t> PixFEDFWInterface::ReadData( PixFED* pPixFED )
{

    std::chrono::milliseconds cWait( 10 );
    // the fNthAcq variable is automatically used to determine which DDR FIFO to read - so it has to be incremented in this method!
    // first find which DDR bank to read

    SelectDaqDDR( fNthAcq );
    //std::cout << "Querying " << fStrDDR << " for FULL condition!" << std::endl;

    uhal::ValWord<uint32_t> cVal;
    do
    {
        cVal = ReadReg( fStrFull );
        if ( cVal == 0 ) std::this_thread::sleep_for( cWait );
    }
    while ( cVal == 0 );
    //std::cout << fStrDDR << " full: " << ReadReg( fStrFull ) << std::endl;

    // DDR control: 0 = ipbus, 1 = user
    WriteReg( fStrDDRControl, 0 );
    std::this_thread::sleep_for( cWait );
    //std::cout << "Starting block read of " << fStrDDR << std::endl;

    std::vector<uint32_t> cData = ReadBlockRegValue( fStrDDR, fBlockSize );
    WriteReg( fStrDDRControl , 1 );
    std::this_thread::sleep_for( cWait );
    WriteReg( fStrReadout, 1 );
    std::this_thread::sleep_for( cWait );

    // full handshake between SW & FW
    while ( ReadReg( fStrFull ) == 1 )
        std::this_thread::sleep_for( cWait );
    WriteReg( fStrReadout, 0 );

    //now I need to do something with the Data that I read into cData

    //for ( auto& cWord : cData )
    //{
    //std::cout << std::hex << std::setw(8) << std::setfill('0');
    //std::cout << cWord << std::dec << std::endl;
    //}

    fNthAcq++;
    return cData;
}

uint32_t PixFEDFWInterface::computeBlockSize( )
{
    // this is the number of bits to read from DDR
    fBlockSize = fNTBM * fNCh * fNPattern * fPacketSize;
    // since the DDR data widt is 256 this is the number of 32 bit words I have to read
    fBlockSize32 = static_cast<uint32_t>( fBlockSize / 8 );
    return fBlockSize;
}

std::vector<uint32_t> PixFEDFWInterface::ReadBlockRegValue( const std::string& pRegNode, const uint32_t& pBlocksize )
{
    uhal::ValVector<uint32_t> valBlock = ReadBlockReg( pRegNode, pBlocksize );
    std::vector<uint32_t> vBlock = valBlock.value();
    return vBlock;
}

bool PixFEDFWInterface::WriteBlockReg( const std::string& pRegNode, const std::vector< uint32_t >& pValues )
{
    bool cWriteCorr = RegManager::WriteBlockReg( pRegNode, pValues );
    return cWriteCorr;
}

void PixFEDFWInterface::SelectDaqDDR( uint32_t pNthAcq )
{
    fStrDDR  = ( ( pNthAcq % 2 + 1 ) == 1 ? "DDR0" : "DDR1" );
    fStrDDRControl = ( ( pNthAcq % 2 + 1 ) == 1 ? "pixfed_ctrl_regs.DDR0_ctrl_sel" : "pixfed_ctrl_regs.DDR1_ctrl_sel" );
    fStrFull = ( ( pNthAcq % 2 + 1 ) == 1 ? "pixfed_stat_regs.DDR0_full" : "pixfed_stat_regs.DDR1_full" );
    fStrReadout = ( ( pNthAcq % 2 + 1 ) == 1 ? "pixfed_ctrl_regs.DDR0_end_readout" : "pixfed_ctrl_regs.DDR1_end_readout" );
}


/////////////////////////////////////////////
// FITEL METHODS
//Methods for Fitels:
////////////////////////////////////////////
// Fix ME!
void PixFEDFWInterface::EncodeReg( const FitelRegItem& pRegItem, uint8_t pFMCId, uint8_t pFitelId, std::vector<uint32_t>& pVecReq )
{
    pVecReq.push_back(  pFMCId  << 24 |  pFitelId << 20 |  pRegItem.fAddress << 8 | pRegItem.fValue );
}


void PixFEDFWInterface::DecodeReg( FitelRegItem& pRegItem, uint8_t pFMCId, uint8_t pFitelId, uint32_t pWord )
{
    uint8_t cFMCId = ( pWord & 0xff000000 ) >> 24;
    pFitelId = (  pWord & 0x00f00000   ) >> 20;
    pRegItem.fAddress = ( pWord & 0x0000ff00 ) >> 8;
    pRegItem.fValue = pWord & 0x000000ff;
    //std::cout << "FMCID " << +(cFMCId) << " pFitelID " << +(pFitelId) << std::endl;
}

void PixFEDFWInterface::i2cRelease(uint32_t pTries)
{
    uint32_t cCounter = 0;
    // release
    WriteReg("pixfed_ctrl_regs.fitel_config_req", 0);
    while (ReadReg("pixfed_stat_regs.fitel_config_ack") != 0)
    {
        if (cCounter > pTries)
        {
            std::cout << "Error, exceeded maximum number of tries for I2C release!" << std::endl;
            break;
        }
        else
        {
            usleep(100);
            cCounter++;
        }
    }
}

bool PixFEDFWInterface::polli2cAcknowledge(uint32_t pTries)
{
    bool cSuccess = false;
    uint32_t cCounter = 0;
    // wait for command acknowledge
    while (ReadReg("pixfed_stat_regs.fitel_config_ack") == 0)
    {
        if (cCounter > pTries)
        {
            std::cout << "Error, polling for I2C command acknowledge timed out!" << std::endl;
            break;

        }
        else
        {
            usleep(100);
            cCounter++;
        }
    }

    // check the value of that register
    if (ReadReg("pixfed_stat_regs.fitel_config_ack") == 1)
    {
        cSuccess = true;
    }
    else if (ReadReg("pixfed_stat_regs.fitel_config_ack") == 3)
    {
        cSuccess = false;
    }
    return cSuccess;
}

bool PixFEDFWInterface::WriteFitelBlockReg(std::vector<uint32_t>& pVecReq)
{
    bool cSuccess = false;
    // write the encoded registers in the tx fifo
    WriteBlockReg("fitel_config_fifo_tx", pVecReq);
    // sent an I2C write request
    WriteReg("pixfed_ctrl_regs.fitel_config_req", 1);

    // wait for command acknowledge
    while (ReadReg("pixfed_stat_regs.fitel_config_ack") == 0) usleep(100);

    if (ReadReg("pixfed_stat_regs.fitel_config_ack") == 1)
    {
        cSuccess = true;
    }
    else if (ReadReg("pixfed_stat_regs.fitel_config_ack") == 3)
    {
        std::cout << "Error writing Registers!" << std::endl;
        cSuccess = false;
    }

    // release
    i2cRelease(10);
    return cSuccess;
}

bool PixFEDFWInterface::ReadFitelBlockReg(std::vector<uint32_t>& pVecReq)
{
    bool cSuccess = false;
    //uint32_t cVecSize = pVecReq.size();

    // write the encoded registers in the tx fifo
    WriteBlockReg("fitel_config_fifo_tx", pVecReq);
    // sent an I2C write request
    WriteReg("pixfed_ctrl_regs.fitel_config_req", 3);

    // wait for command acknowledge
    while (ReadReg("pixfed_stat_regs.fitel_config_ack") == 0) usleep(100);

    if (ReadReg("pixfed_stat_regs.fitel_config_ack") == 1)
    {
        cSuccess = true;
    }
    else if (ReadReg("pixfed_stat_regs.fitel_config_ack") == 3)
    {
        cSuccess = false;
        std::cout << "Error reading registers!" << std::endl;
    }

    // release
    i2cRelease(10);

    // clear the vector & read the data from the fifo
    pVecReq = ReadBlockRegValue("fitel_config_fifo_rx", pVecReq.size());
    return cSuccess;
}

/////////////////////////////////////////////
// FIRMWARE METHODS
//Methods for FW upload:
////////////////////////////////////////////


void PixFEDFWInterface::FlashProm( const std::string & strConfig, const char* pstrFile )
{
    checkIfUploading();

    fpgaConfig->runUpload( strConfig, pstrFile );
}

void PixFEDFWInterface::JumpToFpgaConfig( const std::string & strConfig )
{
    checkIfUploading();

    fpgaConfig->jumpToImage( strConfig );
}

std::vector<std::string> PixFEDFWInterface::getFpgaConfigList()
{
    checkIfUploading();
    return fpgaConfig->getFirmwareImageNames( );
}

void PixFEDFWInterface::DeleteFpgaConfig( const std::string & strId )
{
    checkIfUploading();
    fpgaConfig->deleteFirmwareImage( strId );
}

void PixFEDFWInterface::DownloadFpgaConfig( const std::string& strConfig, const std::string& strDest)
{
    checkIfUploading();
    fpgaConfig->runDownload( strConfig, strDest.c_str());
}

void PixFEDFWInterface::checkIfUploading()
{
    if ( fpgaConfig && fpgaConfig->getUploadingFpga() > 0 )
        throw Exception( "This board is uploading an FPGA configuration" );

    if ( !fpgaConfig )
        fpgaConfig = new CtaFpgaConfig( this );
}
