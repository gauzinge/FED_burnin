#include "Data.h"


void Data::add(int pEventCounter, std::vector<uint32_t> pData)
{
    fData.insert(fData.end(), pData.begin(), pData.end());
    fBoardCounter++;
    fEventCounter = pEventCounter;
}

void Data::clear()
{
    fBoardCounter = 0;
    fEventCounter = 0;
    fData.clear();
}

void Data::check()
{
    // Data format
    // 36 bit word in 2 32 bit words
    // only 8 LSB of the first word used for header
    // 2 MSB of 2nd 32 bit word unused
    // 29-24: 6 bit TBM index (0-48)
    // 23:22 unused
    // 21:20 TMB core
    // 19:16 unused
    // 15:0 payload 0xaaaa or 0x5555

    // check that the number of words / vec.size() is even!
    if (fData.size() % 2 != 0) std::cout << "Warning, odd number of words in the data vector for " << fBoardCounter << " , something is not right!" << std::endl;

    // next break the stuff up in blocks of 2x32 bit words
    for (int i = 0; i < fData.size() / 2; i++)
    {
        int index = 2 * i;
        uint16_t header = fData.at(index) & 0x0000000F;
        uint16_t tbm_index = (fData.at(index + 1) & 0x3F000000) >> 24;
        uint16_t tbm_core = (fData.at(index + 1) & 0x00300000) >> 20;
        uint16_t payload = (fData.at(index + 1) & 0x0000FFFF);

        //std::cout << " TBM index " << tbm_index << " core " << tbm_core << " payload " << std::hex << payload << std::dec << std::endl;
        //now for each set of words check that the value is within permissible range
        if ((tbm_index == 0) || (tbm_index > 48))
        {
            std::cout << "ERROR: TBM index not within permissible range!" << std::endl;
            fTBM_index_error_ctr++;
        }
        if (!(tbm_core == 1 || tbm_core == 2 ))
        {
            std::cout  << "ERROR: TBM core not within permissible range!" << std::endl;
            fTBM_core_error_ctr++;
        }
        if (!(payload == 0xaaaa || payload == 0x5555))
        {
            std::cout  << "ERROR: payload does not match the expected patterns" << std::endl;
            fPayload_error_ctr++;
        }
    }
}
