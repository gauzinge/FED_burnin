#include <ctime>
#include <cstring>
#include <sstream>
#include "uhal/uhal.hpp"
#include "../Utils/Utilities.h"
#include "../HWDescription/PixFED.h"
#include "../HWInterface/PixFEDInterface.h"
#include "../System/SystemController.h"
#include "../Utils/Data.h"

bool checkInput(std::string userInput, std::vector<std::string> validInput){

  for ( auto& s : validInput)
    {
      if( s.compare(userInput) == 0)
	{
	  // we have a valid input
	  return true;
	}
    }
  // the user input does not match any valid input string
  return false;
}



int main(int argc, char* argv[] )
{
    int tbm_index_error_ctr = 0;
    int tbm_core_error_ctr = 0;
    int payload_error_ctr = 0;

    const char* cHWFile = argv[1];
    std::cout << "HW Description File: " << cHWFile << std::endl;

    // for logging
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%d-%m-%Y_%H:%M:%S", timeinfo);

    std::ofstream logger;
    std::string logfilename = "logfile_";
    logfilename += buffer;
    logfilename += ".txt";
    logger.open(logfilename);
    std::cout << "Dumping log to: " << logfilename << std::endl;
    uhal::setLogLevelTo(uhal::Debug());

    // instantiate System Controller
    SystemController cSystemController;

    // initialize map of settings so I can know the proper number of acquisitions and TBMs
    cSystemController.InitializeSettings(cHWFile, std::cout);

    // initialize HWdescription from XML, beware, settings have to be read first
    cSystemController.InitializeHw(cHWFile, std::cout);

    // configure the HW
    cSystemController.ConfigureHw(std::cout );
    auto cSetting = cSystemController.fSettingsMap.find("NAcq");
    int cNAcq = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 10;

    std::string userInput = "";
    
    std::vector<std::string> validUserInputs;
    // board info
    validUserInputs.push_back("i");      
    validUserInputs.push_back("info");   
    // quit/end program
    validUserInputs.push_back("q");      
    validUserInputs.push_back("quit");   
    // start DAQ
    validUserInputs.push_back("s");      
    validUserInputs.push_back("start");  
    // stop DAQ
    validUserInputs.push_back("x");      
    validUserInputs.push_back("stop");      
    // pause DAQ
    validUserInputs.push_back("p");      
    validUserInputs.push_back("pause");
    // resume DAQ
    validUserInputs.push_back("r");      
    validUserInputs.push_back("resume");
    // acquisition loop 
    validUserInputs.push_back("a");      
    // (re)configure FED
    validUserInputs.push_back("c");      
    validUserInputs.push_back("conf");
    validUserInputs.push_back("configure");
    // find phases
    validUserInputs.push_back("findPhase");
    // read data
    validUserInputs.push_back("read");
    // read transparent FIFO
    validUserInputs.push_back("trans");
    // read spy FIFO
    validUserInputs.push_back("spy");
    // read FIFO1
    validUserInputs.push_back("fifo1");
    validUserInputs.push_back("one");
    // dump all FIFOs
    validUserInputs.push_back("dumpallfifo");
    // flash PROM
    validUserInputs.push_back("flash");
    // Jump to FPGA Config
    validUserInputs.push_back("fpgaconf");


    while( userInput != "q" &&  userInput != "quit")
      {
	std::cout << "Please choose an operation:" << std::endl;
	std::cout << "\t [i/info] for board info" << std::endl;
	//DAQ control
	std::cout << "DAQ control:" << std::endl;
	std::cout << "\t [s/start] to start DAQ on all FEDs" << std::endl;
	std::cout << "\t [x/stop] to stop DAQ on all FEDs" << std::endl;
	std::cout << "\t [p/pause] to pause DAQ on all FEDs" << std::endl;
	std::cout << "\t [r/resume] to resume DAQ on all FEDs" << std::endl;
	std::cout << "\t [read] to read data from DAQ" << std::endl;
	std::cout << "\t [a] for data acquisition" << std::endl;
	//DAQ config
	std::cout << "DAQ configuration" << std::endl;
	std::cout << "\t [c/conf] to re-load configuration from disk" << std::endl;
	std::cout << "\t [flash] to execute phase finding" << std::endl;
	std::cout << "\t [fpgaconf] to execute phase finding" << std::endl;
	//DQM
	std::cout << "\t [trans] to dump transparent buffer data" << std::endl;
	std::cout << "\t [spy] to dump spy FIFO data" << std::endl;
	std::cout << "\t [fifo1/one] to dump FIFO1 data" << std::endl;
	std::cout << "\t [dumpallfifo] to dump all three FIFOs" << std::endl;

	std::cout << "\t [q/quit] to to quit" << std::endl;

	getline(std::cin,userInput);

	std::cout << "You entred: " << userInput << std::endl;
	//sanitize input


	//if invalid input do nothing and show promt again
	if(!checkInput(userInput,validUserInputs))
	  {
	    std::cout << userInput <<" is an invalid command! Try again" << std::endl;
	  }
	else if(userInput == "i")
	  {
	    // get the board INFO of all boards and start the acquistion
	    for (auto& cFED : cSystemController.fPixFEDVector)
	      {
		cSystemController.fFEDInterface->getBoardInfo(cFED);
	      }
	  }
	else if(userInput == "s" || userInput == "start")
	  {
	    // START DAQ on all FEDs
	    for (auto& cFED : cSystemController.fPixFEDVector)
	      {
		cSystemController.fFEDInterface->Start(cFED);
	      }
	  }
	else if(userInput == "x" || userInput == "stop")
	  {
	    // STOP DAQ on all FEDs
	    for (auto& cFED : cSystemController.fPixFEDVector)
	      {
		cSystemController.fFEDInterface->Stop(cFED);
	      }
	  }
	else if(userInput == "p" || userInput == "pause")
	  {
	    // PAUSE DAQ on all FEDs
	    for (auto& cFED : cSystemController.fPixFEDVector)
	      {
		cSystemController.fFEDInterface->Pause(cFED);
	      }
	  }
	else if(userInput == "r" || userInput == "resume")
	  {
	    // RESUME DAQ on all FEDs
	    for (auto& cFED : cSystemController.fPixFEDVector)
	      {
		cSystemController.fFEDInterface->Resume(cFED);
	      }
	  }
	else if(userInput == "read")
	  {
	    // READ data from DAQ
	  }
	else if(userInput == "a")
	  {
	    for (auto& cFED : cSystemController.fPixFEDVector)
	      {
		cSystemController.fFEDInterface->Start(cFED);
	      }

	    // loop over the number of acquisitions
	    uint32_t iAcq = 0;
	    bool running = true;
	    while ( running )
	      {
		//std::cout << std::endl << BOLDRED << "Acquisition: " << iAcq << RESET << "\r";
		Data cData;
		for (auto& cFED : cSystemController.fPixFEDVector)
		  {
		    //std::cout << BOLDGREEN << "Data for FED " << +cFED->getBeId() << RESET << std::endl;
		    cData.add(iAcq, cSystemController.fFEDInterface->ReadData(cFED));
		  }
		iAcq++;
		if (iAcq < cNAcq && cNAcq > 0 )running = true;
		else if (cNAcq == 0 ) running = true;
		else running = false;


		if (iAcq % 1000 == 0)
		  {
		    cData.check();
		    tbm_index_error_ctr += cData.getTBM_index_errors();
		    tbm_core_error_ctr += cData.getTBM_core_errors();
		    payload_error_ctr += cData.getPayload_errors();
		    std::stringstream output;
		    std::time_t result = std::time(nullptr);
		    output <<  " Acquisition: " <<  iAcq << " ERROR summary: "
			   << " TBM index errors: " << tbm_index_error_ctr
			   << " TBM core errors:  " << tbm_core_error_ctr
			   << " Payload errors:   " << payload_error_ctr << " ";
		    std::cout << output.str() << "\r";
		    logger << output.str() << " " << std::asctime(std::localtime(&result));
		    logger.close();
		    logger.open(logfilename, std::ofstream::app);
		  }
	      }
	    std::cout << std::endl << "Finished recording " << iAcq << " events!" << std::endl;
	    for (auto& cFED : cSystemController.fPixFEDVector)
	      {
		cSystemController.fFEDInterface->Stop(cFED);
		//std::cout << "Finished reading Data!" << std::endl;
	      }
	    logger.close();
	  }
	else if(userInput == "c" || userInput == "conf" || userInput == "configure" )
	  {
	    // CONFIGURE DAQ

	    // we delete the old settings
	    cSystemController.fPixFEDVector.clear();
	    //do these have to go as well?
	    cSystemController.fSettingsMap.clear();
	    cSystemController.fFWMap.clear();
	    
	    // load new settings
	    // initialize map of settings so I can know the proper number of acquisitions and TBMs
	    cSystemController.InitializeSettings(cHWFile, std::cout);
	    
	    // initialize HWdescription from XML, beware, settings have to be read first
	    cSystemController.InitializeHw(cHWFile, std::cout);
	    
	    // configure the HW
	    cSystemController.ConfigureHw(std::cout );
	    auto cSetting = cSystemController.fSettingsMap.find("NAcq");
	    int cNAcq = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 10;

	  }
	else if(userInput == "flash" )
	  {
	    // FLASH FPGA
	  }
	else if(userInput == "fpgaconf" )
	  {
	    // jump to an FPGA configuration
	  }
	else if(userInput == "trans")
	  {
	    // TRANSPARENT buffer dump
	  }
	else if(userInput == "spy")
	  {
	    // SPY FIFO dump
	  }
	else if(userInput == "fifo1" || userInput == "one")
	  {
	    // FIFO1 dump
	  }
	else if(userInput == "dumpallfifo")
	  {
	    // dump all FIFOs and transparent buffer
	  }
	else if(userInput == "q" || userInput == "quit")
	  {
	    // QUIT/END program
	  }

      }
}
