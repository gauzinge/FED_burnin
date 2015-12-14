#ifndef _Data_h__
#define _Data_h__

#include <string>
#include <vector>
#include <iostream>

class Data
{
public:
    //C'tor
    Data()
    {
        fData.clear();
        fTBM_core_error_ctr = fTBM_index_error_ctr = fPayload_error_ctr = 0;
    };
    // D'tor
    ~Data()
    {
        fData.clear();
    };
    // for checking
    void add(int pEventCounter, std::vector<uint32_t> pData);
    void clear();
    void check();
    // get error counters
    int getTBM_index_errors()
    {
        return fTBM_index_error_ctr;
    };
    int getTBM_core_errors()
    {
        return fTBM_core_error_ctr;
    };
    int getPayload_errors()
    {
        return fPayload_error_ctr;
    };

private:
    std::vector<uint32_t> fData;
    int fBoardCounter;
    int fEventCounter;

    int fTBM_index_error_ctr;
    int fTBM_core_error_ctr;
    int fPayload_error_ctr;
};

#endif
