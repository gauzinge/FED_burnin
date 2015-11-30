/*

        FileName :                    PixFEDFWInterface.cc
        Content :                     PixFEDFWInterface base class
        Programmer :                  G.Auzinger
        Version :                     1.0
        Date of creation :            30/11/15
        Support :                     mail to : georg.auzinger@cern.ch

 */



#include "PixFEDFWInterface.h"
#include "FpgaConfig.h"

#define DEV_FLAG         0

//Constructor, makes the board map
PixFEDFWInterface::PixFEDFWInterface( const char* puHalConfigFileName, uint32_t pBoardId ) :
	RegManager( puHalConfigFileName, pBoardId ),
	fNTotalAcq( 0 ),
	numAcq( 0 ),
	fData( nullptr )
{
}


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

void PixFEDFWInterface::getBoardInfo()
{
	std::cout << "Board Info: " << getBoardType() << std::endl;

	std::cout << ReadReg( "pixfed_stat_regs.user_ascii_code_01" ) << ReadReg( "pixfed_stat_regs.user_ascii_code_02" ) << ReadReg( "pixfed_stat_regs.user_ascii_code_03" ) << ReadReg( "pixfed_stat_regs.user_ascii_code_04" ) << ReadReg( "pixfed_stat_regs.user_ascii_code_05" ) << ReadReg( "pixfed_stat_regs.user_ascii_code_06" ) << ReadReg( "pixfed_stat_regs.user_ascii_code_07" ) << ReadReg( "pixfed_stat_regs.user_ascii_code_08" ) << std::endl;

	std::cout << "FW version IPHC : " << ReadReg( "pixfed_stat_regs.user_iphc_fw_id.fw_ver_nb" ) << ", " << ReadReg( "pixfed_stat_regs.user_iphc_fw_id.archi_ver_nb" ) << ", Date: " << ReadReg( "pixfed_stat_regs.user_iphc_fw_id.fw_ver_day" ) << "." << ReadReg( "pixfed_stat_regs.user_iphc_fw_id.fw_ver_month" ) << "." << ReadReg( "pixfed_stat_regs.user_iphc_fw_id.fw_ver_year" ) std::endl;
	std::cout << "FW version HEPHY : " << ReadReg( "pixfed_stat_regs.user_hephy_fw_id.fw_ver_nb" ) << ", " << ReadReg( "pixfed_stat_regs.user_hephy_fw_id.archi_ver_nb" ) << ", Date: " << ReadReg( "pixfed_stat_regs.user_hephy_fw_id.fw_ver_day" ) << "." << ReadReg( "pixfed_stat_regs.user_hephy_fw_id.fw_ver_month" ) << "." << ReadReg( "pixfed_stat_regs.user_hephy_fw_id.fw_ver_year" ) std::endl;


	std::cout << "FMC 8 Present : " << ReadReg( "status.fmc_l8_present" ) << std::endl;
	std::cout << "FMC 12 Present : " << ReadReg( "status.fmc_l12_present" ) << std::endl;
}

bool PixFEDFWInterface::ConfigureBoard( const PixFED* pPixFED )
{

	//We may here switch in the future with the StackReg method of the RegManager
	//when the timeout thing will be implemented in a transparent and pretty way

	std::vector< std::pair<std::string, uint32_t> > cVecReg;
	std::pair<std::string, uint32_t> cPairReg;

	std::chrono::milliseconds cPause( 200 );

	//Primary Configuration
	cVecReg.push_back( {"pixfed_ctrl_regs.PC_config_ok", 0} );
	cVecReg.push_back( {"pixfed_ctrl_regs.INT_TRIGGER_EN", 0} );
	cVecReg.push_back( {"pixfed_ctrl_regs.rx_index_sel_en", 0} );

	cVecReg.push_back( {"pixfed_ctrl_regs.DDR0_end_readout", 0} );
	cVecReg.push_back( {"pixfed_ctrl_regs.DDR1_end_readout", 0} );

	cVecReg.push_back( {"pixfed_ctrl_regs.CMD_START_BY_PC", 0} );

	WriteStackReg( cVecReg );

	cVecReg.clear();

	std::this_thread::sleep_for( cPause );

	/*
	        GlibRegMap : map<std::string,uint8_t> created from Glib class

	   Mandatory to go through a created cPixFEDRegMap.
	   If you want to put directly pGlib.getGlibRegMap(), you'll end up with
	   a seg fault error, as it is not putting all the map in mem but only
	   begin() and end().
	 */

	PixFEDRegMap cPixFEDRegMap = pPixFED->getPixFEDRegMap();
	for ( auto const& it : cPixFEDRegMap )
	{
		cPairReg.first = it.first;
		cPairReg.second = it.second;
		cVecReg.push_back( cPairReg );
	}

	WriteStackReg( cVecReg );

	cVecReg.clear();

	cVecReg.push_back( {"pixfed_ctrl_regs.PC_CONFIG_OK", 1} );
	WriteStackReg( cVecReg );

	cVecReg.clear();


	std::this_thread::sleep_for( cPause );

	// Read back the DDR3 calib done flag
	bool cDDR3calibrated = ( ReadReg( "pixfed_stat_regs.ddr3_init_calib_done" ) & 0x00000001 );

	if ( cDDR3calibrated ) std::cout << "DDR3 calibrated, board configured!" << std::endl;
	return cDDR3calibrated;

}

void PixFEDFWInterface::Start()
{
	std::vector< std::pair<std::string, uint32_t> > cVecReg;
	std::pair<std::string, uint32_t> cPairReg;

	//Starting the DAQ

	cPairReg.first = BREAK_TRIGGER;
	cPairReg.second = 0;
	cVecReg.push_back( cPairReg );
	cPairReg.first = PC_CONFIG_OK;
	cPairReg.second = 1;
	cVecReg.push_back( cPairReg );
	cPairReg.first = FORCE_BG0_START;
	cPairReg.second = 1;
	cVecReg.push_back( cPairReg );

	WriteStackReg( cVecReg );

	cVecReg.clear();

	// Since the Number of  Packets is a FW register, it should be read from the Settings Table which is one less than is actually read
	cNPackets = ReadReg( CBC_PACKET_NB ) + 1 ;

	//Wait for start acknowledge
	uhal::ValWord<uint32_t> cVal;
	std::chrono::milliseconds cWait( 100 );
	do
	{
		cVal = ReadReg( CMD_START_VALID );

		if ( cVal == 0 )
			std::this_thread::sleep_for( cWait );

	}
	while ( cVal == 0 );

}

void PixFEDFWInterface::Stop( uint32_t pNthAcq )
{

	std::vector< std::pair<std::string, uint32_t> > cVecReg;
	std::pair<std::string, uint32_t> cPairReg;

	uhal::ValWord<uint32_t> cVal;

	//Select SRAM
	SelectDaqSRAM( pNthAcq );

	//Stop the DAQ
	cPairReg.first = BREAK_TRIGGER;
	cPairReg.second = 1;
	cVecReg.push_back( cPairReg );
	cPairReg.first = PC_CONFIG_OK;
	cPairReg.second = 0;
	cVecReg.push_back( cPairReg );
	cPairReg.first = FORCE_BG0_START;
	cPairReg.second = 0;
	cVecReg.push_back( cPairReg );

	WriteStackReg( cVecReg );
	cVecReg.clear();

	std::chrono::milliseconds cWait( 100 );

	//Wait for the selected SRAM to be full then empty it
	do
	{
		cVal = ReadReg( fStrFull );

		if ( cVal == 1 )
			std::this_thread::sleep_for( cWait );

	}
	while ( cVal == 1 );

	WriteReg( fStrReadout, 0 );
	fNTotalAcq++;
}


void PixFEDFWInterface::Pause()
{

	WriteReg( BREAK_TRIGGER, 1 );

}


void PixFEDFWInterface::Resume()
{

	WriteReg( BREAK_TRIGGER, 0 );
}

uint32_t PixFEDFWInterface::ReadData( PixFED* pPixFED, unsigned int pNthAcq, bool pBreakTrigger )
{

}

uint32_t PixFEDFWInterface::computeBlockSize( BeBoard* pBoard )
{

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
	// fStrSram  = ( ( pNthAcq % 2 + 1 ) == 1 ? SRAM1 : SRAM2 );
	// fStrSramUserLogic = ( ( pNthAcq % 2 + 1 ) == 1 ? SRAM1_USR_LOGIC : SRAM2_USR_LOGIC );
	// fStrFull = ( ( pNthAcq % 2 + 1 ) == 1 ? SRAM1_FULL : SRAM2_FULL );
	// fStrReadout = ( ( pNthAcq % 2 + 1 ) == 1 ? SRAM1_END_READOUT : SRAM2_END_READOUT );
}

// FITEL METHODS
//Methods for Fitel's:

void PixFEDFWInterface::SelectFitelDDR( uint32_t pFitelId )
{
	pFitelId = 0;
	fStrSram = ( pFe ? SRAM2 : SRAM1 );
	// fStrOtherSram = ( pFe ? SRAM1 : SRAM2 );
	// fStrSramUserLogic = ( pFe ? SRAM2_USR_LOGIC : SRAM1_USR_LOGIC );
	// fStrOtherSramUserLogic = ( pFe ? SRAM1_USR_LOGIC : SRAM2_USR_LOGIC );
}


bool PixFEDFWInterface::I2cCmdAckWait( uint32_t pAckVal, uint8_t pNcount )
{
	unsigned int cWait( 100 );

	if ( pAckVal )
		cWait = pNcount * 500;


	usleep( cWait );

	uhal::ValWord<uint32_t> cVal;
	uint32_t cLoop = 0;

	do
	{
		cVal = ReadReg( CBC_I2C_CMD_ACK );

		if ( cVal != pAckVal )
		{
			// std::cout << "Waiting for the I2c command acknowledge to be " << pAckVal << " for " << pNcount << " registers." << std::endl;
			usleep( cWait );
		}

	}
	while ( cVal != pAckVal && ++cLoop < MAX_NB_LOOP );

	if ( cLoop >= MAX_NB_LOOP )
	{
		std::cout << "Warning: time out in I2C acknowledge loop (" << pAckVal << ")" << std::endl;
		return false;
	}

	return true;
}

void PixFEDFWInterface::SendBlockFitelI2cRequest( std::vector<uint32_t>& pVecReq, bool pWrite )
{
	WriteReg( fStrSramUserLogic, 1 );

	pVecReq.push_back( 0xFFFFFFFF );

	WriteReg( fStrSramUserLogic, 0 );

	WriteBlockReg( fStrSram, pVecReq );
	WriteReg( fStrOtherSram, 0xFFFFFFFF );

	WriteReg( fStrSramUserLogic, 1 );

	WriteReg( CBC_HARD_RESET, 0 );

	//r/w request
	WriteReg( CBC_I2C_CMD_RQ, pWrite ? 3 : 1 );
	// WriteReg( CBC_I2C_CMD_RQ, 1 );

	pVecReq.pop_back();

	if ( I2cCmdAckWait( ( uint32_t )1, pVecReq.size() ) == 0 )
		throw Exception( "FitelInterface: I2cCmdAckWait 1 failed." );

	WriteReg( CBC_I2C_CMD_RQ, 0 );

	if ( I2cCmdAckWait( ( uint32_t )0, pVecReq.size() ) == 0 )
		throw Exception( "FitelInterface: I2cCmdAckWait 0 failed." );

}

void PixFEDFWInterface::ReadI2cBlockValuesInSRAM( std::vector<uint32_t>& pVecReq )
{

	WriteReg( fStrSramUserLogic, 0 );

	pVecReq = ReadBlockRegValue( fStrSram, pVecReq.size() );

	WriteReg( fStrSramUserLogic, 1 );
	WriteReg( CBC_I2C_CMD_RQ, 0 );

}


void PixFEDFWInterface::EnableI2c( bool pEnable )
{
	uint32_t cValue = I2C_CTRL_ENABLE;

	if ( !pEnable )
		cValue = I2C_CTRL_DISABLE;

	WriteReg( I2C_SETTINGS, cValue );

	if ( pEnable )
		usleep( 100000 );
}

void PixFEDFWInterface::WriteFitelBlockReg( uint8_t pFeId, std::vector<uint32_t>& pVecReq )
{
	SelectFeSRAM( pFeId );
	EnableI2c( 1 );

	try
	{
		SendBlockFitelI2cRequest( pVecReq, true );
	}

	catch ( Exception& except )
	{
		throw except;
	}

	EnableI2c( 0 );
}

void PixFEDFWInterface::ReadFitelBlockReg( uint8_t pFeId, std::vector<uint32_t>& pVecReq )
{
	SelectFeSRAM( pFeId );
	EnableI2c( 1 );

	try
	{
		SendBlockFitelI2cRequest( pVecReq, false );
	}

	catch ( Exception& e )
	{
		throw e;
	}

	ReadI2cBlockValuesInSRAM( pVecReq );

	EnableI2c( 0 );
}

void PixFEDFWInterface::FlashProm( const std::string& strConfig, const char* pstrFile )
{
	checkIfUploading();

	fpgaConfig->runUpload( strConfig, pstrFile );
}

void PixFEDFWInterface::JumpToFpgaConfig( const std::string& strConfig )
{
	checkIfUploading();

	fpgaConfig->jumpToImage( strConfig );
}

std::vector<std::string> PixFEDFWInterface::getFpgaConfigList()
{
	checkIfUploading();
	return fpgaConfig->getFirmwareImageNames( );
}

void PixFEDFWInterface::DeleteFpgaConfig( const std::string& strId )
{
	checkIfUploading();
	fpgaConfig->deleteFirmwareImage( strId );
}

void PixFEDFWInterface::checkIfUploading()
{
	if ( fpgaConfig && fpgaConfig->getUploadingFpga() > 0 )
		throw Exception( "This board is uploading an FPGA configuration" );

	if ( !fpgaConfig )
		fpgaConfig = new CtaFpgaConfig( this );
}

// Fix ME!
void PixFEDFWInterface::EncodeReg( const FitelRegItem& pRegItem, uint8_t pFitelId, std::vector<uint32_t>& pVecReq )
{
	// temporary for 16CBC readout FW  (Beamtest NOV 15)
	// will have to be corrected if we want to read two modules from the same GLIB
	// (pFitelId >> 3) becomes FE ID and is encoded starting from bit21 (not used so far)
	// (pFitelId & 7) restarts FitelIDs from 0 for FE 1 (if FitelID > 7)
	pVecReq.push_back( ( pFitelId >> 3 ) << 21 | ( pFitelId & 7 ) << 17 | pRegItem.fPage << 16 | pRegItem.fAddress << 8 | pRegItem.fValue );
}


void PixFEDFWInterface::DecodeReg( FitelRegItem& pRegItem, uint8_t pFitelId, uint32_t pWord )
{
	// temporary for 16CBC readout FW  (Beamtest NOV 15)
	// will have to be corrected if we want to read two modules from the same GLIB
	uint8_t cFeId = ( pWord & cMask7 ) >> 21;
	pFitelId = ( ( pWord & cMask5 ) | ( cFeId << 3 ) ) >> 17;
	pRegItem.fPage = ( pWord & cMask6 ) >> 16;
	pRegItem.fAddress = ( pWord & cMask2 ) >> 8;
	pRegItem.fValue = pWord & cMask1;
	//std::cout << "FEID " << +(cFeId) << " pFitelID " << +(pFitelId) << std::endl;
}


