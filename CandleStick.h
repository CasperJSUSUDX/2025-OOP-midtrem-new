#pragma once

#include <string>
#include <vector>

struct candleStickEntry
{
    std::string startTimestamp;
    std::string endTimestamp;
    double open;
    double high;
    double low;
    double close;
};

/** x is from left to right
 *  y is from top to bottom
 */
using canvas = std::vector<std::string>;

class CandleStick
{
    public:
        CandleStick();
        /** print out a list of candle stick information by the input vector */
        static void printCandleStick(std::vector<candleStickEntry> candleSticks);

        std::string date;
        double open;
        double high;
        double low;
        double close;
};