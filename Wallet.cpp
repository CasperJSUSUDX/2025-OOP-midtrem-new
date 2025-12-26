#include "Wallet.h"
#include "CSVReader.h"
#include <iostream>
#include <fstream>
#include <algorithm>

Wallet::Wallet(std::string _uuid, std::string walletString)
: uuid(_uuid)
{
    // convert the data in the string and add to this wallet 
    std::vector<std::string> tokens = CSVReader::tokenise(walletString, '|');
    for (std::string& token: tokens)
    {
        std::vector<std::string> elements = CSVReader::tokenise(token, ':');
        std::string currency = elements[0];
        double amount = std::stod(elements[1]); 
        currencies[currency] = amount;
    }
}

void Wallet::insertCurrency(std::string type, double amount)
{
    double balance;
    if (amount < 0)
    {
        throw std::exception{};
    }
    if (currencies.count(type) == 0) // not there yet
    {
        balance = 0;
    }
    else { // is there 
        balance = currencies[type];
    }
    balance += amount; 
    currencies[type] = balance;

    // Add log line in the cache
    std::string logLine = "insert," + type + "," + std::to_string(amount);
    operatesCache.push_back(logLine);
}

bool Wallet::removeCurrency(std::string type, double amount)
{
    if (amount < 0)
    {
        return false; 
    }
    if (currencies.count(type) == 0) // not there yet
    {
        //std::cout << "No currency for " << type << std::endl;
        return false;
    }
    else { // is there - do  we have enough
        if (containsCurrency(type, amount))// we have enough
        {
            //std::cout << "Removing " << type << ": " << amount << std::endl;
            currencies[type] -= amount;
            
            std::string logLine = "remove," + type + "," + std::to_string(amount);
            operatesCache.push_back(logLine);
            return true;
        } 
        else // they have it but not enough.
            return false; 
    }
}

bool Wallet::containsCurrency(std::string type, double amount)
{
    if (currencies.count(type) == 0) // not there yet
        return false;
    else 
        return currencies[type] >= amount;
    
}

void Wallet::processSale(OrderBookEntry& sale)
{
    std::vector<std::string> currs = CSVReader::tokenise(sale.product, '/');
    // ask
    if (sale.orderType == OrderBookType::asksale)
    {
        double outgoingAmount = sale.amount;
        std::string outgoingCurrency = currs[0];
        double incomingAmount = sale.amount * sale.price;
        std::string incomingCurrency = currs[1];

        currencies[incomingCurrency] += incomingAmount;
        currencies[outgoingCurrency] -= outgoingAmount;

        // Add log line in the cache
        std::string logLine = "trade," + outgoingCurrency + "," + std::to_string(outgoingAmount) + "," + incomingCurrency + "," + std::to_string(incomingAmount);
        operatesCache.push_back(logLine);
    }
    // bid
    if (sale.orderType == OrderBookType::bidsale)
    {
        double incomingAmount = sale.amount;
        std::string incomingCurrency = currs[0];
        double outgoingAmount = sale.amount * sale.price;
        std::string outgoingCurrency = currs[1];

        currencies[incomingCurrency] += incomingAmount;
        currencies[outgoingCurrency] -= outgoingAmount;

        // Add log line in the cache
        std::string logLine = "trade," + outgoingCurrency + "," + std::to_string(outgoingAmount) + "," + incomingCurrency + "," + std::to_string(incomingAmount);
        operatesCache.push_back(logLine);
    }
}

bool Wallet::canFulfillOrder(OrderBookEntry order)
{
    std::vector<std::string> currs = CSVReader::tokenise(order.product, '/');
    // ask
    if (order.orderType == OrderBookType::ask)
    {
        double amount = order.amount;
        std::string currency = currs[0];
        std::cout << "Wallet::canFulfillOrder " << currency << " : " << amount << std::endl;

        return containsCurrency(currency, amount);
    }
    // bid
    if (order.orderType == OrderBookType::bid)
    {
        double amount = order.amount * order.price;
        std::string currency = currs[1];
        std::cout << "Wallet::canFulfillOrder " << currency << " : " << amount << std::endl;
        return containsCurrency(currency, amount);
    }


    return false; 
}

void Wallet::logInCSV()
{
    std::string filename = uuid + ".csv";
    std::ofstream writeFile(filename, std::ios::app);
    if (writeFile.is_open())
    {
        
        for (std::string operate: operatesCache)
        {
            writeFile << operate + '\n';
        }
        writeFile.close();
    }
}

void Wallet::updateUserWalletCSV()
{
    std::ifstream readWalletTable("walletTable.csv");
    std::map<std::string, std::string> table;
    if (readWalletTable.is_open())
    {
        std::string line;
        while(std::getline(readWalletTable, line))
        {
            std::vector<std::string> tokens = CSVReader::tokenise(line, ',');
            table[tokens[0]] = tokens[1];
        }
        table[uuid] = storeInString();
        readWalletTable.close();
    }

    std::ofstream writeWalletTable("walletTable.csv", std::ios::trunc);
    if (writeWalletTable.is_open())
    {
        for (std::pair<std::string, std::string> pair: table)
        {
            std::string newLine = pair.first + ',';
            if (pair.first == uuid)
            {
                newLine += storeInString();
            }
            else
            {
                newLine += pair.second + '\n';
            }
            writeWalletTable << newLine;
        }
        writeWalletTable.close();
    }
}

void Wallet::showTansitionOrTradingHistory(unsigned int pieces)
{
    std::ifstream readFile(uuid + ".csv");
    if (readFile.is_open())
    {
        std::vector<std::string> lines;
        std::string line;
        while (std::getline(readFile, line))
        {
            lines.push_back(line);
        }
        
        // set the start index at 0 if there are not enough pieces
        unsigned int startIndex = lines.size() - 1 - pieces;
        if (startIndex < 0) startIndex = 0;
        for (unsigned int i = startIndex; i < lines.size(); ++i)
        {
            std::vector<std::string> tokens = CSVReader::tokenise(lines[i], ',');
            line = "You " + tokens[0] + " a total of " + tokens[2] + " " + tokens[1];
            std::cout << line << std::endl;
        }

        readFile.close();
    }
}

void Wallet::statisticsUserActivity()
{
    std::ifstream readFile(uuid + ".csv");
    if (readFile.is_open())
    {
        // statistic the history
        std::map<std::string, double> statisticMap;
        std::string line;
        while (std::getline(readFile, line))
        {
            std::vector<std::string> tokens = CSVReader::tokenise(line, ',');

            if (tokens[0] == "bid-sale" || tokens[0] == "ask-sale") continue;

            std::string operate = tokens[0];
            std::string product = tokens[1];
            double amount = std::stod(tokens[2]);
            double overallAmount = statisticMap[operate];
            double productAmount = statisticMap[product];
            statisticMap[operate] = overallAmount + amount;
            statisticMap[product] = productAmount + amount;
        }

        // print out the statistic
        for (std::pair<std::string, double> pair: statisticMap)
        {
            std::cout << pair.first << ": " << std::to_string(pair.second) << std::endl;
        }
        
        readFile.close();
    }
}

std::vector<std::string> Wallet::analyzeAndSimulateUserTrade(unsigned int simulateTimes, std::vector<std::string> tradingHistory)
{
    // possiable improve: do the second part during converting
    // convert the trading history into a table
    std::vector<std::string> operates;
    std::map<std::string, CurrencyHistory> table;
    for (std::string& line: tradingHistory)
    {
        std::vector<std::string> tokens = CSVReader::tokenise(line, ',');
        std::string operate = tokens[0];
        std::string currency = tokens[1];
        double amount = std::stod(tokens[2]);
        double total = table[currency].total;
        if (operate == "bid" || operate == "ask-sale")
        {
            table[currency].total = total - amount;
            table[currency].historyAmounts.push_back(-amount);
        }
        else if (operate == "ask" || operate == "bid-sale")
        {
            table[currency].total = total + amount;
            table[currency].historyAmounts.push_back(amount);
        }
    }

    // separte currencies to two part
    // positive is want to get
    // negative is want to sale
    std::vector<std::string> positve;
    std::vector<std::string> negative;
    double total = 0;
    for (std::pair<std::string, CurrencyHistory> pair: table)
    {
        total += pair.second.total;
        if (pair.second.total > 0)
        {
            positve.push_back(pair.first);
        }
        else
        {
            negative.push_back(pair.first);
        }
    }
    int size = table.size();
    if (size == 0) size = 1;
    double mean = total / size;

    // process if there are not negative then separte by the mean of all currencies
    if (negative.empty())
    {
        for (std::string currency: positve)
        {
            if (table[currency].total < mean)
            {
                negative.push_back(currency);
                positve.erase(std::remove(positve.begin(), positve.end(), currency), positve.end());
            }
        }
    }

    if (positve.empty()) return operates;

    // simulate trades
    std::random_device rd;
    std::mt19937 gen(rd());
    std::vector<double> askAmount;
    std::vector<double> bidAmount;
    for (std::string& currency: positve)
    {
       askAmount.push_back(table[currency].total);
    }
    for (std::string& currency: negative)
    {
       bidAmount.push_back(table[currency].total);
    }
    std::discrete_distribution<> randomSelectAsk {askAmount.begin(), askAmount.end()};
    for (unsigned int i = 0; i < simulateTimes; ++i)
    {
        operates.push_back(simulateUserTrade("ask", table, positve, askAmount, gen));
        operates.push_back(simulateUserTrade("bid", table, negative, bidAmount, gen));
    }

    return operates;
}

std::string Wallet::simulateUserTrade(
    std::string operate,
    std::map<std::string, CurrencyHistory>& currenciesTable,
    std::vector<std::string>& currencies,
    std::vector<double>& amounts,
    std::mt19937& gen
)
{
    std::string s = "";
    std::discrete_distribution<> randomSelectAsk {amounts.begin(), amounts.end()};
    unsigned int index = randomSelectAsk(gen);
    std::string currency = currencies[index];

    // find the minimum and maximum value
    std::pair
    <
        std::vector<double>::iterator,
        std::vector<double>::iterator
    > minmaxIterator = std::minmax_element(currenciesTable[currency].historyAmounts.begin(), currenciesTable[currency].historyAmounts.end());
    double minAmount = *minmaxIterator.first;
    double maxAmount = *minmaxIterator.second;
    std::uniform_real_distribution<> randomDouble(minAmount, maxAmount);
    double amount = randomDouble(gen);

    // if generate a negative amount then skip this time
    if (amount <= 0)
    {
        if (operate == "ask")
        {
            return s;
        }
        else
        {
            amount *= -1;
        }
    }

    std::string amountString = std::to_string(amount);
    s = operate + "," + currency + "," + amountString;

    return s;
}

std::string Wallet::toString()
{
    std::string s;
    for (std::pair<std::string,double> pair : currencies)
    {
        std::string currency = pair.first;
        double amount = pair.second;
        s += currency + " : " + std::to_string(amount) + "\n";
    }
    return s;
}
std::ostream& operator<<(std::ostream& os,  Wallet& wallet)
{
    os << wallet.toString();
    return os;
}

std::string Wallet::storeInString()
{
    std::string s;
    for (std::pair<std::string,double> pair: currencies)
    {
        std::string currency = pair.first;
        double amount = pair.second;
        s += currency + ":" + std::to_string(amount) + "|";
    }

    s[s.length() - 1] = '\n';
    return s;
}
