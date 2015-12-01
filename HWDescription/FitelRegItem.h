/*!

        \file            CbcRegItem.h
        \brief                   CbcRegItem description, contents of the structure CbcRegItem with is the value of the CbcRegMap
        \author                  Lorenzo BIDEGAIN
        \version                 1.0
        \date                    25/06/14
        Support :                mail to : lorenzo.bidegain@cern.ch

 */

#ifndef _CbcRegItem_h__
#define _CbcRegItem_h__

#include <stdint.h>

namespace Ph2_HwDescription
{

	/*!
	 * \struct CbcRegItem
	 * \brief Struct for CbcRegisterItem that is identified by Page, Address, DefaultValue, Value
	 */
	struct CbcRegItem
	{

		uint8_t fPage;
		uint8_t fAddress;
		uint8_t fDefValue;
		uint8_t fValue;

	};
}

#endif
