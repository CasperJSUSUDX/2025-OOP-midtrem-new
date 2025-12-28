#include "CandleStick.h"
#include "CSVReader.h"
#include <iostream>
#include <vector>

CandleStick::CandleStick(
    std::string _date,
    double _open,
    double _high,
    double _low,
    double _close)
: date(_date),
  open(_open),
  high(_high),
  low(_low),
  close(_close)
{

}

void CandleStick::drawCandleStick(std::vector<candleStickEntry> candleSticks)
{
    unsigned int height = 20;
    unsigned int width = 60;
    canvas canvas = createCanvas(height, width);
    
    drawText(canvas, height - 1, 0, "test");
    // for (candleStickEntry& cse: candleSticks)
    // {
        
    // }
}

void CandleStick::drawPixel(canvas& canvas, unsigned int x, unsigned int y, char c)
{
    // input check
    if (y > canvas.size()) return;
    if (x > canvas[y].size()) return;

    canvas[y][x] = c;
}

void CandleStick::drawText(canvas& canvas, unsigned int x, unsigned int y, std::string s)
{
    // input check
    if (y > canvas.size()) return;
    if (x > canvas[y].size()) return;

    canvas[y].replace(x, s.length(), s);
}

canvas CandleStick::createCanvas(unsigned int height, unsigned int width)
{
    canvas canvas;
    for (int i = 0; i < height; ++i)
    {
        std::string row(width, ' ');
        canvas.push_back(row);
    }
}

void CandleStick::print()
{
    std::cout << "Current time: " << date << std::endl;
    std::cout << "Open: " << open << std::endl;
    std::cout << "High: " << high << std::endl;
    std::cout << "Low: " << low << std::endl;
    std::cout << "Close: " << close << std::endl;
    std::cout << "===================" << std::endl;
}

std::string CandleStick::getDate(DateRange dateRange)
{
    std::vector<std::string> dates = CSVReader::tokenise(date, '/');
    switch (dateRange)
    {
    case DateRange::YEARLY:
        return date.substr(0, 4);
    case DateRange::MONTHLY:
        return date.substr(0, 7);
    case DateRange::DAILY:
        return date.substr(0, 10);
    default:
        return date.substr(0, 4);
    }
}