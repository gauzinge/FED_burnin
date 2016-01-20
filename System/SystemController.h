/*!

        \file                    SystemController.h
        \brief                                   Controller of the System, overall wrapper of the framework
        \author                                  Nicolas PIERRE
        \version                 1.0
        \date                                    10/08/14
        Support :                                mail to : lorenzo.bidegain@cern.ch, nico.pierre@icloud.com

 */


#ifndef __SYSTEMCONTROLLER_H__
#define __SYSTEMCONTROLLER_H__

#include "../Utils/Utilities.h"
#include "../Utils/pugixml.hpp"
#include "../Utils/ConsoleColor.h"
#include "../HWInterface/PixFEDFWInterface.h"
#include "../HWInterface/PixFEDInterface.h"
#include <iostream>
#include <vector>
#include <map>
#include <stdlib.h>
# include <string.h>

typedef std::map<std::string, uint32_t> SettingsMap;    /*!< Maps the settings */


/*!
 * \class SystemController
 * \brief Create, initialise, configure a predefined HW structure
 */
class SystemController
{
public:
    PixFEDInterface*        fFEDInterface;
    PixFEDFWMap             fFWMap;
    std::vector<PixFED*> fPixFEDVector;
    SettingsMap    fSettingsMap;
public:
    /*!
     * \brief Constructor of the SystemController class
     */
    SystemController();
    /*!
     * \brief Destructor of the SystemController class
     */
    ~SystemController();
    /*!
     * \brief Initialize the Hardware via a config file
     * \param pFilename : HW Description file
     *\param os : ostream to dump output
     */
    void InitializeHw( const std::string& pFilename, std::ostream& os = std::cout );
    /*!
     * \brief Initialize the settings
     * \param pFilename :   settings file
     *\param os : ostream to dump output
    */
    void InitializeSettings( const std::string& pFilename, std::ostream& os = std::cout );
    /*!
     * \brief Configure the Hardware with XML file indicated values
     */
    void ConfigureHw( std::ostream& os = std::cout );
    /*!
     * \brief: Halt Hardware and put it back to safe mode!
     */
    void HaltHw( std::ostream& os = std::cout );
    /*!
     * \brief Run a DAQ
     * \param pBeBoard
     * \param pNthAcq
     */
    void Run( PixFED* pPixFED, uint32_t pNthAcq );

    /*!
     * \brief converts any char array to int by automatically detecting if it is hex or dec
     * \param pRegValue: parsed xml parmaeter char*
     * \return converted integer
     */
    uint32_t convertAnyInt( const char* pRegValue )
    {
        if ( std::string( pRegValue ).find( "0x" ) != std::string::npos ) return static_cast<uint32_t>( strtoul( pRegValue , 0, 16 ) );
        if ( std::string( pRegValue ).find( "0b" ) != std::string::npos ) return static_cast<uint32_t>( strtoull( pRegValue , 0, 2 ) );
        else return static_cast<uint32_t>( strtoul( pRegValue , 0, 10 ) );

    }

    /*!
     * \brief Get next event from data buffer
     * \param pBoard
     * \return Next event
     */
    //const Event* GetNextEvent( const BeBoard* pBoard )
    //{
    //return fPixFEDFWInterface->GetNextEvent( pBoard );
    //}
    //const Event* GetEvent( const BeBoard* pBoard, int i ) const
    //{
    //return fPixFEDFWInterface->GetEvent( pBoard, i );
    //}
    //const std::vector<Event*>& GetEvents( const BeBoard* pBoard ) const
    //{
    //return fPixFEDFWInterface->GetEvents( pBoard );
    //}

    /*!
     * \brief Initialize the hardware via  XML config file
     * \param pFilename : HW Description file
     *\param os : ostream to dump output
     */
    void parseHWxml( const std::string& pFilename, std::ostream& os = std::cout );

protected:
private:
    /*!
     * \brief Initialize the hardware via JSON config file
     * \param pFilename : HW Description file
     *\param os : ostream to dump output
     */
    void parseSettingsxml( const std::string& pFilename, std::ostream& os );
    /*!
     * \brief Initialize the settins via  XML file
     * \param pFilename : settings Description file
     *\param os : ostream to dump output
     */
};

#endif
