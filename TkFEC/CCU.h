#ifndef _CCU_H__
#define _CCU_H__

#include <iostream>
#include <vector>
#include <string>

struct i2c
{
public:
    i2c(uint8_t pChannel, uint8_t pAddress, uint8_t pValue) : fChannel(pChannel), fAddress(pAddress), fValue(pValue) {}
    uint8_t fChannel;
    uint8_t fAddress;
    uint8_t fValue;
};

struct CCU
{
public:
    CCU(uint32_t pAddress, uint32_t pRingId) : fAddress(pAddress), fRingId(pRingId) {}
    uint32_t fAddress;
    uint32_t fRingId;
    std::vector<i2c*> fChannelVector;
};



#endif
