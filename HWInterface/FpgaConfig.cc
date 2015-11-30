/* Copyright 2014 Institut Pluridisciplinaire Hubert Curien
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   FileName : 		FpgaConfig.cc
   Content : 		FPGA configuration
   Programmer : 	Christian Bonnin
   Version : 		
   Date of creation : 2014-07-10
   Support : 		mail to : christian.bonnin@iphc.cnrs.fr
*/
#include <sys/stat.h>//file size
#include <time.h>
#include <fstream>
#include <boost/format.hpp>
#include <boost/thread.hpp>
//#include <uhal/uhal.hpp>
#include "BeBoardFWInterface.h"
#include "FpgaConfig.h"

using namespace std;

namespace Ph2_HwInterface
{
    
FpgaConfig::FpgaConfig(BeBoardFWInterface* pbbfi){
	fwManager = pbbfi;
	numUploadingFpga=0;
	progressValue=0;
	progressString="";
}
	
}

