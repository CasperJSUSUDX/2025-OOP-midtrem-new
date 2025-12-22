#pragma once

#include <string>

enum DateRange
{
    DAILY,
    MONTHLY,
    YEARLY,
    UNKNOWN
};

class CandleStickEntry
{
    public:
        CandleStickEntry(std::string _date, double _open, double _high, double _low, double _close);
        std::string getDate(DateRange dateRange);
        void print();
    
    private:
        std::string date;
        double open;
        double high;
        double low;
        double close;
};