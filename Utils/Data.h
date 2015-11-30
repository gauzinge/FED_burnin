/*

    \file                          Data.h
    \brief                         Data handling from DAQ
    \author                        Nicolas PIERRE
    \version                       1.0
    \date                          10/07/14
    Support :                      mail to : nicolas.pierre@icloud.com

 */

#ifndef __DATA_H__
#define __DATA_H__

#include <uhal/uhal.hpp>
#include <memory>
#include <ios>
#include <istream>
#include "../Utils/Event.h"
#include "../HWDescription/BeBoard.h"
#include "../HWDescription/Definition.h"


using namespace Ph2_HwDescription;
namespace Ph2_HwInterface
{

	/*!
	 * \class Data
	 * \brief Data buffer class for CBC data
	 */
	class Data
	{
	  private:
		uint32_t fNevents;              /*! Number of Events<*/
		uint32_t fCurrentEvent;         /*! Current EventNumber in use <*/
		uint32_t fNCbc  ;               /*! Number of CBCs in the setup <*/
		uint32_t fEventSize  ;          /*! Size of 1 Event <*/

		std::vector<Event*> fEventList;

	  private:

	  public:
		/*!
		 * \brief Constructor of the Data class
		 * \param pNbCbc
		 */
		Data( ) :  fCurrentEvent( 0 ), fEventSize( 0 ) {
		}
		/*!
		 * \brief Copy Constructor of the Data class
		 */
		Data( const Data& pData );
		/*!
		 * \brief Destructor of the Data class
		 */
		~Data() {
			for ( auto pevt : fEventList )
				delete pevt;
			fEventList.clear();
		}
		/*!
		 * \brief Set the data in the data map
		 * \param *pBoard : pointer to Boat
		 * \param *pData : Data from the Cbc
		 * \param pNevents : The number of events in this acquisiton
		 */
		void Set( const BeBoard* pBoard, const std::vector<uint32_t>& pData, uint32_t pNevents, bool swapBytes = true );
		/*!
		 * \brief Reset the data structure
		 */
		void Reset();
		/*!
		 * \brief Get the next Event
		 * \param pBoard: pointer to BeBoard
		 * \return Next Event
		 */
		// cannot be const as fCurrentEvent is incremented
		const Event* GetNextEvent( const BeBoard* pBoard ) {
			return ( ( fCurrentEvent >= fEventList.size() ) ? nullptr : fEventList.at( fCurrentEvent++ ) );
		}
		const Event* GetEvent( const BeBoard* pBoard, int i ) const {
			return ( ( i >= fEventList.size() ) ? nullptr : fEventList.at( i ) );
		}
		const std::vector<Event*>& GetEvents( const BeBoard* pBoard ) const {
			return fEventList;
		}
	};

}
#endif
