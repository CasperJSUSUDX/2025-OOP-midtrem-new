// Made by myself - start
#pragma once

#include <string>
#include <vector>

struct candlestickEntry
{
    std::string startTimestamp;
    std::string endTimestamp;
    double open;
    double high;
    double low;
    double close;
};

class Candlestick
{
    public:
        Candlestick();
        /** print out a list of candle stick information by the input vector */
        static void printCandlestick(std::vector<candlestickEntry> candlesticks);
};
// Made by myself - end