/*!

        \file                   Fitel.h
        \brief                  Fitel Description class, config of the Fitels
        \author                 Lorenzo BIDEGAIN
        \version                1.0
        \date                   25/06/14
        Support :               mail to : lorenzo.bidegain@gmail.com

 */


#ifndef Fitel_h__
#define Fitel_h__

#include "FitelRegItem.h"
#include "../Utils/Exception.h"
#include <iostream>
#include <map>
#include <string>
#include <stdint.h>
#include <utility>
#include <set>
#include <vector>

// Fitel2 Chip HW Description Class


typedef std::map < std::string, FitelRegItem > FitelRegMap;
typedef std::pair <std::string, FitelRegItem> FitelRegPair;

/*!
 * \class Fitel
 * \brief Read/Write Fitel's registers on a file, contains a register map
 */
class Fitel
{

public:
    std::vector<int> fChEnableVec;

    // C'tors with object FE Description
    Fitel( uint8_t pBeId, uint8_t pFMCId, uint8_t pFitelId, const std::string& filename );


    // Default C'tor
    Fitel();

    // Copy C'tor
    Fitel( const Fitel& Fitelobj );

    // D'Tor
    ~Fitel();

    void loadfRegMap( const std::string& filename );

    /*!
    * \brief Get any register from the Map
    * \param pReg
    * \return The value of the register
    */
    uint8_t getReg( const std::string& pReg ) const;
    /*!
    * \brief Set any register of the Map
    * \param pReg
    * \param psetValue
    */
    void setReg( const std::string& pReg, uint8_t psetValue );
    /*!
    * \brief Get any registeritem of the Map
    * \param pReg
    * \return  RegItem
    */
    FitelRegItem getRegItem( const std::string& pReg );
    /*!
    * \brief Write the registers of the Map in a file
    * \param filename
    */
    void saveRegMap( const std::string& filename );

    /*!
    * \brief Get the Map of the registers
    * \return The map of register
    */
    FitelRegMap& getRegMap()
    {
        return fRegMap;
    }
    const FitelRegMap& getRegMap() const
    {
        return fRegMap;
    }
    /*!
    * \brief Get the Fitel Id
    * \return The Fitel ID
    */
    uint8_t getFitelId() const
    {
        return fFitelId;
    }
    /*!
     * \brief Set the Fitel Id
     * \param pFitelId
     */
    void setFitelId( uint8_t pFitelId )
    {
        fFitelId = pFitelId;
    }

    uint8_t getFMCId() const
    {
        return fFMCId;
    }
    /*!
     * \brief Set the Fitel Id
     * \param pFitelId
     */
    void setFMCId( uint8_t pFMCId )
    {
        fFMCId = pFMCId;
    }

    uint8_t getBeId() const
    {
        return fBeId;
    }

    void setBeId( uint8_t pBeId )
    {
        fBeId = pBeId;
    }

protected:

    uint8_t fBeId;
    uint8_t fFMCId;
    uint8_t fFitelId;

    // Map of Register Name vs. RegisterItem that contains: Page, Address, Default Value, Value
    FitelRegMap fRegMap;

};


/*!
 * \struct FitelComparer
 * \brief Compare two Fitel by their ID
 */
struct FitelComparer
{

    bool operator()( const Fitel& Fitel1, const Fitel& Fitel2 ) const;

};

/*!
 * \struct RegItemComparer
 * \brief Compare two pair of Register Name Versus FitelRegItem by the Page and Adress of the FitelRegItem
 */
struct RegItemComparer
{

    bool operator()( const FitelRegPair& pRegItem1, const FitelRegPair& pRegItem2 ) const;

};


#endif
