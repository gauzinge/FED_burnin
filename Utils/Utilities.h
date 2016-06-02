/*

    \file                           Utilities.h
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
#include <unistd.h>
#include <stdint.h>
#include <ios>
#include <istream>
#include <limits>
#include <iostream>
#include <vector>
#include <string>

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
int kbhit();

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
/*!
 * \brief converts any vector of 32 bit words to vector of 64 bit words
 * \param pVector: vector of 32 bit words
 * \return vector of 64 bit words
 */
std::vector<uint64_t> expandto64(const std::vector<uint32_t>& pVector);
/*!
 * \brief: verify if image is part of list
 * \param strImage: Image Name
 * \param lstName: List of Images
 * */
void verifyImageName( const std::string& strImage, const std::vector<std::string>& lstNames);

#endif
