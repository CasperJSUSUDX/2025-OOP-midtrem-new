#include "CandleStickEntry.h"
#include "CSVReader.h"
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

std::string CandleStickEntry::getDate(dateRange dateRange)
{
    std::vector dates = CSVReader::tokenise(date, '/');
    switch (dateRange)
    {
    case dateRange::YEARLY:
        return dates[0];
    case dateRange::MONTHLY:
        return dates[0] + '/' + dates[1];
    case dateRange::DAILY:
        return dates[0] + '/' + dates[1] + '/' + dates[2];
    default:
        return dates[0];
    }
}