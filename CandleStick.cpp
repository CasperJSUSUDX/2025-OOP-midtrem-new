#include "CandleStick.h"
#include "CSVReader.h"
#include <iostream>
#include <algorithm>
#include <cctype>

CandleStick::CandleStick(std::string filename)
{
    dateRange dateRange = inputDateRange();
    CSVReader::readCSV(filename);
}

dateRange CandleStick::inputDateRange()
{
    std::string input;
    std::cout << "Please type the date range of the summary. (daily/monthly/yearly)" << std::endl;
    std::cin >> input;
    
    std::transform(input.begin(), input.end(), input.begin(), ::tolower);
    if (input == "yearly") return dateRange::YEARLY;
    if (input == "monthly") return dateRange::MONTHLY;
    if (input == "daily") return dateRange::DAILY;
    return dateRange::UNKNOWN;
}