/*!

        \file                PiXFED.h
        \brief                           PiXFED Description class, configs of the PiXFED
        \author                          Lorenzo BIDEGAIN
        \date                            14/07/14
        \version                         1.0
        Support :                        mail to : lorenzo.bidegain@gmail.com

 */

#ifndef _PixFED_h__
#define _PixFED_h__

// #include "Definition.h"
#include "Fitel.h"
#include <vector>
#include <map>
#include <stdint.h>
#include <string>

typedef std::map< std::string, uint32_t > PixFEDRegMap;     /*!< Map containing the registers of a board */

/*!
 * \class PiXFED
 * \brief Read/Write PiXFED's registers on a file, handles a register map and handles a vector of Fitel which are connected to the PiXFED
 */
class PixFED
{

public:

    // C'tors: the PiXFED only needs to know about it's shelf and which BE it is
    /*!
     * \brief Default C'tor
     */
    PixFED();

    /*!
     * \brief Standard C'tor
     * \param pBeId
     */
    PixFED( uint8_t pBeId );

    /*!
    * \brief C'tor for a standard PiXFED reading a config file
    * \param pBeId
    * \param filename of the configuration file
    */
    PixFED( uint8_t pBeId, const std::string& filename );

    /*!
    * \brief Destructor
    */
    ~PixFED()
    {
    }

    // Public Methods

    /*!
    * \brief Get any register from the Map
    * \param pReg
    * \return The value of the register
    */
    uint32_t getReg( const std::string& pReg ) const;
    /*!
    * \brief Set any register of the Map, if the register is not on the map, it adds it.
    * \param pReg
    * \param psetValue
    */
    void setReg( const std::string& pReg, uint32_t psetValue );


    PixFEDRegMap getPixFEDRegMap() const
    {
        return fRegMap;
    }

    /*!
    * \brief Get the BeBoardId of the PiXFED
    * \return the PiXFED Id
    */
    uint8_t getBeId() const
    {
        return fBeId;
    }

    /*!
    * \brief Set the Be Id of the PiXFED
    * \param pBeId
    */
    void setBeId( uint8_t pBeId )
    {
        fBeId = pBeId;
    };

    std::vector<Fitel*> fFitelVector;
    /*!
    * \brief Adding a module to the vector
    * \param pFitel
    */
    void addFitel( Fitel& pFitel )
    {
        fFitelVector.push_back( &pFitel );
    }

    void addFitel( Fitel* pFitel )
    {
        fFitelVector.push_back( pFitel );
    }

    /*!
     * \brief Remove a Fitel from the vector
     * \param pFitelId
     * \return a bool which indicate if the removing was successful
     */
    bool removeFitel( uint8_t pFitelId );
    /*!
     * \brief Get a module from the vector
     * \param pFitelId
     * \return a pointer of module, so we can manipulate directly the module contained in the vector
     */
    Fitel* getFitel( uint8_t pFitelId ) const;

protected:
    //Connection Members
    uint8_t fBeId;


    PixFEDRegMap fRegMap;             /*!< Map of PiXFED Register Names vs. Register Values */

private:

    /*!
    * \brief Load RegMap from a file
    * \param filename
    */
    void loadConfigFile( const std::string& filename );
};

#endif
