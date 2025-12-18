#pragma once

#include "CandleStickEntry.h"
#include <string>
#include <vector>

class CandleStick
{
    public:
        CandleStick(std::string filename);
    private:
        dateRange inputDateRange();
        std::vector<CandleStickEntry> candleSticks;
};