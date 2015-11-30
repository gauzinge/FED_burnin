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
#include <vector>
#include <map>
#include <stdint.h>


typedef std::map< std::string, uint32_t > PixFEDRegMap;     /*!< Map containing the registers of a board */

/*!
 * \class PiXFED
 * \brief Read/Write PiXFED's registers on a file, handles a register map and handles a vector of Module which are connected to the PiXFED
 */
class PiXFED
{

  public:

	// C'tors: the PiXFED only needs to know about it's shelf and which BE it is
	/*!
	 * \brief Default C'tor
	 */
	PiXFED();

	/*!
	 * \brief Standard C'tor
	 * \param pBeId
	 */
	PiXFED( uint8_t pBeId );

	/*!
	* \brief C'tor for a standard PiXFED reading a config file
	* \param pBeId
	* \param filename of the configuration file
	*/
	PiXFED( uint8_t pBeId, const std::string& filename );

	/*!
	* \brief Destructor
	*/
	~PiXFED() {
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


	PixFEDRegMap getPixFEDRegMap() const {
		return fRegMap;
	}

	/*!
	* \brief Get the BeBoardId of the PiXFED
	* \return the PiXFED Id
	*/
	uint8_t getBeId() const {
		return fBeId;
	}

	/*!
	* \brief Set the Be Id of the PiXFED
	* \param pBeId
	*/
	void setBeId( uint8_t pBeId ) {
		fBeId = pBeId;
	};

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
