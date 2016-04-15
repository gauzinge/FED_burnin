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
PixFEDFWInterface::PixFEDFWInterface ( const char* puHalConfigFileName, uint32_t pBoardId ) :
    RegManager ( puHalConfigFileName, pBoardId ),
    fNumAcq ( 0 )
    //fData( nullptr )
{
}
//Constructor, makes the board map
PixFEDFWInterface::PixFEDFWInterface ( const char* pId, const char* pUri, const char* pAddressTable ) :
    RegManager ( pId, pUri, pAddressTable ),
    fNumAcq ( 0 )
{
}
/////////////////////////////////////////////
// CONFIG / START / STOP METHODS
////////////////////////////////////////////

std::string PixFEDFWInterface::getBoardType()
{
    // adapt me!
    std::string cBoardTypeString;

    uhal::ValWord<uint32_t> cBoardType = ReadReg ( "board_id" );

    char cChar = ( ( cBoardType & 0xFF000000 ) >> 24 );
    cBoardTypeString.push_back ( cChar );

    cChar = ( ( cBoardType & 0x00FF0000 ) >> 16 );
    cBoardTypeString.push_back ( cChar );

    cChar = ( ( cBoardType & 0x0000FF00 ) >> 8 );
    cBoardTypeString.push_back ( cChar );

    cChar = ( cBoardType & 0x000000FF );
    cBoardTypeString.push_back ( cChar );

    return cBoardTypeString;

}

void PixFEDFWInterface::getFEDNetworkParameters()
{
    std::cout << "MAC & IP Source: " << ReadReg ("mac_ip_source") << std::endl;

    std::cout << "MAC Address: " << std::hex << ReadReg ("mac_b5") << ":" << ReadReg ("mac_b4") << ":" << ReadReg ("mac_b3") << ":" << ReadReg ("mac_b2") << ":" << ReadReg ("mac_b1") << ":" << ReadReg ("mac_b0") << std::dec << std::endl;
}

void PixFEDFWInterface::getBoardInfo()
{
    std::cout << std::endl << "Board Type: " << getBoardType() << std::endl;
    getFEDNetworkParameters();
    std::string cBoardTypeString;

    uhal::ValWord<uint32_t> cBoardType = ReadReg ( "pixfed_stat_regs.user_ascii_code_01to04" );

    char cChar = ( ( cBoardType & 0xFF000000 ) >> 24 );
    cBoardTypeString.push_back ( cChar );

    cChar = ( ( cBoardType & 0x00FF0000 ) >> 16 );
    cBoardTypeString.push_back ( cChar );

    cChar = ( ( cBoardType & 0x0000FF00 ) >> 8 );
    cBoardTypeString.push_back ( cChar );

    cChar = ( cBoardType & 0x000000FF );
    cBoardTypeString.push_back ( cChar );

    cBoardType = ReadReg ( "pixfed_stat_regs.user_ascii_code_05to08" );
    cChar = ( ( cBoardType & 0xFF000000 ) >> 24 );
    cBoardTypeString.push_back ( cChar );

    cChar = ( ( cBoardType & 0x00FF0000 ) >> 16 );
    cBoardTypeString.push_back ( cChar );

    cChar = ( ( cBoardType & 0x0000FF00 ) >> 8 );
    cBoardTypeString.push_back ( cChar );

    cChar = ( ( cBoardType & 0x00000000 ) );
    cBoardTypeString.push_back ( cChar );

    std::cout << "Board Use: " << cBoardTypeString << std::endl;

    std::cout << "FW version IPHC : " << ReadReg ( "pixfed_stat_regs.user_iphc_fw_id.fw_ver_nb" ) << "." << ReadReg ( "pixfed_stat_regs.user_iphc_fw_id.archi_ver_nb" ) << "; Date: " << ReadReg ( "pixfed_stat_regs.user_iphc_fw_id.fw_ver_day" ) << "." << ReadReg ( "pixfed_stat_regs.user_iphc_fw_id.fw_ver_month" ) << "." << ReadReg ( "pixfed_stat_regs.user_iphc_fw_id.fw_ver_year" ) <<  std::endl;
    std::cout << "FW version HEPHY : " << ReadReg ( "pixfed_stat_regs.user_hephy_fw_id.fw_ver_nb" ) << "." << ReadReg ( "pixfed_stat_regs.user_hephy_fw_id.archi_ver_nb" ) << "; Date: " << ReadReg ( "pixfed_stat_regs.user_hephy_fw_id.fw_ver_day" ) << "." << ReadReg ( "pixfed_stat_regs.user_hephy_fw_id.fw_ver_month" ) << "." << ReadReg ( "pixfed_stat_regs.user_hephy_fw_id.fw_ver_year" ) << std::endl;


    std::cout << "FMC 8 Present : " << ReadReg ( "status.fmc_l8_present" ) << std::endl;
    std::cout << "FMC 12 Present : " << ReadReg ( "status.fmc_l12_present" ) << std::endl << std::endl;
}


void PixFEDFWInterface::disableFMCs()
{
    std::vector< std::pair<std::string, uint32_t> > cVecReg;
    cVecReg.push_back ( { "fmc_pg_c2m", 0 } );
    cVecReg.push_back ( { "fmc_l12_pwr_en", 0 } );
    cVecReg.push_back ( { "fmc_l8_pwr_en", 0 } );
    WriteStackReg (cVecReg);
    cVecReg.clear();
}

void PixFEDFWInterface::enableFMCs()
{
    std::vector< std::pair<std::string, uint32_t> > cVecReg;
    cVecReg.push_back ( { "fmc_pg_c2m", 1 } );
    cVecReg.push_back ( { "fmc_l12_pwr_en", 1 } );
    cVecReg.push_back ( { "fmc_l8_pwr_en", 1 } );
    WriteStackReg (cVecReg);
    cVecReg.clear();
}

void PixFEDFWInterface::findPhases (uint32_t pScopeFIFOCh)
{
    // Perform all the resets
    std::vector< std::pair<std::string, uint32_t> > cVecReg;
    cVecReg.push_back ( { "fe_ctrl_regs.decode_reset", 1 } ); // reset deocode auto clear
    cVecReg.push_back ( { "fe_ctrl_regs.decode_reg_reset", 1 } ); // reset REG auto clear
    cVecReg.push_back ( { "fe_ctrl_regs.idel_ctrl_reset", 1} );
    WriteStackReg (cVecReg);
    cVecReg.clear();
    cVecReg.push_back ( { "fe_ctrl_regs.idel_ctrl_reset", 0} );
    WriteStackReg (cVecReg);
    cVecReg.clear();

    // NOTE: here the register idel_individual_ctrl is the base address of the registers for all 48 channels. So each 32-bit word contains the control info for 1 channel. Thus by creating a vector of 48 32-bit words and writing them at the same time I can write to each channel without using relative addresses!

    // set the parameters for IDELAY scan
    std::vector<uint32_t> cValVec;

    for (uint32_t cChannel = 0; cChannel < 48; cChannel++)
        // create a Value Vector that contains the write value for each channel
        cValVec.push_back ( 0x80000000 );

    WriteBlockReg ( "fe_ctrl_regs.idel_individual_ctrl", cValVec );
    cValVec.clear();

    // set auto_delay_scan and set idel_RST
    for (uint32_t cChannel = 0; cChannel < 48; cChannel++)
        cValVec.push_back ( 0xc0000000 );

    WriteBlockReg ( "fe_ctrl_regs.idel_individual_ctrl", cValVec );
    cValVec.clear();

    // set auto_delay_scan and remove idel_RST
    for (uint32_t cChannel = 0; cChannel < 48; cChannel++)
        cValVec.push_back ( 0x80000000 );

    WriteBlockReg ( "fe_ctrl_regs.idel_individual_ctrl", cValVec );
    cValVec.clear();

    // some additional configuration
    cVecReg.push_back ( { "fe_ctrl_regs.fifo_config.overflow_value", 0x700e0}); // set 192val
    cVecReg.push_back ( { "fe_ctrl_regs.fifo_config.channel_of_interest", pScopeFIFOCh} ); // set channel for scope FIFO
    WriteStackReg (cVecReg);
    cVecReg.clear();

    // initialize Phase Finding
    WriteReg ("fe_ctrl_regs.initialize_swap", 1);
    std::cout << "Initializing Phase Finding ..." << std::endl << std::endl;
    std::chrono::milliseconds cWait ( 3000 );
    std::this_thread::sleep_for ( cWait );

    //here I might do the print loop again as Helmut does it in the latest version of the PixFED python script
    WriteReg ("fe_ctrl_regs.initialize_swap", 0);

    std::this_thread::sleep_for ( cWait );

    std::cout <<  "Phase finding Results: " << std::endl;

    uint32_t cNChannel = 24;
    std::vector<uint32_t> cReadValues = ReadBlockRegValue ( "idel_individual_stat_block", cNChannel * 4 );

    std::cout << BOLDGREEN << "FIBRE CTRL_RDY CNTVAL_Hi CNTVAL_Lo   pattern:                     S H1 L1 H0 L0   W R" << RESET << std::endl;

    //for(uint32_t cChannel = 0; cChannel < 48; cChannel++){
    for (uint32_t cChannel = 0; cChannel < cNChannel; cChannel++)
        prettyprintPhase (cReadValues, cChannel);

    //std::this_thread::sleep_for( cWait );

    cVecReg.push_back ( { "pixfed_ctrl_regs.PC_CONFIG_OK", 0} );
    WriteStackReg (cVecReg);
    cVecReg.clear();
    cVecReg.push_back ( { "pixfed_ctrl_regs.PC_CONFIG_OK", 1} );
    WriteStackReg (cVecReg);
    cVecReg.clear();
}

void PixFEDFWInterface::monitorPhases (uint32_t pScopeFIFOCh)
{
    //std::cout << "Monitoring Phases for selected Channel of Interest for 10 seconds ... " << std::endl << std::endl;
    //std::cout << BOLDGREEN << "FIBRE CTRL_RDY CNTVAL_Hi CNTVAL_Lo   pattern:                     S H1 L1 H0 L0   W R" << RESET << std::endl;
    std::chrono::milliseconds cWait ( 3000 );

    std::string cRegname = "idel_individual_stat.CH" + std::to_string (pScopeFIFOCh);
    std::vector<uint32_t> cReadValues = ReadBlockRegValue ( cRegname, 4 );
    prettyprintPhase (cReadValues, 0);
    std::this_thread::sleep_for ( cWait );
}

void PixFEDFWInterface::prettyprintPhase (const std::vector<uint32_t>& pData, int pChannel)
{
    std::cout << GREEN << "Fibre: " << std::setw (2) <<  pChannel + 1 << RESET << "    " <<
              std::bitset<1> ( (pData.at ( (pChannel * 4 ) + 0 ) >> 10 ) & 0x1 )   << "    " << std::setw (2) <<
              ( (pData.at ( (pChannel * 4 ) + 0 ) >> 5  ) & 0x1f )  << "    " << std::setw (2) <<
              ( (pData.at ( (pChannel * 4 ) + 0 )       ) & 0x1f ) << "    " <<
              BLUE << std::bitset<32> ( pData.at ( (pChannel * 4 ) + 1 ) ) << RESET << "    " <<
              std::bitset<1> ( (pData.at ( (pChannel * 4 ) + 2 ) >> 31 ) & 0x1 )  << " " << std::setw (2) <<
              ( (pData.at ( (pChannel * 4 ) + 2 ) >> 23 ) & 0x1f)  << " " << std::setw (2) <<
              ( (pData.at ( (pChannel * 4 ) + 2 ) >> 18 ) & 0x1f)  << " " << std::setw (2) <<
              ( (pData.at ( (pChannel * 4 ) + 2 ) >> 13 ) & 0x1f ) << " " << std::setw (2) <<
              ( (pData.at ( (pChannel * 4 ) + 2 ) >> 8  ) & 0x1f ) << " " << std::setw (2) <<
              ( (pData.at ( (pChannel * 4 ) + 2 ) >> 5  ) & 0x7  ) << " " << std::setw (2) <<
              ( (pData.at ( (pChannel * 4 ) + 2 )       ) & 0x1f ) << std::endl;
}

std::vector<uint32_t> PixFEDFWInterface::readTransparentFIFO()
{
    //WriteReg("fe_ctrl_regs.decode_reg_reset", 1);
    //std::vector<uint32_t> cFifoVec = ReadBlockRegValue( "fifo.bit_stream", 32 );
    std::cout << std::endl << BOLDBLUE <<  "Transparent FIFO: " << RESET << std::endl;

    //for (auto& cWord : cFifoVec)
    //std::cout << GREEN << std::bitset<30>(cWord) << RESET << std::endl;

    std::vector<uint32_t> cFifoVec;

    //std::cout << "DEBUG: Helmut's way:" << std::endl;
    for (int i = 0; i < 32; i++)
    {
        uint32_t cWord = ReadReg ("fifo.bit_stream");
        cFifoVec.push_back (cWord);

        //        std::cout << GREEN << std::bitset<30>(cWord) << RESET << std::endl;
        for (int iBit = 29; iBit >= 0; iBit--)
        {
            if (std::bitset<30> (cWord) [iBit] == 0) std::cout << GREEN << "_";
            else std::cout << "-";
        }
    }

    std::cout << RESET << std::endl;
    return cFifoVec;
}

std::vector<uint32_t> PixFEDFWInterface::readSpyFIFO()
{
    std::vector<uint32_t> cSpyA;
    std::vector<uint32_t> cSpyB;

    // cSpyA = ReadBlockRegValue( "fifo.spy_A", fBlockSize / 2 );
    // cSpyB = ReadBlockRegValue( "fifo.spy_B", fBlockSize / 2 );

    cSpyA = ReadBlockRegValue ( "fifo.spy_A", 4096 );
    cSpyB = ReadBlockRegValue ( "fifo.spy_B", 4096 );

    std::cout  << std::endl << BOLDBLUE << "TBM_SPY FIFO A: " << RESET << std::endl;
    prettyprintSpyFIFO (cSpyA);
    std::cout << std::endl << BOLDBLUE << "TBM_SPY FIFO B: " << RESET << std::endl;
    prettyprintSpyFIFO (cSpyB);
    //append content of Spy Fifo B to A and return
    std::vector<uint32_t> cAppendedSPyFifo = cSpyA;
    //cAppendedSPyFifo.insert(cSpyA.end(), cSpyB.begin(), cSpyB.end());
    return cAppendedSPyFifo;
}

void PixFEDFWInterface::prettyprintSpyFIFO (const std::vector<uint32_t>& pVec)
{
    uint32_t cMask = 0xf0;

    for (auto& cWord : pVec )
    {
        if (cWord != 0)
        {
            if ( (cWord & 0xff) != 0) std::cout << std::hex << (cWord & 0xff) << " " ;

            if ( ( (cWord & cMask) >> 4) == 11 ) std::cout << " " << std::endl;

            if ( ( (cWord & cMask) >> 4) == 6 ) std::cout << " " << std::endl;

            if ( ( (cWord & cMask) >> 4) == 7 ) std::cout << " " << std::endl;

            if ( ( (cWord & cMask) >> 4) == 15 ) std::cout << " " << std::endl;
        }

    }
}
std::string PixFEDFWInterface::readFIFO1()
{
    std::stringstream cFIFO1Str;
    std::vector<uint32_t> cFifo1A;
    std::vector<uint32_t> cFifo1B;
    std::vector<uint32_t> cMarkerA;
    std::vector<uint32_t> cMarkerB;

    cFifo1A = ReadBlockRegValue ("fifo.spy_1_A", fBlockSize / 4);
    cMarkerA = ReadBlockRegValue ("fifo.spy_1_A_marker", fBlockSize / 4);
    cFifo1B = ReadBlockRegValue ("fifo.spy_1_B", fBlockSize / 4);
    cMarkerB = ReadBlockRegValue ("fifo.spy_1_B_marker", fBlockSize / 4);
    // pass cFIFO1Str as ostream to prettyPrint for later FileIo
    std::cout << std::endl << BOLDBLUE <<  "FIFO 1 Channel A: " << RESET << std::endl;
    cFIFO1Str << "FIFO 1 Channel A: " << std::endl;
    prettyprintFIFO1 (cFifo1A, cMarkerA);
    prettyprintFIFO1 (cFifo1A, cMarkerA, cFIFO1Str);

    std::cout << std::endl << BOLDBLUE << "FIFO 1 Channel B: " << RESET << std::endl;
    cFIFO1Str << "FIFO 1 Channel B: " << std::endl;
    prettyprintFIFO1 (cFifo1B, cMarkerB);
    prettyprintFIFO1 (cFifo1B, cMarkerB, cFIFO1Str);

    return cFIFO1Str.str();
}

// a private member method to pretty print FIFO1 contents
void PixFEDFWInterface::prettyprintFIFO1 ( const std::vector<uint32_t>& pFifoVec, const std::vector<uint32_t>& pMarkerVec, std::ostream& os)
{
    os << "----------------------------------------------------------------------------------" << std::endl;

    for (uint32_t cIndex = 0; cIndex < pFifoVec.size(); cIndex++ )
    {
        if (pMarkerVec.at (cIndex) == 8)
        {
            // Event Header
            os << RED << std::dec << "    Header: " << "CH: " << ( (pFifoVec.at (cIndex) >> 26) & 0x3f ) << " ID: " <<  ( (pFifoVec.at (cIndex) >> 21) & 0x1f ) << " TBM_H: " <<  ( (pFifoVec.at (cIndex) >> 9) & 0xff ) << " EVT Nr: " <<  ( (pFifoVec.at (cIndex) ) & 0xff ) << RESET << std::endl;
        }

        if (pMarkerVec.at (cIndex) == 12)
            os << std::dec << GREEN << "ROC Header: " << "CH: " << ( (pFifoVec.at (cIndex) >> 26) & 0x3f  ) << " ROC Nr: " <<  ( (pFifoVec.at (cIndex) >> 21) & 0x1f ) << " Status: " << (  (pFifoVec.at (cIndex) ) & 0xff ) << RESET << std::endl;

        if (pMarkerVec.at (cIndex) == 1)
            os  << std::dec << "            CH: " << ( (pFifoVec.at (cIndex) >> 26) & 0x3f ) << " ROC Nr: " <<  ( (pFifoVec.at (cIndex) >> 21) & 0x1f ) << " DC: " <<  ( (pFifoVec.at (cIndex) >> 16) & 0x1f ) << " PXL: " <<  ( (pFifoVec.at (cIndex) >> 8) & 0xff ) <<  " PH: " <<  ( (pFifoVec.at (cIndex) ) & 0xff ) << std::endl;

        if (pMarkerVec.at (cIndex) == 4)
        {
            // TBM Trailer
            os << std::dec << BLUE << "   Trailer: " << "CH: " << ( (pFifoVec.at (cIndex) >> 26) & 0x3f ) << " ID: " <<  ( (pFifoVec.at (cIndex) >> 21) & 0x1f ) << " TBM_T2: " <<  ( (pFifoVec.at (cIndex) >> 12) & 0xff ) << " TBM_T1: " <<  ( (pFifoVec.at (cIndex) ) & 0xff ) << RESET << std::endl;
        }

        if (pMarkerVec.at (cIndex) == 6)
        {
            // Event Trailer
            os << std::dec << RED << "Event Trailer: " << "CH: " << ( (pFifoVec.at (cIndex) >> 26) & 0x3f ) << " ID: " <<  ( (pFifoVec.at (cIndex) >> 21) & 0x1f ) << " marker: " <<  ( (pFifoVec.at (cIndex) ) & 0x1fffff ) << RESET << std::endl;
        }
    }

    os << "----------------------------------------------------------------------------------" << std::endl;
}


uint32_t PixFEDFWInterface::readOSDWord (uint32_t pROCId, uint32_t pScopeFIFOCh)
{
    std::cout << BOLDBLUE << "OSD Readback enabled for ROC " << pROCId << RESET << std::endl;
    // first, tell the FW which ROC to consider
    WriteReg ("fe_ctrl_regs.fifo_config.OSD_ROC_Nr", (pROCId & 0x7) );

    // now read back the OSD bit - note that 16 triggers are required for a full 16 bit word

    uint16_t cOSD_word_A = 0;
    uint16_t cOSD_word_B = 0;

    //int cNChannel = 24;
    // use this to read the OSD word for all channels, obviously need a loop to pick the correct word for all channels then
    //std::vector<uint32_t> cReadValues = ReadBlockRegValue( "idel_individual_stat_block", cNChannel * 4 );
    //for(uint32_t iChannel = 0; iChannel < cNChannel; i++)
    //{
    //  word = cReadValues.at((iChannel * 4)) + 3;
    //  std::cout << A << B << etc...
    //}

    // use this to read for an individual channel
    std::string cRegname = "idel_individual_stat.CH" + std::to_string (pScopeFIFOCh);
    std::vector<uint32_t> cReadValues = ReadBlockRegValue ( cRegname, 4 );

    cOSD_word_A = cReadValues.at (3) & 0xffff;
    cOSD_word_B = (cReadValues.at (3) >> 16) & 0xffff;

    std::cout << GREEN << "TBM Core A: " << std::endl << "aaaarrrr87654321" << std::endl << std::bitset<16> (cOSD_word_A) << RESET << std::endl;

    std::cout << GREEN << "TBM Core B: " << std::endl << "aaaarrrr87654321" << std::endl << std::bitset<16> (cOSD_word_B) << RESET << std::endl;

    return cReadValues.at (3);
}


bool PixFEDFWInterface::ConfigureBoard ( const PixFED* pPixFED, bool pFakeData )
{
    std::vector< std::pair<std::string, uint32_t> > cVecReg;

    std::chrono::milliseconds cPause ( 200 );
    //WriteReg( "pixfed_ctrl_regs.PC_CONFIG_OK", 0 );
    //Primary Configuration
    cVecReg.push_back ( {"pixfed_ctrl_regs.PC_CONFIG_OK", 0} );
    //cVecReg.push_back( {"pixfed_ctrl_regs.INT_TRIGGER_EN", 0} );
    cVecReg.push_back ( {"pixfed_ctrl_regs.rx_index_sel_en", 0} );
    cVecReg.push_back ( {"pixfed_ctrl_regs.DDR0_end_readout", 0} );
    cVecReg.push_back ( {"pixfed_ctrl_regs.DDR1_end_readout", 0} );

    //cVecReg.push_back( {"pixfed_ctrl_regs.CMD_START_BY_PC", 0} );

    // fitel I2C bus reset & fifo TX & RX reset
    cVecReg.push_back ({"pixfed_ctrl_regs.fitel_i2c_cmd_reset", 1});


    // the FW needs to be aware of the true 32 bit workd Block size for some reason! This is the Packet_nb_true in the python script?!
    computeBlockSize ( pFakeData );
    cVecReg.push_back ( {"pixfed_ctrl_regs.PACKET_NB", fBlockSize32 } );

    PixFEDRegMap cPixFEDRegMap = pPixFED->getPixFEDRegMap();

    for ( auto const& it : cPixFEDRegMap )
    {
        cVecReg.push_back ( {it.first, it.second} );
    }

    WriteStackReg ( cVecReg );

    cVecReg.clear();

    cVecReg.push_back ({"pixfed_ctrl_regs.fitel_i2c_cmd_reset", 0});
    cVecReg.push_back ({"pixfed_ctrl_regs.acq_ctrl.calib_mode", 0});
    cVecReg.push_back ({"pixfed_ctrl_regs.fitel_config_req", 0});
    cVecReg.push_back ( {"pixfed_ctrl_regs.PC_CONFIG_OK", 1} );
    WriteStackReg ( cVecReg );

    cVecReg.clear();

    fAcq_mode = ReadReg ("pixfed_ctrl_regs.acq_ctrl.acq_mode");

    std::this_thread::sleep_for ( cPause );

    // Read back the DDR3 calib done flag
    bool cDDR3calibrated = ( ReadReg ( "pixfed_stat_regs.ddr3_init_calib_done" ) & 0x00000001 );

    if ( cDDR3calibrated ) std::cout << "DDR3 calibrated, board configured!" << std::endl;

    return cDDR3calibrated;

}

void PixFEDFWInterface::HaltBoard()
{
    std::string cRegName = "ctrl.ttc_xpoint_A_out3";
    uint32_t cRegValue = 3;
    WriteReg (cRegName, cRegValue);
}

void PixFEDFWInterface::Start()
{
    //set fNthAcq to 0 since I am starting from scratch and thus start with DDR0
    fNthAcq = 0;
    std::vector< std::pair<std::string, uint32_t> > cVecReg;
    //cVecReg.push_back( {"pixfed_ctrl_regs.CMD_START_BY_PC", 1} );
    cVecReg.push_back ( {"pixfed_ctrl_regs.PC_CONFIG_OK", 1} );
    cVecReg.push_back ( {"pixfed_ctrl_regs.INT_TRIGGER_EN", 0} );

    //WriteStackReg( cVecReg );
}

void PixFEDFWInterface::Stop()
{
    //Stop the DAQ
    std::vector< std::pair<std::string, uint32_t> > cVecReg;
    //cVecReg.push_back( {"pixfed_ctrl_regs.CMD_START_BY_PC", 0} );
    //cVecReg.push_back( {"pixfed_ctrl_regs.INT_TRIGGER_EN", 0} );
    cVecReg.push_back ( {"pixfed_ctrl_regs.PC_CONFIG_OK", 0} );
    //cVecReg.push_back( {"pixfed_ctrl_regs.rx_index_sel_en", 0} );

    WriteStackReg ( cVecReg );
    cVecReg.clear();

    std::chrono::milliseconds cWait ( 100 );

    //Wait for the selected SRAM to be full then empty it
    //Select SRAM
    uhal::ValWord<uint32_t> cVal;

    SelectDaqDDR ( fNthAcq );

    do
    {
        cVal = ReadReg ( fStrFull );

        if ( cVal == 1 )
            std::this_thread::sleep_for ( cWait );

    }
    while ( cVal == 1 );

    WriteReg ( fStrReadout, 0 );
    fNthAcq++;
}


void PixFEDFWInterface::Pause()
{
    WriteReg ( "pixfed_ctrl_regs.INT_TRIGGER_EN", 0 );
}


void PixFEDFWInterface::Resume()
{
    WriteReg ( "pixfed_ctrl_regs.INT_TRIGGER_EN", 1 );
}

std::vector<uint32_t> PixFEDFWInterface::ReadData ( PixFED* pPixFED, uint32_t pBlockSize )
{
    uint32_t cBlockSize = 0;

    if (pBlockSize == 0) cBlockSize = fBlockSize;
    else cBlockSize = pBlockSize;

    std::chrono::milliseconds cWait ( 10 );
    // the fNthAcq variable is automatically used to determine which DDR FIFO to read - so it has to be incremented in this method!

    // first find which DDR bank to read
    SelectDaqDDR ( fNthAcq );
    //std::cout << "Querying " << fStrDDR << " for FULL condition!" << std::endl;

    uhal::ValWord<uint32_t> cVal;

    do
    {
        cVal = ReadReg ( fStrFull );

        if ( cVal == 0 ) std::this_thread::sleep_for ( cWait );
    }
    while ( cVal == 0 );

    //std::cout << fStrDDR << " full: " << ReadReg( fStrFull ) << std::endl;

    // DDR control: 0 = ipbus, 1 = user
    //WriteReg ( fStrDDRControl, 0 );
    //std::this_thread::sleep_for ( cWait );
    //std::cout << "Starting block read of " << fStrDDR << std::endl;

    std::vector<uint32_t> cData = ReadBlockRegValue ( fStrDDR, cBlockSize );
    //WriteReg ( fStrDDRControl , 1 );
    //std::this_thread::sleep_for ( cWait );
    WriteReg ( fStrReadout, 1 );
    std::this_thread::sleep_for ( cWait );

    // full handshake between SW & FW
    while ( ReadReg ( fStrFull ) == 1 )
        std::this_thread::sleep_for ( cWait );

    WriteReg ( fStrReadout, 0 );

    if (fAcq_mode == 1) prettyprintTBMFIFO (cData);
    else prettyprintSlink (expandto64(cData));
    fNthAcq++;
    return cData;
}

std::vector<uint32_t> PixFEDFWInterface::ReadNEvents ( PixFED* pPixFED, uint32_t pNEvents )
{
    std::cout << "Requesting " << pNEvents << " Events from FW!" << std::endl;
    //first, set up calibration mode
    std::vector< std::pair<std::string, uint32_t> > cVecReg;
    cVecReg.push_back ( {"pixfed_ctrl_regs.PC_CONFIG_OK", 0} );
    cVecReg.push_back ({"pixfed_ctrl_regs.acq_ctrl.calib_mode", 1});
    cVecReg.push_back ({"pixfed_ctrl_regs.acq_ctrl.calib_mode_NEvents", pNEvents - 1});
    WriteStackReg ( cVecReg );
    cVecReg.clear();

    WriteReg("pixfed_ctrl_regs.PC_CONFIG_OK", 1);

    // first set DDR bank to 0
    SelectDaqDDR ( 0 );
    //uint32_t cBlockSize = 0;

    //if (pBlockSize == 0) cBlockSize = fBlockSize;
    //else cBlockSize = pBlockSize;

    std::chrono::milliseconds cWait ( 10 );
    // the fNthAcq variable is automatically used to determine which DDR FIFO to read - so it has to be incremented in this method!

    //std::cout << "Querying " << fStrDDR << " for FULL condition!" << std::endl;

    uhal::ValWord<uint32_t> cVal;

    do
    {
        cVal = ReadReg ( fStrFull );

        if ( cVal == 0 ) std::this_thread::sleep_for ( cWait );
    }
    while ( cVal == 0 );

    //std::cout << fStrDDR << " full: " << ReadReg( fStrFull ) << std::endl;

    //now figure out how many 32 bit words to read
    uint32_t cNWords32 = ReadReg ("pixfed_stat_regs.cnt_word32from_start");
    std::cout << "Reading " << cNWords32 << " 32 bit words from DDR " << 0 << std::endl;
    //in normal TBM Fifo mode read 2* the number of words read from the FW (+1 fake trigger)
    //in FEROL IPBUS mode read the number of 32 bit words + 2*2*pNEvents (1 factor 2 is for 64 bit words)
uint32_t cBlockSize = (fAcq_mode == 1) ?  2 * cNWords32 + 1 :
                          cNWords32 + (2 * 2 * pNEvents) + 1;
std::cout << "This translates into " << cBlockSize << " words in the current mode: " << fAcq_mode << std::endl;

    // DDR control: 0 = ipbus, 1 = user
    //WriteReg ( fStrDDRControl, 0 );
    std::this_thread::sleep_for ( cWait );
    //std::cout << "Starting block read of " << fStrDDR << std::endl;

    std::vector<uint32_t> cData = ReadBlockRegValue ( fStrDDR, cBlockSize );
    //WriteReg ( fStrDDRControl , 1 );
    std::this_thread::sleep_for ( cWait );
    WriteReg ( fStrReadout, 1 );
    std::this_thread::sleep_for ( cWait );

    // full handshake between SW & FW
    while ( ReadReg ( fStrFull ) == 1 )
        std::this_thread::sleep_for ( cWait );

    WriteReg ( fStrReadout, 0 );

    if (fAcq_mode == 1) prettyprintTBMFIFO (cData);
    else prettyprintSlink (expandto64(cData));

    fNthAcq++;
    return cData;
}

void PixFEDFWInterface::prettyprintTBMFIFO (const std::vector<uint32_t>& pData )
{
    std::cout << BOLDBLUE << "Global TBM Readout FIFO: " << RESET << std::endl;
    //now I need to do something with the Data that I read into cData
    int cIndex = 0;
    uint32_t cPreviousWord;

    for ( auto& cWord : pData )
    {
        //      std::cout << std::hex << std::setw(8) << std::setfill('0');
        if (cIndex % 2 == 0)
            cPreviousWord = cWord;

        else if (cPreviousWord == 0x1)
        {
            //std::cout << cWord <<  std::endl;
            std::cout << "    Pixel Hit: CH: " << ( (cWord >> 26) & 0x3f) << " ROC: " << ( (cWord >> 21) & 0x1f) << " DC: " << ( (cWord >> 16) & 0x1f) << " ROW: " << ( (cWord >> 8) & 0xff) << " PH: " << (cWord & 0xff) << std::dec << std::endl;
        }
        //else if (cPreviousWord == 0x6)
        //{
        ////std::cout << cWord <<  std::endl;
        //std::cout << "Event Trailer: CH: " << ((cWord >> 26) & 0x3f) << " ID: " << ((cWord >> 21) & 0x1f) << " marker: " << (cWord & 0x1fffff) << " ROW: " << ((cWord >> 8) & 0xff) << " PH: " << (cWord & 0xff) << std::dec << std::endl;
        //}
        else if (cPreviousWord == 0x8)
        {
            //std::cout << cWord <<  std::endl;
            std::cout << "Event Header: CH: " << ( (cWord >> 26) & 0x3f) << " ID: " << ( (cWord >> 21) & 0x1f) << " TBM H: " << ( (cWord >> 16) & 0x1f) << " ROW: " << ( (cWord >> 9) & 0xff) << " EventNumber: " << (cWord & 0xff) << std::dec << std::endl;
        }
        else if (cPreviousWord == 0xC)
        {
            //std::cout << cWord <<  std::endl;
            std::cout << " ROC Header: CH: " << ( (cWord >> 26) & 0x3f) << " ROC Nr: " << ( (cWord >> 21) & 0x1f) << " Status : " << (cWord  & 0xff) << std::dec << std::endl;
        }
        else if (cPreviousWord == 0x4)
        {
            //std::cout << cWord <<  std::endl;
            std::cout << " TBM Trailer: CH: " << ( (cWord >> 26) & 0x3f) << " ID: " << ( (cWord >> 21) & 0x1f) << " TBM T2: " << ( (cWord >> 12) & 0xff) << " TBM_T1: " << (cWord & 0xff) << std::dec << std::endl;
        }

        cIndex++;
    }
}

void PixFEDFWInterface::prettyprintSlink (const std::vector<uint64_t>& pData )
{
    for (auto& cWord : pData)
    {
        //now run Jordans decoder. First, check the header
        if ( (cWord >> 60) == 0x5 )
        {
            //Header
            std::cout << BOLDGREEN << "Evt. ty " << ((cWord >> 56) & 0xF )<< " L1A Id " << ((cWord >> 32) & 0xFFFFFF) << " BX Id " << ((cWord >> 20) & 0xFFF ) << " Source Id " << ((cWord >> 8) & 0xFFF) << " FOV " << ((cWord >> 4) & 0xF) << RESET << std::endl;

        }
        else if( (cWord >> 60) == 0xa )
        {
            //Trailer
            std::cout << BOLDRED << "Evt. Length " << ((cWord >> 32) & 0xFFFFFF )<< " CRC " << ((cWord >> 16) & 0xFFFF) << RESET << std::endl;
            
        }
        else //if (cWord != 0xFFFFFFFFFFFFFFFF)
        {
           //Payload
           //2 32 bit data words in each 64 bit word containing a hit each 
            uint32_t cWord1 = cWord & 0xFFFFFFFF;
            uint32_t cWord2 = (cWord >> 32) & 0xFFFFFFFF;
            
            std::cout << "Channel " << ((cWord1 >> 26) & 0x3F) << " ROC " <<  ((cWord1 >> 21) & 0x1F) << " DC " << ((cWord1 >> 16) & 0x1F) << " Pxl " << ((cWord1 >> 8) & 0xFF) << " PH " << (cWord1 & 0xFF) << std::endl; 
            std::cout << "Channel " << ((cWord2 >> 26) & 0x3F) << " ROC " <<  ((cWord2 >> 21) & 0x1F) << " DC " << ((cWord2 >> 16) & 0x1F) << " Pxl " << ((cWord2 >> 8) & 0xFF) << " PH " << (cWord2 & 0xFF) << std::endl; 
        }
    } 
}

uint32_t PixFEDFWInterface::computeBlockSize ( bool pFakeData )
{
    if (pFakeData)
    {
        // this is the number of bits to read from DDR
        fBlockSize = fNTBM * fNCh * fNPattern * fPacketSize;
    }

    // since the DDR data widt is 256 this is the number of 32 bit words I have to read
    fBlockSize32 = static_cast<uint32_t> ( ceil (fBlockSize / double (8 ) ) - 1);
    return fBlockSize;
}

std::vector<uint32_t> PixFEDFWInterface::ReadBlockRegValue ( const std::string& pRegNode, const uint32_t& pBlocksize )
{
    uhal::ValVector<uint32_t> valBlock = ReadBlockReg ( pRegNode, pBlocksize );
    std::vector<uint32_t> vBlock = valBlock.value();
    return vBlock;
}

bool PixFEDFWInterface::WriteBlockReg ( const std::string& pRegNode, const std::vector< uint32_t >& pValues )
{
    bool cWriteCorr = RegManager::WriteBlockReg ( pRegNode, pValues );
    return cWriteCorr;
}

void PixFEDFWInterface::SelectDaqDDR ( uint32_t pNthAcq )
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
void PixFEDFWInterface::EncodeReg ( const FitelRegItem& pRegItem, uint8_t pFMCId, uint8_t pFitelId, std::vector<uint32_t>& pVecReq )
{
    pVecReq.push_back (  pFMCId  << 24 |  pFitelId << 20 |  pRegItem.fAddress << 8 | pRegItem.fValue );
}


void PixFEDFWInterface::DecodeReg ( FitelRegItem& pRegItem, uint8_t pFMCId, uint8_t pFitelId, uint32_t pWord )
{
    uint8_t cFMCId = ( pWord & 0xff000000 ) >> 24;
    pFitelId = (  pWord & 0x00f00000   ) >> 20;
    pRegItem.fAddress = ( pWord & 0x0000ff00 ) >> 8;
    pRegItem.fValue = pWord & 0x000000ff;
    //std::cout << "FMCID " << +(cFMCId) << " pFitelID " << +(pFitelId) << std::endl;
}

void PixFEDFWInterface::i2cRelease (uint32_t pTries)
{
    uint32_t cCounter = 0;
    // release
    WriteReg ("pixfed_ctrl_regs.fitel_config_req", 0);

    while (ReadReg ("pixfed_stat_regs.fitel_config_ack") != 0)
    {
        if (cCounter > pTries)
        {
            std::cout << "Error, exceeded maximum number of tries for I2C release!" << std::endl;
            break;
        }
        else
        {
            usleep (100);
            cCounter++;
        }
    }
}

bool PixFEDFWInterface::polli2cAcknowledge (uint32_t pTries)
{
    bool cSuccess = false;
    uint32_t cCounter = 0;

    // wait for command acknowledge
    while (ReadReg ("pixfed_stat_regs.fitel_config_ack") == 0)
    {
        if (cCounter > pTries)
        {
            std::cout << "Error, polling for I2C command acknowledge timed out!" << std::endl;
            break;

        }
        else
        {
            usleep (100);
            cCounter++;
        }
    }

    // check the value of that register
    if (ReadReg ("pixfed_stat_regs.fitel_config_ack") == 1)
        cSuccess = true;
    else if (ReadReg ("pixfed_stat_regs.fitel_config_ack") == 3)
        cSuccess = false;

    return cSuccess;
}

bool PixFEDFWInterface::WriteFitelBlockReg (std::vector<uint32_t>& pVecReq)
{
    WriteReg ("pixfed_ctrl_regs.fitel_i2c_addr", 0x4d);
    bool cSuccess = false;
    // write the encoded registers in the tx fifo
    WriteBlockReg ("fitel_config_fifo_tx", pVecReq);
    // sent an I2C write request
    WriteReg ("pixfed_ctrl_regs.fitel_config_req", 1);

    // wait for command acknowledge
    while (ReadReg ("pixfed_stat_regs.fitel_config_ack") == 0) usleep (100);

    uint32_t cVal = ReadReg ("pixfed_stat_regs.fitel_config_ack");

    //if (ReadReg("pixfed_stat_regs.fitel_config_ack") == 1)
    if (cVal == 1)
        cSuccess = true;
    //else if (ReadReg("pixfed_stat_regs.fitel_config_ack") == 3)
    else if (cVal == 3)
    {
        std::cout << "Error writing Registers!" << std::endl;
        cSuccess = false;
    }

    // release
    i2cRelease (10);
    return cSuccess;
}

bool PixFEDFWInterface::ReadFitelBlockReg (std::vector<uint32_t>& pVecReq)
{
    WriteReg ("pixfed_ctrl_regs.fitel_i2c_addr", 0x4d);
    bool cSuccess = false;
    //uint32_t cVecSize = pVecReq.size();

    // write the encoded registers in the tx fifo
    WriteBlockReg ("fitel_config_fifo_tx", pVecReq);
    // sent an I2C write request
    WriteReg ("pixfed_ctrl_regs.fitel_config_req", 3);

    // wait for command acknowledge
    while (ReadReg ("pixfed_stat_regs.fitel_config_ack") == 0) usleep (100);

    uint32_t cVal = ReadReg ("pixfed_stat_regs.fitel_config_ack");

    //if (ReadReg("pixfed_stat_regs.fitel_config_ack") == 1)
    if (cVal == 1)
        cSuccess = true;
    //else if (ReadReg("pixfed_stat_regs.fitel_config_ack") == 3)
    else if (cVal == 3)
    {
        cSuccess = false;
        std::cout << "Error reading registers!" << std::endl;
    }

    // release
    i2cRelease (10);

    // clear the vector & read the data from the fifo
    pVecReq = ReadBlockRegValue ("fitel_config_fifo_rx", pVecReq.size() );
    return cSuccess;
}



std::vector<double> PixFEDFWInterface::ReadADC ( const uint8_t pFMCId, const uint8_t pFitelId, bool pPrintAll)
{
    // the Fitel FMC needs to be set up to be able to read the RSSI on a given Channel:
    // I2C register 0x1: set to 0x4 for RSSI, set to 0x5 for Die Temperature of the Fitel
    // Channel Control Registers: set to 0x02 to disable RSSI for this channel, set to 0x0c to enable RSSI for this channel
    // the ADC always reads the sum of all the enabled channels!
    //initial FW setup
    WriteReg ("pixfed_ctrl_regs.fitel_i2c_cmd_reset", 1);

    std::vector<std::pair<std::string, uint32_t> > cVecReg;
    cVecReg.push_back ({"pixfed_ctrl_regs.fitel_i2c_cmd_reset", 0});
    cVecReg.push_back ({"pixfed_ctrl_regs.fitel_config_req", 0});
    //Laurent is a Bastard because he changes the i2c addr register!
    cVecReg.push_back ({"pixfed_ctrl_regs.fitel_i2c_addr", 0x77});

    WriteStackReg (cVecReg);

    //first, write the correct registers to configure the ADC
    //the values are: Address 0x01 -> 0x1<<6 & 0x1f
    //                Address 0x02 -> 0x1

    // Vectors for write and read data!
    std::vector<uint32_t> cVecWrite;
    std::vector<uint32_t> cVecRead;

    //encode them in a 32 bit word and write, no readback yet
    cVecWrite.push_back (  pFMCId  << 24 |  pFitelId << 20 |  0x1 << 8 | 0x5f );
    cVecWrite.push_back (  pFMCId  << 24 |  pFitelId << 20 |  0x2 << 8 | 0x01 );
    WriteBlockReg ("fitel_config_fifo_tx", cVecWrite);

    // sent an I2C write request
    WriteReg ("pixfed_ctrl_regs.fitel_config_req", 1);

    // wait for command acknowledge
    while (ReadReg ("pixfed_stat_regs.fitel_config_ack") == 0) usleep (100);

    uint32_t cVal = ReadReg ("pixfed_stat_regs.fitel_config_ack");

    if (cVal == 3)
        std::cout << "Error reading registers!" << std::endl;

    // release
    i2cRelease (10);

    //now prepare the read-back of the values
    uint8_t cNWord = 10;

    for (uint8_t cIndex = 0; cIndex < cNWord; cIndex++)
        cVecRead.push_back ( pFMCId << 24 | pFitelId << 20 | (0x6 + cIndex ) << 8 | 0 );

    //Laurent is a Bastard because he changes the i2c addr register!
    WriteReg ("pixfed_ctrl_regs.fitel_i2c_addr", 0x4c);

    WriteBlockReg ( "fitel_config_fifo_tx", cVecRead );
    // sent an I2C write request
    WriteReg ("pixfed_ctrl_regs.fitel_config_req", 3);

    // wait for command acknowledge
    while (ReadReg ("pixfed_stat_regs.fitel_config_ack") == 0) usleep (100);

    cVal = ReadReg ("pixfed_stat_regs.fitel_config_ack");

    if (cVal == 3)
        std::cout << "Error reading registers!" << std::endl;

    // release
    i2cRelease (10);

    // clear the vector & read the data from the fifo
    cVecRead = ReadBlockRegValue ("fitel_config_fifo_rx", cVecRead.size() );

    // now convert to Voltages!
    std::vector<double> cLTCValues (cNWord / 2, 0);

    double cConstant = 0.00030518;

    // each value is hidden in 2 I2C words
    for (int cMeasurement = 0; cMeasurement < cNWord / 2; cMeasurement++)
    {
        // build the values
        uint16_t cValue = ( (cVecRead.at (2 * cMeasurement) & 0x7F) << 8) + (cVecRead.at (2 * cMeasurement + 1) & 0xFF);
        uint8_t cSign = (cValue >> 14) & 0x1;

        //now the conversions are different for each of the voltages, so check by cMeasurement
        if (cMeasurement == 4)
            cLTCValues.at (cMeasurement) = (cSign == 0b1) ? (- ( 32768 - cValue ) * cConstant + 2.5) : (cValue * cConstant + 2.5);

        else
            cLTCValues.at (cMeasurement) = (cSign == 0b1) ? (- ( 32768 - cValue ) * cConstant) : (cValue * cConstant);

        if (pPrintAll)
            std::cout << "V " << cMeasurement + 1 << " = " << cLTCValues.at (cMeasurement) << std::endl;
    }

    // now I have all 4 voltage values in a vector of size 5
    // V1 = cLTCValues[0]
    // V2 = cLTCValues[1]
    // V3 = cLTCValues[2]
    // V4 = cLTCValues[3]
    // Vcc = cLTCValues[4]
    //
    // the RSSI value = fabs(V3-V4) / R=150 Ohm [in Amps]
    double cADCVal = fabs (cLTCValues.at (2) - cLTCValues.at (3) ) / 150.0;
    std::cout << BOLDBLUE << "FMC " << +pFMCId << " Fitel " << +pFitelId << " RSSI " << cADCVal * 1000  << " mA" << RESET << std::endl;
    return cLTCValues;
}

/////////////////////////////////////////////
// FIRMWARE METHODS
//Methods for FW upload:
////////////////////////////////////////////


void PixFEDFWInterface::FlashProm ( const std::string& strConfig, const char* pstrFile )
{
    checkIfUploading();

    fpgaConfig->runUpload ( strConfig, pstrFile );
}

void PixFEDFWInterface::JumpToFpgaConfig ( const std::string& strConfig )
{
    checkIfUploading();

    fpgaConfig->jumpToImage ( strConfig );
}

std::vector<std::string> PixFEDFWInterface::getFpgaConfigList()
{
    checkIfUploading();
    return fpgaConfig->getFirmwareImageNames( );
}

void PixFEDFWInterface::DeleteFpgaConfig ( const std::string& strId )
{
    checkIfUploading();
    fpgaConfig->deleteFirmwareImage ( strId );
}

void PixFEDFWInterface::DownloadFpgaConfig ( const std::string& strConfig, const std::string& strDest)
{
    checkIfUploading();
    fpgaConfig->runDownload ( strConfig, strDest.c_str() );
}

void PixFEDFWInterface::checkIfUploading()
{
    if ( fpgaConfig && fpgaConfig->getUploadingFpga() > 0 )
        throw Exception ( "This board is uploading an FPGA configuration" );

    if ( !fpgaConfig )
        fpgaConfig = new CtaFpgaConfig ( this );
}
