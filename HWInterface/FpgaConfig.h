/*!  
   \file 		FpgaConfig.h
   \brief 		FPGA configuration by uploading the firware in MCS file format
   \version             1.0
   \author	 	Christian Bonnin
   \date		02/03/2015
   Support : 		mail to : christian.bonnin@iphc.cnrs.fr
*/
#ifndef _FPGACONFIG_H_
#define _FPGACONFIG_H_

#include "HWDescription/BeBoard.h"
using namespace Ph2_HwDescription;
namespace Ph2_HwInterface
{
	class BeBoardFWInterface;
/*!
 * \brief Upload MCS files into Flash EPROM as FPGA configuration
 * @author cbonnin
 */
	class FpgaConfig{
		public:
/*! \brief Constructor from a BeBoardFWInterface 
 * \param pbbi Reference to the BeBoardFWInterface
 */
			FpgaConfig(BeBoardFWInterface* pbbi);
/*! \brief Launch the firmware upload in a separate thread
 * \param strConfig FPGA configuration number or name
 * \param pstrFile absolute path to the MCS file
 */
			virtual void runUpload(const std::string& strConfig, const char* pstrFile) throw (std::string)=0;
/*! \brief Tells if a configuration is currently been uploaded
 * \return Configuration number or 0 if no upload is been processed
 */
			uint32_t getUploadingFpga() const {return numUploadingFpga;}
/*! \brief Percentage of the upload already done
 * \return 0 to 100 percentage value
 */
			uint32_t getProgressValue() const  {return progressValue;}
/*! \brief Description of the upload current status
 * \return String describing the upload status containing the block number and the estimated remaining time
 */
			const std::string& getProgressString() const {return progressString;}

/*! \brief Jump to an FPGA configuration
 * \param strConfig FPGA configuration number or name
 */
			virtual void jumpToImage( const std::string& strImage)=0;

		protected:

			timeval timStart, timEnd;
			uint32_t progressValue, numUploadingFpga;
			std::string progressString;
			BeBoardFWInterface* fwManager;
	};
}
#endif
