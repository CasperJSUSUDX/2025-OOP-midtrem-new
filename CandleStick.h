#pragma once

#include <string>
#include <vector>

enum DateRange
{
    DAILY,
    MONTHLY,
    YEARLY,
    UNKNOWN
};

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
        CandleStick(std::string _date, double _open, double _high, double _low, double _close);
        void printCandleStick(std::vector<candleStickEntry> candleSticks);
        std::string getDate(DateRange dateRange);
        void print();

        std::string date;
        double open;
        double high;
        double low;
        double close;

    private:
        /** edit the char at (x,y) */
        void drawPixel(canvas& canvas, unsigned int x, unsigned int y, char c);
        /** add a text start at (x,y) */
        void drawText(canvas& canvas, unsigned int x, unsigned int y, std::string s);
        /** return a new canvas */
        canvas createCanvas(unsigned int height, unsigned int width);
};