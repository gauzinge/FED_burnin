/*

    \file                          Utilities.h
    \brief                         Some objects that might come in handy
    \author                        Nicolas PIERRE
    \version                       1.0
    \date                          10/06/14
    Support :                      mail to : nicolas.pierre@icloud.com

 */

#ifndef __UTILITIES_H__
#define __UTILITIES_H__

#include <math.h>
#include <sys/time.h>
#include <stdint.h>
#include <ios>
#include <istream>
#include <limits>
#include "../HWDescription/Definition.h"
#include <iostream>

/*!
 * \brief Get time took since the start
 * \param pStart : Variable taking the start
 * \param pMili : Result in milliseconds/microseconds -> 1/0
 * \return The time took
 */
long getTimeTook( struct timeval& pStart, bool pMili );
/*!
 * \brief Flush the content of the input stream
 * \param in : input stream
 */
void myflush( std::istream& in );
/*!
 * \brief Wait for Enter key press
 */
void mypause();
/*!
 * \brief get Current Time & Date
 */
const std::string currentDateTime();
/*!
 * \brief Error Function for SCurve Fit
 * \param x: array of values
 * \param p: parameter array
 * \return function value
 */
double MyErf( double* x, double* par );
/*!
 * \brief converts any char array to int by automatically detecting if it is hex or dec
 * \param pRegValue: parsed xml parmaeter char*
 * \return converted integer
 */
uint32_t convertAnyInt( const char* pRegValue );

#endif
