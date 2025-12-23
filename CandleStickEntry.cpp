#include "CandleStickEntry.h"
#include "CSVReader.h"
#include <iostream>
#include <vector>

CandleStickEntry::CandleStickEntry(
    std::string _date,
    double _open,
    double _high,
    double _low,
    double _close)
: date(CSVReader::tokenise(_date, ' ')[0]),
  open(_open),
  high(_high),
  low(_low),
  close(close)
{

}

void CandleStickEntry::print()
{
    std::cout << "Date: " << date << std::endl;
    std::cout << "Open: " << open << std::endl;
    std::cout << "High: " << high << std::endl;
    std::cout << "Low: " << low << std::endl;
    std::cout << "Close: " << close << std::endl;
}

std::string CandleStickEntry::getDate(DateRange dateRange)
{
    std::vector<std::string> dates = CSVReader::tokenise(date, '/');
    switch (dateRange)
    {
    case DateRange::YEARLY:
        return dates[0];
    case DateRange::MONTHLY:
        return dates[0] + '/' + dates[1];
    case DateRange::DAILY:
        return dates[0] + '/' + dates[1] + '/' + dates[2];
    default:
        return dates[0];
    }
}