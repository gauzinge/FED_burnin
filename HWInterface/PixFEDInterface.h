/*!

        \file                                            PixFEDInterface.h
        \brief                                           User Interface to the Boards
        \author                                          G. Auzinger
        \version                                         1.0
        \date                        01/12/15
        Support :                    mail to :georg.auzinger@SPAMNOTcern.ch
*/
#ifndef PixFEDInterface_h_
#define PixFEDInterface_h_

#include "PixFEDFWInterface.h"
#include "../Utils/ConsoleColor.h"
#include "../Utils/Utilities.h"
#include "../HWDescription/Fitel.h"
#include "../HWDescription/PixFED.h"

/*!
 * \namespace Ph2_HwInterface
 * \brief Namespace regrouping all the interfaces to the hardware
 */

typedef std::map<uint16_t, PixFEDFWInterface*> PixFEDFWMap;    /*!< Map of Board connected */

/*!
 * \class BeBoardInterface
 * \brief Class representing the User Interface to the different boards
 */
class PixFEDInterface
{

private:
    PixFEDFWMap fFWMap;                     /*!< Map of Board connected */
    PixFEDFWInterface* fFEDFW;                     /*!< Board loaded */
    uint16_t prevBoardIdentifier;                     /*!< Id of the previous board */

private:
    /*!
     * \brief Set the board to talk with
     * \param pBoardId
     */
    void setBoard( uint16_t pBoardIdentifier );

public:
    /*!
     * \brief Constructor of the BeBoardInterface class
     * \param pBoardMap Reference to the BoardFWInterface
     */
    PixFEDInterface( const PixFEDFWMap& pFWMap );
    /*!
     * \brief Destructor of the BeBoardInterface class
     */
    ~PixFEDInterface();

    //////////////////////////////////////
    // PixFED Register Methods
    /////////////////////////////////////

    /*!
     * \brief Update both Board register and Config File
     * \param pFED
     * \param pRegNode : Node of the register to update
     * \param pVal : Value to write
     */
    void WriteBoardReg( PixFED* pFED, const std::string& pRegNode, const uint32_t& pVal );
    /*!
         * \brief Write a block of a given size into the board
        * \param pFED
        * \param pRegNode : Node of the register to write
        * pValVec Vector of values to write
         */
    void WriteBlockBoardReg( PixFED* pFED, const std::string& pRegNode, const std::vector<uint32_t>& pValVec );
    /*!
     * \brief Write: Update both Board register and Config File
     * \param pFED
     * \param pRegVec : Vector of Register/Value pairs
     */
    void WriteBoardMultReg( PixFED* pFED, const std::vector < std::pair< std::string , uint32_t > >& pRegVec );
    /*!
    * \brief Update Config File with the value in the Board register
    * \param pFED
    * \param pRegNode : Node of the register to update
    */
    uint32_t ReadBoardReg( PixFED* pFED, const std::string& pRegNode );
    /*!
         * \brief Read a block of a given size from the board
        * \param pFED
        * \param pRegNode : Node of the register to read
        * \param pSize Number of 32-bit words in the block
         */
    std::vector<uint32_t> ReadBlockBoardReg( PixFED* pFED, const std::string& pRegNode, uint32_t pSize );
    /*!
     * \brief Read a vector of Registers
     * \param pFED
     * \param pRegVec : Vector of Register/Value pairs
     */
    void ReadBoardMultReg( PixFED* pFED, std::vector < std::pair< std::string , uint32_t > >& pRegVec );


    /////////////////////////////////////
    // FITEL Register Methods
    /////////////////////////////////////

    void EncodeFitelReg( const FitelRegItem& pRegItem, uint8_t pFMCId, uint8_t pFitelId , std::vector<uint32_t>& pVecReq )
    {
        fFEDFW->EncodeReg( pRegItem, pFMCId, pFitelId, pVecReq );
    }


    void DecodeFitelReg( FitelRegItem& pRegItem, uint8_t pFMCId, uint8_t pFitelId, uint32_t pWord )
    {
        fFEDFW->DecodeReg( pRegItem, pFMCId, pFitelId, pWord );
    }

    bool WriteFitelReg( Fitel* pFitel, const std::string& pRegNode, uint8_t pValue, bool pVerifLoop = true);

    uint8_t ReadFitelReg( Fitel* pFitel, const std::string& pRegNode);

    void ConfigureFitel( Fitel* pFitel, bool pVerifLoop = true);

    std::vector<double> ReadADC( Fitel* pFitel, uint32_t pChan, bool pPrintAll = false );

    /////////////////////////////////////
    // PixFED System Methods
    /////////////////////////////////////

    /*!
     * \brief Get the board infos
     * \param pFED
     */
    void getBoardInfo( const PixFED* pFED );
    /*!
     * \brief: enable FMCs
     */
    void enableFMCs( const PixFED* pFED );
    /*!
     * \brief: disable FMCs
     */
    void disableFMCs( const PixFED* pFED );
    /*!
     * \brief Configure the board with its Config File
     * \param pFED
     */
    void ConfigureFED( const PixFED* pFED );
    /*!
     * \brief: Halt the FED and put it back into safe mode
     */
    void HaltFED( const PixFED* pFED );
    /*!
     * \brief: find Phases for input data stream
     * \param: pScopeFIFOCh: channel for Phase Stability monitoring
     */
    void findPhases( const PixFED* pFED, uint32_t pScopeFIFOCh = 0 );
    /*!
     * \brief: find Phases for input data stream
     * \param: pScopeFIFOCh: channel for Phase Stability monitoring
     */
    void monitorPhases( const PixFED* pFED, uint32_t pScopeFIFOCh = 0 );
    /*!
     * \brief: read transparent Fifo
     * \return: vector of 32 bit words with FIFO content
     */
    std::vector<uint32_t> readTransparentFIFO( const PixFED* pFED );
    /*!
     * \brief: read contents of the Spy Fifo
     * \return: concanated Fifos for TBM cores A (1) & B(1)
     */
    std::vector<uint32_t> readSpyFIFO( const PixFED* pFED );
    /*!
     * \brief: read contents of FIFO1
     * \return string of the FIFO1 console dump
     */
    std::string readFIFO1( const PixFED* pFED );
    /*!
     * \brief: read ROC OSD readback word
     * \param: pFED
     * \param: pROCId
     * \param: pChannelOfInteres
     * \return: 32 bit word containing OSD words both both TBM cores
     */
    uint32_t readOSDWord(const PixFED* pFED, uint32_t pROCId, uint32_t pChannelOfInterest);
    /*!
     * \brief Start a DAQ
     * \param pFED
     */
    void Start( PixFED* pFED );
    /*!
     * \brief Stop a DAQ
     * \param pFED
     * \param pNthAcq : actual number of acquisitions
     */
    void Stop( PixFED* pFED );
    /*!
     * \brief Pause a DAQ
     * \param pFED
     */
    void Pause( PixFED* pFED );
    /*!
     * \brief Resume a DAQ
     * \param pFED
     */
    void Resume( PixFED* pFED );
    /*!
     * \brief Read data from DAQ
     * \param pFED
     * \param pBreakTrigger : if true, enable the break trigger
     * \return cData: the data in 32 bit words
     */
    std::vector<uint32_t> ReadData( PixFED* pFED, uint32_t pBlockSize = 0 );
    /*!
     * \brief Read N Events 
     * \param pFED
     * \param pNEvents: number of Events to read
     * \return cData: the data in 32 bit words
     */
    std::vector<uint32_t> ReadNEvents( PixFED* pFED, uint32_t pNEvents = 1 );
    /*!
    * \brief: Initialize the Slink
    * \param: pointer to PixFED
    */ 
    void InitSlink( const PixFED* pFED);
    /*!
     * \brief Prints the status of the Slink
     * \param pFED
     */
    void PrintSlinkStatus( PixFED * pFED );

    /*!
     * \brief Get next event from data buffer
     * \param pFED
     * \return Next event
     */
    // const Event* GetNextEvent( const PixFED* pFED );
    // const Event* GetEvent( const PixFED* pFED, int i );
    // const std::vector<Event*>& GetEvents( const PixFED* pFED );

    /*! \brief Get a uHAL node object from its path in the uHAL XML address file
     * \param pFED pointer to a board description
     * \return Reference to the uhal::node object
     */
    const uhal::Node& getUhalNode( const PixFED* pFED, const std::string& pStrPath );
    /*! \brief Access to the uHAL main interface for a given board
     * \param pFED pointer to a board description
     * \return pointer to the uhal::HwInterface object
     */
    uhal::HwInterface* getHardwareInterface( const PixFED* pFED );
    /*! \brief Upload a configuration in a board FPGA
     * \param pFED pointer to a board description
     * \param numConfig FPGA configuration number to be uploaded
     * \param pstrFile path to MCS file containing the FPGA configuration
     */
    void FlashProm( PixFED* pFED, const std::string& strConfig, const char* pstrFile );
    /*! \brief Jump to an FPGA configuration
    * \param pFED pointer to a board description
    * \param numConfig FPGA configuration number
    */
    void JumpToFpgaConfig( PixFED* pFED, const std::string& strConfig );
    void DownloadFpgaConfig( PixFED* pFED, const std::string& strConfig, const std::string& strDest);
    /*! \brief Current FPGA configuration
     * \param pFED pointer to a board description
     * \return const pointer to an FPGA uploading process. NULL means that no upload is been processed.
     */
    const FpgaConfig* getConfiguringFpga( PixFED* pFED );

    /*! \brief Get the list of available FPGA configuration (or firmware images)
    * \param pFED pointer to a board description */
    std::vector<std::string> getFpgaConfigList( PixFED* pFED );

    /*! \brief Delete one Fpga configuration (or firmware image)
    * \param pFED pointer to a board description
     * \param strId Firmware image identifier*/
    void DeleteFpgaConfig( PixFED* pFED, const std::string& strId );

private:
    void toggleFitelChannels(Fitel* pFitel, bool pEnable);

    int swap_channels(int pChan);
};

#endif
