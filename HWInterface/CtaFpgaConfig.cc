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

   FileName : 		CtaFpgaConfig.cc
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
#include "BeBoardFWInterface.h"
#include "CtaFpgaConfig.h"

using namespace std;

#define SECURE_MODE_PASSWORD	"RuleBritannia"


namespace Ph2_HwInterface
{
    
CtaFpgaConfig::CtaFpgaConfig(BeBoardFWInterface* pbbi):
	FpgaConfig(pbbi),
    	lNode(dynamic_cast< const fc7::MmcPipeInterface & > (fwManager->getUhalNode( "buf_cta" )))
{
}

    void CtaFpgaConfig::runUpload(const std::string& strImage, const char* szFile) throw (std::string){
	vector<string> lstNames=lNode.ListFilesOnSD();
	for (int iName=0; iName<lstNames.size(); iName++){
		if (!strImage.compare(lstNames[iName]))
			numUploadingFpga=iName+1;
		break;
	}
       	progressValue=0;
       	progressString="Starting upload";
	boost::thread(&CtaFpgaConfig::dumpFromFileIntoSD, this, strImage, szFile);
    }

    void CtaFpgaConfig::dumpFromFileIntoSD(const std::string& strImage, const char* pstrFile){
	fc7::XilinxBitFile bitFile(pstrFile);
	lNode.FileToSD(strImage, bitFile, &progressValue, &progressString);
	progressValue=100;
	lNode.RebootFPGA(strImage, SECURE_MODE_PASSWORD);
    }

    void CtaFpgaConfig::jumpToImage( const std::string& strImage){
	lNode.RebootFPGA(strImage, SECURE_MODE_PASSWORD);
    }

    std::vector<std::string>  CtaFpgaConfig::getFirmwareImageNames(){
	return lNode.ListFilesOnSD ();
		
    }

    void CtaFpgaConfig::deleteFirmwareImage(const std::string& strId){
	lNode.DeleteFromSD(strId, SECURE_MODE_PASSWORD);
    }
}

