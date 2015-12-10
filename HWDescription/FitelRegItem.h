/*!

        \file            CbcRegItem.h
        \brief                   CbcRegItem description, contents of the structure CbcRegItem with is the value of the CbcRegMap
        \author                  Lorenzo BIDEGAIN
        \version                 1.0
        \date                    25/06/14
        Support :                mail to : lorenzo.bidegain@cern.ch

 */

#ifndef _FitelRegItem_h_
#define _FitelRegItem_h_

#include <stdint.h>


/*!
 * \struct FitelRegItem
 * \brief Struct for FitelRegisterItem that is identified by  Address, DefaultValue, Value
 */
struct FitelRegItem
{
    uint8_t fAddress;
    uint8_t fDefValue;
    uint8_t fValue;
    char fPermission;
};

#endif
