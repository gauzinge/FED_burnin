#ifndef commandlinetool_h_
#define commandlinetool_h_

#include <ctime>
#include <cstring>
#include <string>
#include <sstream>
#include <fstream>
#include <chrono>
#include <thread>
#include "uhal/uhal.hpp"
#include "../Utils/Utilities.h"
#include "../HWDescription/PixFED.h"
#include "../HWInterface/PixFEDInterface.h"
#include "../System/SystemController.h"
#include "../AMC13/Amc13Controller.h"
#include "../Utils/Data.h"

//my variables

int cChannelOfInterest;
void changeChannelOfInterest(int CoI);
int cROCOfInterest;
void changeROCOfInterest(int RoI);

const char* cHWFile;
SystemController cSystemController;
Amc13Controller cAmc13Controller;

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

void L1start();
void L1stop();
void L1burst();
void L1burst(int triggers);

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
void getOSD();
void getOSD(int RoI, int CoI);
void dumpAll();

void findLight();
void findPhases();

void quit();

#endif
