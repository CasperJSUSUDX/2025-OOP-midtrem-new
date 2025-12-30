#include "OrderBook.h"
#include "CSVReader.h"
#include <algorithm>
#include <iostream>
#include <cmath>
#include <fstream>
#include <map>

/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);
}

/** return vector of all know products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts()
{
    std::vector<std::string> products;

    std::map<std::string,bool> prodMap;

    for (OrderBookEntry& e : orders)
    {
        prodMap[e.product] = true;
    }
    
    // now flatten the map to a vector of strings
    for (auto const& e : prodMap)
    {
        products.push_back(e.first);
    }

    return products;
}
/** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, 
                                        std::string product, 
                                        std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry& e : orders)
    {
        if (e.orderType == type && 
            e.product == product && 
            e.timestamp == timestamp )
            {
                orders_sub.push_back(e);
            }
    }
    return orders_sub;
}


double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders)
{
    double max = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price > max)max = e.price;
    }
    return max;
}


double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders)
{
    double min = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price < min)min = e.price;
    }
    return min;
}

std::string OrderBook::getEarliestTime()
{
    return orders[0].timestamp;
}

std::string OrderBook::getNextTime(std::string timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp > timestamp) 
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        next_timestamp = orders[0].timestamp;
    }
    return next_timestamp;
}

void OrderBook::insertOrder(OrderBookEntry& order)
{
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

void OrderBook::appendOrder(OrderBookEntry& order)
{
    orders.push_back(order);
}
void OrderBook::sortOrder()
{
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

std::vector<candlestickEntry> OrderBook::generateCandlesticks(
    std::string startTimestamp,
    std::string endTimestamp,
    unsigned int timeInterval,
    std::string product,
    OrderBookType candlestickType
)
{
    std::vector<candlestickEntry> candlesticks;
    std::vector<OrderBookEntry> orders_sub;
    std::string nextTimestamp = OrderBookEntry::calcNextTimestamp(startTimestamp, timeInterval);

    for (OrderBookEntry& obe: orders)
    {
        std::string timestamp = obe.timestamp.substr(0, 19);
        // filter
        if (obe.product != product) continue;
        if (obe.orderType != candlestickType) continue;
        if (timestamp < startTimestamp) continue;
        if (timestamp > endTimestamp) break;

        // Time block check
        while (timestamp >= nextTimestamp)
        {
            if (!orders_sub.empty())
            {
                candlestickEntry cse {
                    startTimestamp,
                    nextTimestamp,
                    orders_sub[0].price,
                    OrderBook::getHighPrice(orders_sub),
                    OrderBook::getLowPrice(orders_sub),
                    orders_sub.back().price
                };
                candlesticks.push_back(cse);
                orders_sub.clear();
            }

            startTimestamp = nextTimestamp;
            nextTimestamp = OrderBookEntry::calcNextTimestamp(startTimestamp, timeInterval);

            if (timestamp > endTimestamp) break;
        }

        // push the obe to cache
        if (timestamp >= startTimestamp && timestamp < nextTimestamp)
        {
            orders_sub.push_back(obe);
        }
    }

    // process remain obes
    if (!orders_sub.empty())
    {
        candlestickEntry cse {
            startTimestamp,
            orders_sub[orders_sub.size()-1].timestamp.substr(0, 19),
            orders_sub[0].price,
            OrderBook::getHighPrice(orders_sub),
            OrderBook::getLowPrice(orders_sub),
            orders_sub[orders_sub.size()-1].price
        };
        candlesticks.push_back(cse);
    }

    return candlesticks;
}

unsigned int OrderBook::calcTimeInterval(std::string& timeStamp1, std::string& timeStamp2)
{
    std::string timeStamp1_p = timeStamp1.substr(11, 8);
    std::string timeStamp2_p = timeStamp2.substr(11, 8);

    int hour1 = std::stoi(timeStamp1_p.substr(0, 2));
    int minute1 = std::stoi(timeStamp1_p.substr(3, 2));
    int second1 = std::stoi(timeStamp1_p.substr(7, 2));
    int hour2 = std::stoi(timeStamp2_p.substr(0, 2));
    int minute2 = std::stoi(timeStamp2_p.substr(3, 2));
    int second2 = std::stoi(timeStamp2_p.substr(7, 2));

    unsigned int hour = std::abs(hour1 - hour2) * 3600;
    unsigned int minute = std::abs(minute1 - minute2) * 60;
    unsigned int second = std::abs(second1 - second2);

    return hour + minute + second;
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
// asks = orderbook.asks
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, 
                                                 product, 
                                                 timestamp);
// bids = orderbook.bids
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, 
                                                 product, 
                                                    timestamp);

    // sales = []
    std::vector<OrderBookEntry> sales; 

    // I put in a little check to ensure we have bids and asks
    // to process.
    if (asks.size() == 0 || bids.size() == 0)
    {
        std::cout << " OrderBook::matchAsksToBids no bids or asks" << std::endl;
        return sales;
    }

    // sort asks lowest first
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    // sort bids highest first
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    // for ask in asks:
    std::cout << "max ask " << asks[asks.size()-1].price << std::endl;
    std::cout << "min ask " << asks[0].price << std::endl;
    std::cout << "max bid " << bids[0].price << std::endl;
    std::cout << "min bid " << bids[bids.size()-1].price << std::endl;
    
    for (OrderBookEntry& ask : asks)
    {
    //     for bid in bids:
        for (OrderBookEntry& bid : bids)
        {
    //         if bid.price >= ask.price # we have a match
            if (bid.price >= ask.price)
            {
    //             sale = new order()
    //             sale.price = ask.price
            OrderBookEntry sale{ask.price, 0, timestamp, 
                product, 
                OrderBookType::asksale};

                if (bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType =  OrderBookType::asksale;
                }
            
    //             # now work out how much was sold and 
    //             # create new bids and asks covering 
    //             # anything that was not sold
    //             if bid.amount == ask.amount: # bid completely clears ask
                if (bid.amount == ask.amount)
                {
    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
    //                 # can do no more with this ask
    //                 # go onto the next ask
    //                 break
                    break;
                }
    //           if bid.amount > ask.amount:  # ask is completely gone slice the bid
                if (bid.amount > ask.amount)
                {
    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 # we adjust the bid in place
    //                 # so it can be used to process the next ask
    //                 bid.amount = bid.amount - ask.amount
                    bid.amount =  bid.amount - ask.amount;
    //                 # ask is completely gone, so go to next ask                
    //                 break
                    break;
                }


    //             if bid.amount < ask.amount # bid is completely gone, slice the ask
                if (bid.amount < ask.amount && 
                   bid.amount > 0)
                {
    //                 sale.amount = bid.amount
                    sale.amount = bid.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 # update the ask
    //                 # and allow further bids to process the remaining amount
    //                 ask.amount = ask.amount - bid.amount
                    ask.amount = ask.amount - bid.amount;
    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
    //                 # some ask remains so go to the next bid
    //                 continue
                    continue;
                }
            }
        }
    }
    return sales;             
}
