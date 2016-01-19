#ifndef commandlinetool_h_
#define commandlinetool_h_

#include <ctime>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include "uhal/uhal.hpp"
#include "../Utils/Utilities.h"
#include "../HWDescription/PixFED.h"
#include "../HWInterface/PixFEDInterface.h"
#include "../System/SystemController.h"
#include "../Utils/Data.h"

//my variables
const char* cHWFile;
SystemController cSystemController;

int cNAcq;
int tbm_index_error_ctr;
int tbm_core_error_ctr;
int payload_error_ctr;

std::ofstream logger;
std::string logfilename;

std::string userInput;

std::vector<std::string> splitUserInput;
std::vector<std::string> validInput;

//my functions

void defineValidInput();
void splitInput(std::string userInput);
bool checkInput(std::string userInput);
void printPromt();

bool fileexists(std::string filename);


void DAQinfo();
void startDAQ();
void stopDAQ();
void pauseDAQ();
void resumeDAQ();
void readData();
void loopDAQ(int loops);

void configDAQ(const char* configfile);
void flashFPGA();
void switchFW();
void listFW();

void getTransparent();
void getSpy();
void getFIFO1();
void dumpAll();

void quit();

#endif
