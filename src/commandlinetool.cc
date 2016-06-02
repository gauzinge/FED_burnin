#include "commandlinetool.hh"

int main(int argc, char* argv[] )
{
    tbm_index_error_ctr = 0;
    tbm_core_error_ctr = 0;
    payload_error_ctr = 0;

    cHWFile = argv[1];
    std::cout << "HW Description File: " << cHWFile << std::endl;

    // for logging
    /*
    time_t rawtime;
    struct tm * timeinfo;
    char buffer[80];

    time (&rawtime);
    timeinfo = localtime(&rawtime);

    strftime(buffer, 80, "%d-%m-%Y_%H:%M:%S", timeinfo);


    logfilename = "logfile_";
    logfilename += buffer;
    logfilename += ".txt";
    logger.open(logfilename);
    std::cout << "Dumping log to: " << logfilename << std::endl;
    uhal::setLogLevelTo(uhal::Debug());
    */
    // instantiate System Controller
    //    cSystemController;

    // initialize map of settings so I can know the proper number of acquisitions and TBMs
    cSystemController.InitializeSettings(cHWFile, std::cout);

    // initialize HWdescription from XML, beware, settings have to be read first
    cAmc13Controller.InitializeAmc13( cHWFile, std::cout );
    cSystemController.InitializeHw(cHWFile, std::cout);

     // configure the HW
    cAmc13Controller.ConfigureAmc13( std::cout );
    cSystemController.ConfigureHw(std::cout );
    auto cSetting = cSystemController.fSettingsMap.find("NAcq");
    cNAcq = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 10;

    //settings are as generic as possible and should be configured later
    cSetting = cSystemController.fSettingsMap.find("ChannelOfInterest");
    cChannelOfInterest = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 0;
    
    cSetting = cSystemController.fSettingsMap.find("ROCOfInterest");
    cROCOfInterest = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 0;
    

    // we start without sending triggers, it's the job of the operator to enable them
    cAmc13Controller.fAmc13Interface->EnableBGO(0);
    cAmc13Controller.fAmc13Interface->StopL1A();


    //clear userinput
    userInput = "";
    
    //prepare for loop
    defineValidInput();


    while( userInput != "q" &&  userInput != "quit")
      {
	//show the user a promt to choose his actions from       
	if(splitUserInput.size() == 0)
	  {
	    printPromt();
	    //get user input
	    getline(std::cin,userInput);
	    //sanitize input
	    splitInput(userInput);
	  }

	//if invalid input do nothing and show promt again
	if(splitUserInput.size() == 0)
	  {
	    std::cout << "No arguments were provided, please go again" << std::endl; 
	  }
	else if(!checkInput(splitUserInput[0]))
	  {
	    std::cout << splitUserInput[0]<<" is an invalid command! Try again" << std::endl;
	    splitUserInput.clear();
	  }
	else if(splitUserInput[0] == "i" || splitUserInput[0] == "info")
	  {
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "DAQinfo()" <<  std::endl;
	    DAQinfo();
	  }
	else if(splitUserInput[0] == "channel" || splitUserInput[0] == "ch")
	  {
	    std::string::size_type sz;
	    int channel;
	    if(splitUserInput.size() > 1 && !(checkInput(splitUserInput[1])))
	      {
		try
		  {
		    channel = stoi(splitUserInput[1], &sz);
		  }
		catch(std::invalid_argument&)
		  {
		    std::cout << "There must have been an error in the command string. Try again!" << std::endl;
		    splitUserInput.clear();
		    continue;
		  }
		
		if(splitUserInput[1].substr(sz) == "\0")
		  {
		    cChannelOfInterest = channel;
		    changeChannelOfInterest(cChannelOfInterest);
		    splitUserInput.erase(splitUserInput.begin(), splitUserInput.begin()+2);
		  }
		else
		  {
		    std::cout << "There must have been an error in the command string. Try again!" << std::endl;
		    splitUserInput.clear();
		    continue;
		  }
	      }
	    else
	      {
		splitUserInput.erase(splitUserInput.begin());
	      }
	   
	  }
	else if(splitUserInput[0] == "roc")
	  {
	    std::string::size_type sz;
	    int roc;
	    if(splitUserInput.size() > 1 && !(checkInput(splitUserInput[1])))
	      {
		try
		  {
		    roc = stoi(splitUserInput[1], &sz);
		  }
		catch(std::invalid_argument&)
		  {
		    std::cout << "There must have been an error in the command string. Try again!" << std::endl;
		    splitUserInput.clear();
		    continue;
		  }
		
		if(splitUserInput[1].substr(sz) == "\0")
		  {
		    cROCOfInterest = roc;
		    changeROCOfInterest(cROCOfInterest);
		    splitUserInput.erase(splitUserInput.begin(), splitUserInput.begin()+2);
		  }
		else
		  {
		    std::cout << "There must have been an error in the command string. Try again!" << std::endl;
		    splitUserInput.clear();
		    continue;
		  }
	      }
	    else
	      {
		splitUserInput.erase(splitUserInput.begin());
	      }
	  }
	else if(splitUserInput[0] == "L1start")
	  {
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "L1start()" <<  std::endl;
	    L1start();
	  }
	else if(splitUserInput[0] == "L1stop")
	  {
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "L1stop()" <<  std::endl;
	    L1stop();
	  }
	else if(splitUserInput[0] == "L1burst")
	  {
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "L1burst()" <<  std::endl;
	    L1burst();
	  }
	else if(splitUserInput[0] == "s" || splitUserInput[0] == "start")
	  {
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "startDAQ()" <<  std::endl;
	    startDAQ();
	  }
	else if(splitUserInput[0] == "x" || splitUserInput[0] == "stop")
	  {
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "stopDAQ()" <<  std::endl;
	    stopDAQ();
	  }
	else if(splitUserInput[0] == "p" || splitUserInput[0] == "pause")
	  {
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "pauseDAQ()" <<  std::endl;
	    pauseDAQ();
	  }
	else if(splitUserInput[0] == "r" || splitUserInput[0] == "resume")
	  {
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "resumeDAQ()" <<  std::endl;
	    resumeDAQ();
	  }
	else if(splitUserInput[0] == "read")
	  {
	    // READ data from DAQ
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "readDAQ()" <<  std::endl;
	    readData();
	  }
	else if(splitUserInput[0] == "a")
	  {
	    //check if a value for loops was supplied
	    std::string::size_type sz;
	    int loops;
	    if(splitUserInput.size() > 1 && !(checkInput(splitUserInput[1])))
	      {
		try
		  {
		    loops = stoi(splitUserInput[1], &sz);
		  }
		catch(std::invalid_argument&)
		  {
		    std::cout << "There must have been an error in the command string. Try again!" << std::endl;
		    splitUserInput.clear();
		    continue;
		  }

		if(splitUserInput[1].substr(sz) == "\0")
		  {
		    cNAcq = loops;
		    splitUserInput.erase(splitUserInput.begin(), splitUserInput.begin()+1);
		  }
		else
		  {
		    std::cout << "There must have been an error in the command string. Try again!" << std::endl;
		    splitUserInput.clear();
		    continue;
		  }
	      }
	    else
	      {
		splitUserInput.erase(splitUserInput.begin());
	      }
	    std::cout << "loopDAQ() || cNAcq =" << cNAcq <<  std::endl;
	    loopDAQ(cNAcq);
	  }
	else if(splitUserInput[0] == "c" || splitUserInput[0] == "conf" || splitUserInput[0] == "configure" )
	  {
	    if(splitUserInput.size() > 1 && !checkInput(splitUserInput[1]))
	      {
		if(fileexists(splitUserInput[1]))
		  {
		    //the configuration is loaded from a valid file path
		    const char* newHWFile = splitUserInput[1].c_str();
	    	    splitUserInput.erase(splitUserInput.begin(),splitUserInput.begin()+1);
		    std::cout << "configDAQ() with " << newHWFile <<  std::endl;
		    configDAQ(newHWFile);
		  }
		else
		  {
		    //neither the command after the configure is valid,
		    //nor can an existing file be found.
		    //we terminate the loop for now
		    std::cout << splitUserInput[0] << " " <<  splitUserInput[1] <<" Error: HWdescription file does not exist" << std::endl;
		    splitUserInput.clear();
		  }
	      }
	    //no additional argument is presented
	    else
	      {
		splitUserInput.erase(splitUserInput.begin());
		std::cout << "configDAQ() with " << cHWFile <<  std::endl;
		configDAQ(cHWFile);
	      }
	  }
	else if(splitUserInput[0] == "list" )
	  {
	    // LIST firmware images
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "listFW()" <<  std::endl;
	    listFW();
	  }
	else if(splitUserInput[0] == "flash" )
	  {
	    // FLASH FPGA
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "flashFPGA()" <<  std::endl;	    
	    flashFPGA();
	  }
	else if(splitUserInput[0] == "fpgaconf" )
	  {
	    // jump to an FPGA configuration (this could be a bit trickier)
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "switchFW()" <<  std::endl;	    
	    switchFW();
	  }
	else if(splitUserInput[0] == "trans")
	  {
	    // TRANSPARENT buffer dump
	    //	    getTransparent();
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "getTransparent()" <<  std::endl;
	    getTransparent();
	  }
	else if(splitUserInput[0] == "spy")
	  {
	    // SPY FIFO dump
	    //	    getSpy();
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "getSpy()" <<  std::endl;	    
	    getSpy();
	  }
	else if(splitUserInput[0] == "fifo1" || splitUserInput[0] == "one")
	  {
	    // FIFO1 dump
	    //	    getFIFO1();
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "getFIFO1()" <<  std::endl;	    
	    getFIFO1();
	  }
	else if(splitUserInput[0] == "OSD")
	  {
	    // OSD read back
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "getOSD" <<  std::endl;	    
	    getOSD();
	  }
	else if(splitUserInput[0] == "dumpallfifo" || splitUserInput[0] == "dump")
	  {
	    // dump all FIFOs and transparent buffer
	    //	    dumpAll();
	    splitUserInput.erase(splitUserInput.begin());
	    std::cout << "dumpAll()" <<  std::endl;	    
	    dumpAll();
	  }
	else if(splitUserInput[0] == "findphase" || splitUserInput[0] == "phase")
	  {
	    // find good phase
	    splitUserInput.erase(splitUserInput.begin());	    
	    findPhases();
	  }
	else if(splitUserInput[0] == "light")
	  {
	    // find light on fibres
	    splitUserInput.erase(splitUserInput.begin());
	    findLight();
	  }
	else if(splitUserInput[0] == "q" || splitUserInput[0] == "quit")
	  {
	    // QUIT/END program
	    break;
	  }

      }
    //    logger.close();

}

void defineValidInput(){

  // board info
  validInput.push_back("i");      
  validInput.push_back("info");   
  // quit/end program
  validInput.push_back("q");      
  validInput.push_back("quit");   
  // start DAQ
  validInput.push_back("s");      
  validInput.push_back("start");  
  // stop DAQ
  validInput.push_back("x");      
  validInput.push_back("stop");      
  // pause DAQ
  validInput.push_back("p");      
  validInput.push_back("pause");
  // resume DAQ
  validInput.push_back("r");      
  validInput.push_back("resume");
  // acquisition loop 
  validInput.push_back("a");      
  // (re)configure FED
  validInput.push_back("c");      
  validInput.push_back("conf");
  validInput.push_back("configure");
  // find phases
  validInput.push_back("findPhase");
  // read data
  validInput.push_back("read");
  // read transparent FIFO
  validInput.push_back("trans");
  // read spy FIFO
  validInput.push_back("spy");
  // read FIFO1
  validInput.push_back("fifo1");
  validInput.push_back("one");
  // OSD read back
  validInput.push_back("OSD");
  // dump all FIFOs
  validInput.push_back("dumpallfifo");
  validInput.push_back("dump");
  // flash PROM
  validInput.push_back("flash");
  // Jump to FPGA Config
  validInput.push_back("fpgaconf");
  // List all firmware images
  validInput.push_back("list");
  // Check for light on fibres
  validInput.push_back("light");
  // Find good phases for readout
  validInput.push_back("findphase");
  validInput.push_back("phase");
  // set channel of interest
  validInput.push_back("channel");
  validInput.push_back("ch");
  // set ROC of interest
  validInput.push_back("roc");
  // start L1A triggers
  validInput.push_back("L1start");
  // stop L1A triggers
  validInput.push_back("L1stop");
  // burst L1A triggers
  validInput.push_back("L1burst");  
}

void splitInput(std::string userInput){

  //split into different words
  std::istringstream iss (userInput);
  while(!iss.eof())
    {
      std::string uI;
      getline(iss,uI,' ');
      //std::cout << uI << std::endl;
      splitUserInput.push_back(uI);
    }
}

bool checkInput(std::string userInput){

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

void printPromt(){

  std::cout << "Current settings: " << std::endl;
  std::cout << "Channel of Interest: " << cChannelOfInterest << std::endl;
  std::cout << "ROC of Interest: " << cROCOfInterest << std::endl;

  std::cout << "*****************************************************" << std::endl;

  std::cout << "Please choose an operation:" << std::endl;
  std::cout << "\t [i/info] for board info" << std::endl;
  //general settings
  std::cout << "General readout settings:" << std::endl;
  std::cout << "\t [channel/ch xx] to define the channel of interest" << std::endl; 
  std::cout << "\t [roc xx] to define the roc of interest" << std::endl; 
  //connection tests
  std::cout << "Connection Tests: " << std::endl;
  std::cout << "\t [light] to check light on fibre" << std::endl;
  std::cout << "\t [phase] to find correct phase settings" << std::endl;
  //trigger control
  std::cout << "AMC13 trigger control:" << std::endl;
  std::cout << "\t [L1start] to start L1A triggers " << std::endl;
  std::cout << "\t [L1stop] to stop L1A triggers " << std::endl;
  std::cout << "\t [L1burst] to send a L1A trigger burst " << std::endl;
  std::cout << "\t \t [L1burst XXX] to send xxx consecutive L1A trigger burst " << std::endl;
  //DAQ control
  std::cout << "DAQ control:" << std::endl;
  std::cout << "\t [s/start] to start DAQ on all FEDs" << std::endl;
  std::cout << "\t [x/stop] to stop DAQ on all FEDs" << std::endl;
  std::cout << "\t [p/pause] to pause DAQ on all FEDs" << std::endl;
  std::cout << "\t [r/resume] to resume DAQ on all FEDs" << std::endl;
  std::cout << "\t [read] to read data from DAQ" << std::endl;
  std::cout << "\t [a] for data acquisition" << std::endl;
  std::cout << "\t \t or [a xxx] for data acquisition of xxx loops" << std::endl;
  //DAQ config
  std::cout << "DAQ configuration" << std::endl;
  std::cout << "\t [c/conf] to re-load configuration from disk" << std::endl;
  std::cout << "\t [list] to show all available firmware versions on all boards" << std::endl; //perhaps here it would be better to also ask for a board nummer or something to identify the correct FED
  std::cout << "\t [flash] to execute phase finding" << std::endl;
  std::cout << "\t [fpgaconf] to execute phase finding" << std::endl;
  //DQM
  std::cout << "\t [trans] to dump transparent buffer data" << std::endl;
  std::cout << "\t [spy] to dump spy FIFO data" << std::endl;
  std::cout << "\t [one] to dump FIFO1 data" << std::endl;
  std::cout << "\t [OSD] to read back ODS data" << std::endl;
  std::cout << "\t [dump] to dump all three FIFOs" << std::endl;

  std::cout << "\t [q/quit] to to quit" << std::endl;

}


bool fileexists(std::string filename){
  std::ifstream ifile(filename);
  return ifile;
}

void DAQinfo(){
  // get the board INFO of all boards and start the acquistion
  std::cout << "DAQ info" << std::endl;
  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      cSystemController.fFEDInterface->getBoardInfo(cFED);
    }
}

void L1start(){
  // start the L1A triggers
  cAmc13Controller.fAmc13Interface->StartL1A();
}

void L1stop(){
  // stop the L1A triggers
  cAmc13Controller.fAmc13Interface->StopL1A();
}

void L1burst(){
  // send a burst of L1A triggers
  cAmc13Controller.fAmc13Interface->BurstL1A();
}

void L1burst(int triggers){

  for(int i = 0; i < triggers; ++i)
    {
      L1burst();
    }
}

void startDAQ(){
  // START DAQ on all FEDs
  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      cSystemController.fFEDInterface->Start(cFED);
    }
}

void stopDAQ(){
  // STOP DAQ on all FEDs
  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      cSystemController.fFEDInterface->Stop(cFED);
    }
}

void pauseDAQ(){
  // PAUSE DAQ on all FEDs
  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      cSystemController.fFEDInterface->Pause(cFED);
    }
}

void resumeDAQ(){
  // RESUME DAQ on all FEDs
  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      cSystemController.fFEDInterface->Resume(cFED);
    }
}


void readData(){
  //READ a single data acquisition
  Data cData;
  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      cData.add(0, cSystemController.fFEDInterface->ReadData(cFED,2));
    }
}


void loopDAQ(int loops){
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
    }
  std::cout << std::endl << "Finished recording " << iAcq << " events!" << std::endl;
  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      cSystemController.fFEDInterface->Stop(cFED);
      //std::cout << "Finished reading Data!" << std::endl;
    }
}

void configDAQ(const char* configfile){
  // CONFIGURE DAQ
  std::cout << "Loading new DAQ calibration from " << *configfile << std::endl;
  // we delete the old settings
  cSystemController.fPixFEDVector.clear();
  //do these have to go as well?
  cSystemController.fSettingsMap.clear();
  cSystemController.fFWMap.clear();
	    
  // load new settings

  // initialize map of settings so I can know the proper number of acquisitions and TBMs
  cSystemController.InitializeSettings(configfile, std::cout);
	    
  // initialize HWdescription from XML, beware, settings have to be read first
  cSystemController.InitializeHw(configfile, std::cout);
	    
  // configure the HW
  cSystemController.ConfigureHw(std::cout );
  auto cSetting = cSystemController.fSettingsMap.find("NAcq");
  int cNAcq = (cSetting != std::end(cSystemController.fSettingsMap)) ? cSetting->second : 10;
}

void flashFPGA(){
  std::cout << "This function is currently not supported. \n Please check back later." << std::endl;
}

void switchFW(){
  std::cout << "This function is currently not supported. \n Please check back later." << std::endl;
}

void listFW(){
  // list all saved firmware images for all FEDs

  //TODO: this thing is totally broken. when a SystemController is initialised with
  //      the FITEL receivers specified we get a segmentation fault.
  //      As soon as this bug I can fix this function can be fixed

  std::cout << "This function is currently not supported. \n Please check back later." << std::endl;

  // std::string cHWFileForFWUpload = "settings/FWUpload.xml";
  // SystemController cFWSystemController;
  // cFWSystemController.InitializeHw(cHWFileForFWUpload);
  // PixFED* pBoard = cFWSystemController.fPixFEDVector.at(0);
  // std::vector<std::string> FWNames = cFWSystemController.fFEDInterface->getFpgaConfigList(pBoard);

  // std::cout << FWNames.size() << " firmware images on SD card:" << std::endl;
  // for (auto &name : FWNames)
  //   {
  //     std::cout <<"\t -" << name << std::endl;
  //   }
  
}

void getTransparent(){
  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      // read TRANSPARENT fifo
      cSystemController.fFEDInterface->readTransparentFIFO(cFED);
    }
}

void getSpy(){
  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      // read SPY fifo
      cSystemController.fFEDInterface->readSpyFIFO(cFED);
    }
}

void getFIFO1(){
  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      // read fifo 1
      cSystemController.fFEDInterface->readFIFO1(cFED);
    }
}

void getOSD(){
  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      // read fifo 1
      cSystemController.fFEDInterface->readOSDWord(cFED, cROCOfInterest, cChannelOfInterest);
    }
}

void dumpAll(){
  Data cData;
  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      // read TRANSPARENT fifo
      cSystemController.fFEDInterface->readTransparentFIFO(cFED);
      // read SPY fifo
      cSystemController.fFEDInterface->readSpyFIFO(cFED);
      // read fifo 1
      cSystemController.fFEDInterface->readFIFO1(cFED);
      // read data
      //      cData.add(0, cSystemController.fFEDInterface->ReadData(cFED,2));
    }
}

void findLight(){
    for (auto& cFED : cSystemController.fPixFEDVector)
    {
      std::cout <<"FED loop" << std::endl;
      for (auto& cFitel : cFED->fFitelVector)
	{
	  std::cout <<"Fitel loop" << std::endl;
	  cSystemController.fFEDInterface->ReadADC(cFitel, cChannelOfInterest, true);
        }
    }
}

void findPhases(){

  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      cSystemController.fFEDInterface->getBoardInfo(cFED);
      cSystemController.fFEDInterface->findPhases(cFED, cChannelOfInterest);
    }
}

void changeChannelOfInterest(int CoI){

  for (auto& cFED : cSystemController.fPixFEDVector)
    {
      cSystemController.fFEDInterface->WriteBoardReg(cFED, "fe_ctrl_regs.fifo_config.channel_of_interest", CoI);
    }
}

void changeROCOfInterest(int RoI){

 for (auto& cFED : cSystemController.fPixFEDVector)
    {
      cSystemController.fFEDInterface->WriteBoardReg(cFED, "fe_ctrl_regs.fifo_config.OSD_ROC_Nr", RoI );
    }
}


void quit(){}
