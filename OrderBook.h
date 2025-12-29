#pragma once

#include "OrderBookEntry.h"
#include "CandleStick.h"
#include "CSVReader.h"
#include <string>
#include <vector>

class OrderBook
{
    public:
    /** construct, reading a csv data file */
        OrderBook(std::string filename);
    /** return vector of all know products in the dataset*/
        std::vector<std::string> getKnownProducts();
    /** return vector of Orders according to the sent filters*/
        std::vector<OrderBookEntry> getOrders(OrderBookType type, 
                                              std::string product, 
                                              std::string timestamp);

        /** returns the earliest time in the orderbook*/
        std::string getEarliestTime();
        /** returns the next time after the 
         * sent time in the orderbook  
         * If there is no next timestamp, wraps around to the start
         * */
        std::string getNextTime(std::string timestamp);

        void insertOrder(OrderBookEntry& order);

        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

        static double getHighPrice(std::vector<OrderBookEntry>& orders);
        static double getLowPrice(std::vector<OrderBookEntry>& orders);

        /** generate a vector includes candleStickEntry between two gave timestamp */
        std::vector<candleStickEntry> generateCnadleSticks(
            std::string startTimestamp,
            std::string endTimestamp,
            unsigned int timeInterval,
            std::string product,
            OrderBookType candleStickType
        );

        /** calculate the interval seconds between two time stamp */
        static unsigned int calcTimeInterval(std::string& timeStamp1, std::string& timeStamp2);

    private:
        std::vector<OrderBookEntry> orders;


};
