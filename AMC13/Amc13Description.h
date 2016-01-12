#ifndef _AMC13_DESCRIPTION_h__
#define _AMC13_DESCRIPTION_h__

#include <map>
#include <string>
#include <vector>
#include <stdint.h>
#include <iostream>

typedef std::map<std::string, uint32_t> RegMap;

// a little forward declaration!
struct Trigger;
struct BGO;

class Amc13Description
{
public:
    Amc13Description();
    ~Amc13Description();

    uint32_t getReg(int pTounge, std::string& pReg);
    void setReg(int pTounge, std::string& pReg, uint32_t pValue);
    RegMap getRegMap(int pTounge)
    {
        return (pTounge == 1) ? fT1map : fT2map;
    }
    int Id;

    void setTrigger(int pType, int pRate, int pRules);
    void setTrigger( Trigger* pTrigger);
    Trigger* getTrigger();

    void addBGO( int pChannel, bool pStatus, int pPrescale, int pBX );
    void addBGO( BGO* pBGO );
    BGO* getBGO( int pPos );

    void setAMCMask(const std::vector<int>& pMask);

private:
    RegMap fT1map;
    RegMap fT2map;
    std::vector<int> fAMCMask;
    std::vector<BGO*> fBGOs;
    Trigger* fTrigger;
};

struct BGO
{
    BGO(int pChannel, bool pStatus, int pPrescale, int pBX) : fChannel( pChannel ), fStatus( pStatus ), fPrescale( pPrescale ), fBX( pBX ) {}
    int fChannel;
    bool fStatus;
    int fPrescale;
    int fBX;
};

struct Trigger
{
    Trigger(int pType, int pRate, int pRules) : fType( pType ), fRate( pRate ), fRules( pRules ) {}
    int fType;
    int fRate;
    int fRules;
};

#endif
