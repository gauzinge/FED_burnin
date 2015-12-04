/*!
        \file                PixFEDFWInterface.h
        \brief                           PixFEDFWInterface base class of all type of boards
        \author                          Lorenzo BIDEGAIN, Nicolas PIERRE
        \version             1.0
        \date                            28/07/14
        Support :                        mail to : lorenzo.bidegain@gmail.com, nico.pierre@icloud.com

 */

#ifndef _PixFEDFWInterface_h__
#define _PixFEDFWInterface_h__

#include <boost/thread.hpp>
#include <uhal/uhal.hpp>
#include "RegManager.h"
// #include "../Utils/Event.h"
//#include "../Utils/Data.h"
#include "../Utils/Utilities.h"
#include "../Utils/Exception.h"
// #include "../HWDescription/Definition.h"
#include "../HWDescription/PixFED.h"
#include "CtaFpgaConfig.h"

#include <iostream>
#include <fstream>

class CtaFpgaConfig;

/*!
 * \class PixFEDFWInterface
 * \brief Class separating board system FW interface from uHal wrapper
 */
class PixFEDFWInterface : public RegManager
{
    //MEMBER VARIABLES
private:
    //Data* fData; [>!< Data read storage<]

    // Size Variables for Data to read
    uint32_t fNumAcq;       // the number of acquisitions
    uint32_t fNTBM;         // the number of TBMs
    uint32_t fNCh = 2;          // the number of Channels per TBM = 2
    uint32_t fNPattern = 8;     // the number of Patterns to send per channel = 8 (8 ROCs per TBM channel)
    uint32_t fPacketSize = 2;   // the size of a packet for each acquisition = 2;

    // to be computed internally
    uint32_t fBlockSize;    // the number of bits to read from the DDR memory
    uint32_t fBlockSize32; // the number of 32-bit words to read from DDR memory
    uint32_t fNthAcq;       // the index of the current Acquistion - to be used to select the correct DDR bank
    //struct timeval fStartVeto;
    CtaFpgaConfig* fpgaConfig;
    // strings fro DDR control
    std::string fStrDDR, fStrDDRControl, fStrFull, fStrReadout;

public:
    /*!
    * \brief Constructor of the PixFEDFWInterface class
    * \param puHalConfigFileName : path of the uHal Config File
    * \param pFileHandler : pointer to file handler for saving Raw Data*/
    PixFEDFWInterface( const char* puHalConfigFileName, uint32_t pBoardId );
    /*!
    * \brief Destructor of the PixFEDFWInterface class
    */
    ~PixFEDFWInterface()
    {
        //if ( fData ) delete fData;
    }
    /*!
     * \brief set the number of TBMs
     * \param pNTBM : number of TBMs connected
     */
    void setNTBM(uint32_t pNTBM)
    {
        fNTBM = pNTBM;
    };

    //DAQ METHODS
    /*!
    * \brief Get the board type
    */
    std::string getBoardType();
    /*!
    * \brief Get the board infos
    */
    void getBoardInfo();


    /*!
     * \brief Configure the board with its Config File
     * \param pPixFED
     */
    bool ConfigureBoard( const PixFED* pPixFED );

    /*!
     * \brief Start a DAQ
     */
    void Start();
    /*!
     * \brief Stop a DAQ
     * \param pNthAcq : actual number of acquisitions
     */
    void Stop();
    /*!
     * \brief Pause a DAQ
     */
    void Pause();
    /*!
     * \brief Resume a DAQ
     */
    void Resume();
    /*!
     * \brief Read data from DAQ
     * \param pPixFED
     * \param pNthAcq : actual number of acquisitions
     * \param pBreakTrigger : if true, enable the break trigger
     * \return cNPackets: the number of packets read
     */
    uint32_t ReadData( PixFED* pPixFED );

    // EVENT HANDLING
    //const Event* GetNextEvent( const PixFED* pPixFED ) const;
    //const Event* GetEvent( const PixFED* pPixFED, int i ) const;
    //const std::vector<Event*>& GetEvents( const PixFED* pPixFED ) const;

    std::vector<uint32_t> ReadBlockRegValue( const std::string& pRegNode, const uint32_t& pBlocksize );
    bool WriteBlockReg( const std::string& pRegNode, const std::vector< uint32_t >& pValues );

    //FITEL METHODS
protected:
    //Encode/Decode FITEL FMC values
    /*!
    * \brief Encode a/several word(s) readable for a Fitel
    * \param pRegItem : RegItem containing infos (name, adress, value...) about the register to write
    * \param pFitelId : Id of the Fitel to work with
    * \param pVecReq : Vector to stack the encoded words
    */
    //void EncodeReg( const FitelRegItem& pRegItem, uint8_t pFitelId, std::vector<uint32_t>& pVecReq ); [>!< Encode a/several word(s) readable for a Fitel<]
    /*!
    * \brief Decode a word from a read of a register of the Fitel
    * \param pRegItem : RegItem containing infos (name, adress, value...) about the register to read
    * \param pFitelId : Id of the Fitel to work with
    * \param pWord : variable to put the decoded word
    */
    //void DecodeReg( FitelRegItem& pRegItem, uint8_t pCFitelId, uint32_t pWord ); [>!< Decode a word from a read of a register of the Fitel<]

    //pure methods which are defined in the proper BoardFWInterface class
    //r/w the Fitel registers
    /*!
    * \brief Write register blocks of a Fitel
    * \param pFeId : FrontEnd to work with
    * \param pVecReq : Block of words to write
    */
    //void WriteFitelBlockReg( uint8_t pFitelId, std::vector<uint32_t>& pVecReq );
    /*!
    * \brief Read register blocks of a Fitel
    * \param pFeId : FrontEnd to work with
    * \param pVecReq : Vector to stack the read words
    */
    //void ReadFitelBlockReg( uint8_t pFitelId, std::vector<uint32_t>& pVecReq );

private:
    void getFEDNetworkParameters();
    /*!
     * \brief DDR selection for DAQ
     * \param pNthAcq : actual number of acquisitions
     */
    void SelectDaqDDR( uint32_t pNthAcq );
    //I2C Methods

    /*!
     * \brief Wait for the I2C command acknowledgement
     * \param pAckVal : Expected status of acknowledgement, 1/0 -> true/false
     * \param pNcount : Number of registers at stake
     * \return boolean confirming the acknowledgement
     */
    bool I2cCmdAckWait( uint32_t pAckVal, uint8_t pNcount = 1 );
    /*!
     * \brief Send request to r/w blocks via I2C
     * \param pVecReq : Block of words to send
     * \param pWrite : 1/0 -> Write/Read
     */
    void SendBlockFitelI2cRequest( std::vector<uint32_t>& pVecReq, bool pWrite );
    /*!
     * \brief Read blocks from SRAM via I2C
     * \param pVecReq : Vector to stack the read words
     */
    void ReadI2cBlockValuesInDDR( std::vector<uint32_t>& pVecReq );
    /*!
     * \brief Enable I2C communications
     * \param pEnable : 1/0 -> Enable/Disable
     */
    void EnableI2c( bool pEnable );

    void SelectFitelDDR( uint32_t pFitelId );

    /*! Compute the size of an acquisition data block
     * \return Number of 32-bit words to be read at each iteration */
    uint32_t computeBlockSize();

    // FPGA CONFIG METHODS
public:
    void checkIfUploading();
    /*! \brief Upload a firmware (FPGA configuration) from a file in MCS format into a given configuration
     * \param strConfig FPGA configuration name
     * \param pstrFile path to MCS file
     */
    void FlashProm( const std::string& strConfig, const char* pstrFile );
    /*! \brief Jump to an FPGA configuration */
    void JumpToFpgaConfig( const std::string& strConfig );
    /*! \brief Is the FPGA being configured ?
     * \return FPGA configuring process or NULL if configuration occurs */
    const FpgaConfig* getConfiguringFpga()
    {
        return ( const FpgaConfig* )fpgaConfig;
    }
    /*! \brief Get the list of available FPGA configuration (or firmware images)*/
    std::vector<std::string> getFpgaConfigList( );
    /*! \brief Delete one Fpga configuration (or firmware image)*/
    void DeleteFpgaConfig( const std::string& strId );
};

#endif
