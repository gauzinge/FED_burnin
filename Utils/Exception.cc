/*

   FileName :                    Exception.cc
   Content :                     Handles with exceptions
   Programmer :                  Nicolas PIERRE
   Version :                     1.0
   Date of creation :            10/06/14
   Support :                     mail to : nicolas.pierre@cern.ch

 */

#include "../Utils/Exception.h"
// #include "TROOT.h"


const char* Exception::what() const throw()
{
    std::string pStr = "HWDescription: " + fStrError;
    const char* pBuff = pStr.c_str();
    return pBuff;
}
