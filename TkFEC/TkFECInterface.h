/*
This file is part of Fec Software project.

Fec Software is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

Fec Software is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Fec Software; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

Copyright 2002 - 2003, Frederic DROUHIN - Universite de Haute-Alsace, Mulhouse-France
*/
#include <iostream>
#include <sstream>
#include <fstream>

using std::cout;
using std::endl;

#include <stdio.h>    // fopen snprintf
#include <stdlib.h>
#include <string.h>   // strcmp
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <vector>
#include <string>
#include <time.h>



#include "ServerAccess.h"
#include "SimpleCommand.h"
#include "MultiplexingServer.h"
#include "VMELock.h"

//port address
//const int portaddress = 2002;

using namespace std;

// address table
//map< string, int > fecAddressMap;               // key = sector
//map< string, unsigned int > fecAddressMap;       // key = sector
//map< string, unsigned int > ringAddressMap;
//map< string, unsigned int > ccuAddressMap;
//map< string, unsigned int > channelAddressMap;  // key = group

class TkFECInterface
{
public:
    TkFECInterface(const std::string& pHardwareId, const std::string& pUri, const std::string& pAddressTable);
    ~TkFECInterface();

    string writeI2C(unsigned int fecAddress, unsigned int ringAddress, unsigned int ccuAddress, unsigned int channelAddress, unsigned int deviceAddress, unsigned int deviceValue);

    string readI2C(unsigned int fecAddress, unsigned int ringAddress, unsigned int ccuAddress, unsigned int channelAddress, unsigned int deviceAddress);


    int RunInteracitve(unsigned int fecAddress, unsigned int ringAddress, unsigned int channelAddress, unsigned int ccuAddress, unsigned int piaChannelAddress);



private:
    FecAccess* ffecAccess;
    long floop;
    enumDeviceType fmodeType = PHILIPS ;
    unsigned long ftms;

private:
    //void readAddressMaps(const string filename);
    //string printAddressMap();
    void tokenize(const string& str, vector<string>& tokens, const string& symbols);

    string handle(vector<string> tokens, string sector, string group, FecAccess *fecAccess, unsigned int fecAddress, unsigned int ringAddress, unsigned int ccuAddress, unsigned int channelAddress, unsigned int piaChannelAddress, long loop );
};
