#include "MerkelMain.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "AccountManager.h"
#include "Candlestick.h"
#include <ctime>
#include <iostream>
#include <random>
#include <vector>

MerkelMain::MerkelMain(Wallet _wallet)
: wallet(_wallet)
{

}

void MerkelMain::init()
{
    int input;
    currentTime = orderBook.getEarliestTime();
    candlestickStartTimestamp = currentTime.substr(0, 19);
    getCurrentSystemTimestamp();

    while(true)
    {
        printMenu();
        input = getUserOption();

        cleanConsole();
        processUserOption(input);

        if (exitFlag) break;

        std::cout << "" << std::endl;
    }
}

void MerkelMain::printMenu()
{
    // print menu based on the menu vector
    unsigned int index = 1;
    for (std::pair<std::string, voidFunc>& pair: menus[indexOfMenus])
    {
        std::cout << std::to_string(index) << ": " << pair.first << std::endl;
        ++index;
    }

    std::cout << "============== " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

// Main menu
void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}
void MerkelMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;
    }
}
void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::ask 
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }   
    }
}
void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::bid 
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }   
    }
}
void MerkelMain::simulateTrade()
{
    std::cout << "Simulating trade..." << std::endl;
    std::string systemTimestamp = getCurrentSystemTimestamp();
    std::vector<std::string> products = orderBook.getKnownProducts();
    std::vector<OrderBookEntry> simulateOBES;

    // Add randomness
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> fluctuation(-0.05, 0.05);

    struct amountRange
    {
        double min = 0.001;
        double max = 0.001;
    };
    

    for (std::string& p: products)
    {
        std::vector<OrderBookEntry> askHistory = orderBook.getOrders(OrderBookType::ask, p, currentTime);
        std::vector<OrderBookEntry> bidHistory = orderBook.getOrders(OrderBookType::bid, p, currentTime);
        amountRange askRange;
        amountRange bidRange;

        // get an avarage price as the base price and the range of amount
        double askPrice = 10;
        if (!askHistory.empty())
        {
            double askSum = 0;
            askRange.max = askHistory[0].amount;
            askRange.min = askHistory[0].amount;

            for (OrderBookEntry& obe: askHistory)
            {
                if (obe.amount > askRange.max) askRange.max = obe.amount;
                if (obe.amount < askRange.min) askRange.min = obe.amount;

                askSum += obe.price;
            }
            askPrice = askSum / askHistory.size();
        }
        double bidPrice = 10;
        if (!bidHistory.empty())
        {
            double bidSum = 0;
            bidRange.max = bidHistory[0].amount;
            bidRange.min = bidHistory[0].amount;

            for (OrderBookEntry& obe: bidHistory)
            {
                if (obe.amount > bidRange.max) bidRange.max = obe.amount;
                if (obe.amount < bidRange.min) bidRange.min = obe.amount;

                bidSum += obe.price;
            }
            bidPrice = bidSum / bidHistory.size();
        }

        // simulate n number of ask
        for (unsigned int i = 0; i < simulateTimes; ++i)
        {
            double price = askPrice * (1 + fluctuation(gen));

            std::uniform_real_distribution<> randomAmount(askRange.min, askRange.max);
            double amount = randomAmount(gen);

            OrderBookEntry obe {
                price,
                amount,
                systemTimestamp,
                p,
                OrderBookType::ask,
                "simuser"
            };

            simulateOBES.push_back(obe);
            orderBook.appendOrder(obe);
        }

        // simulate n number of bid
        for (unsigned int i = 0; i < simulateTimes; ++i)
        {
            double price = bidPrice * (1 + fluctuation(gen));

            std::uniform_real_distribution<> randomAmount(bidRange.min, bidRange.max);
            double amount = randomAmount(gen);

            OrderBookEntry obe {
                price,
                amount,
                systemTimestamp,
                p,
                OrderBookType::bid,
                "simuser"
            };

            simulateOBES.push_back(obe);
            orderBook.appendOrder(obe);
        }
    }
    orderBook.sortOrder();
    std::cout << "Simulate successfully." << std::endl;
    if (debug)
    {
        for (OrderBookEntry& obe: simulateOBES)
        {
            std::cout << "Timestamp: " << obe.timestamp << std::endl;
            if (obe.orderType == OrderBookType::bid) std::cout << "Type: bid" << std::endl;
            if (obe.orderType == OrderBookType::ask) std::cout << "Type: ask" << std::endl;
            std::cout << "Product" << obe.product << std::endl;
            std::cout << "Amount: " << obe.amount << std::endl;
            std::cout << "Price: " << obe.price << std::endl;
        }
    }
        
}
void MerkelMain::jumpToWallet()
{
    std::cout << "Switch to wallet menu." << std::endl;
    indexOfMenus = 1;
}
void MerkelMain::jumpToCandlestick()
{
    std::cout << "Switch to candle stick page.\n" << std::endl;
    printCandlestick();
    indexOfMenus = 2;
}
void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl; 
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
        
    }

    currentTime = orderBook.getNextTime(currentTime);
}
void MerkelMain::exitApp()
{
    std::cout << "See you next time." << std::endl;
    wallet.logInCSV();
    wallet.updateUserWalletCSV();
    exitFlag = true;
}

// Wallet menu
void MerkelMain::dopsiteToWallet()
{
    std::string currency;
    std::string amountString;
    double amount;
    std::cout << "Deposite currency" << std::endl;
    
    while (true)
    {
        std::cout << "Currency: " << std::flush;
        std::cin >> currency;
        std::cout << "Amount: " << std::flush;
        std::cin >> amountString;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try
        {
            bool find = false;
            std::vector<std::string> existProducts = orderBook.getKnownProducts();
            for (std::string p: existProducts)
            {
                std::vector<std::string> tokens = CSVReader::tokenise(p, '/');
                if (currency == tokens[0] || currency == tokens[1]) 
                {
                    find = true;
                    break;
                }
            }
            amount = std::stod(amountString);
            if (find) break;
        }
        catch(std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
        std::cout << "Bad input. Please try again." << std::endl;
    }

    wallet.insertCurrency(currency, amount);
    std::cout << "Deposite successfuly" << std::endl;
}
void MerkelMain::withdrawFromWallet()
{
    std::string currency;
    std::string amountString;
    double amount;
    std::cout << "Withdraw currency" << std::endl;
    while (true)
    {
        std::cout << "Currency: " << std::flush;
        std::cin >> currency;
        std::cout << "Amount: " << std::flush;
        std::cin >> amountString;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        try
        {
            bool find = false;
            std::vector<std::string> existProducts = orderBook.getKnownProducts();
            for (std::string p: existProducts)
            {
                std::vector<std::string> tokens = CSVReader::tokenise(p, '/');
                if (currency == tokens[0] || currency == tokens[1]) 
                {
                    find = true;
                    break;
                }
            }
            amount = std::stod(amountString);
            if (find) break;
        }
        catch(std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
        std::cout << "Bad input. Please try again." << std::endl;
    }

    wallet.removeCurrency(currency, amount);
    std::cout << "Withdraw successfuly" << std::endl;
}
void MerkelMain::printCurrencies()
{
    std::cout << wallet.toString();
}
void MerkelMain::printStatistic()
{
    wallet.statisticsUserActivity();
}
void MerkelMain::printRecentHistory()
{
    std::string input;
    unsigned int num;
    std::cout << "Please input how many to display." << std::endl;
    while (true)
    {
        std::cin >> input;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

        // input check
        try
        {
            int n = std::stoi(input);
            if (n > 0) 
            {
                num = n;
                break;
            }
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        std::cout << "Wrong input. Please input a number again." << std::endl;
    }

    std::cout << "" << std::endl;
    wallet.showTansitionOrTradingHistory(num);
}
void MerkelMain::updateUserCSV()
{
    wallet.updateUserWalletCSV();
    wallet.logInCSV();
    std::cout << "Successfully update." << std::endl;
}
void MerkelMain::exitWalletPage()
{
    std::cout << "Back to main menu." << std::endl;
    indexOfMenus = 0;
}

// Candle stick menu
void MerkelMain::printCandlestick()
{
    std::vector<candlestickEntry> candlesticks = orderBook.generateCandlesticks(
        candlestickStartTimestamp,
        currentTime.substr(0, 19),
        candlestickInterval,
        candlestickProduct,
        candlestickType
    );

    std::string typeString;
    switch (candlestickType)
    {
        case OrderBookType::ask:
            typeString = "Ask";
            break;
        case OrderBookType::bid:
            typeString = "Bid";
            break;
        default:
            typeString = "Unknow";
            break;
    }
    std::cout << "Product: " << candlestickProduct + ", Order type: " << typeString << std::endl;
    std::cout << "==============" << std::endl;

    // generate a candlestick vector by orderbook
    Candlestick::printCandlestick(candlesticks);
}
void MerkelMain::switchCandlestickProduct()
{
    std::string product;
    std::cout << "Enter product. (ETH/BTC)" << std::endl;
    while (true)
    {
        std::cout << "Product: ";
        std::cin >> product;

        std::vector<std::string> existProducts = orderBook.getKnownProducts();
        for (std::string p: existProducts)
        {
            if (p == product) 
            {
                candlestickProduct = product;
                std::cout << "Change successfully. Current product is: " << product << "\n" << std::endl;
                printCandlestick();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                return;
            }
        }

        std::cout << "Bad input. Please try again." << std::endl;
    }
}
void MerkelMain::switchCandlestickType()
{
    std::string type;
    OrderBookType obType;
    std::cout << "Enter a order type to set type of the candle stick. (ask or bid)" << std::endl;
    while (true)
    {
        std::cin >> type;
        

        if (type == "ask")
        {
            obType = OrderBookType::ask;
            break;
        }
        if (type == "bid")
        {
            obType = OrderBookType::bid;
            break;
        }

        std::cout << "Bad input. Please try again. (Format: YYYY/MM/DD HH:MM:SS)" << std::endl;
    }

    candlestickType = obType;
    std::cout << "Change successfuly. Current type is: " << type << "\n" << std::endl;
    printCandlestick();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
void MerkelMain::switchCandlestickStartTimestamp()
{
    std::string timestamp;
    std::cout << "Enter a timestamp to set it as the start timestamp of the candle stick. (Format: YYYY/MM/DD HH:MM:SS)" << std::endl;
    while (true)
    {
        std::cin >> timestamp;
        
        try
        {
            timestamp = timestamp.substr(0, 19);
            std::string day = timestamp.substr(0, 10);
            std::string time = timestamp.substr(11, 8);

            std::vector<std::string> tokens = CSVReader::tokenise(day, '/');
            if (tokens[0].length() != 4) throw;
            if (tokens[1].length() != 2) throw;
            if (tokens[2].length() != 2) throw;
            tokens.clear();
            
            tokens = CSVReader::tokenise(time, ':');
            if (tokens.size() != 3) throw;
            if (tokens[0].length() != 2 || tokens[1].length() != 2 || tokens[2].length() != 2) throw;

            break;
        }
        catch(const std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }

        std::cout << "Bad input. Please try again. (Format: YYYY/MM/DD HH:MM:SS)" << std::endl;
    }

    candlestickStartTimestamp = timestamp;
    std::cout << "Change successfuly. Current start timestamp is: " << timestamp << "\n" << std::endl;
    printCandlestick();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
void MerkelMain::switchCandlestickInterval()
{
    std::string input;
    int num;
    std::cout << "Enter a new timestamp interval of the candlestick. (Will floor to multiples of five)" << std::endl;
    while (true)
    {
        std::cin >> input;

        try
        {
            num = std::stoi(input);
            break;
        }
        catch(std::exception& e)
        {
            std::cerr << e.what() << '\n';
        }
        
        std::cout << "Please enter a positive integer." << std::endl;
    }

    candlestickInterval = num - (num % 5);
    std::cout << "Change successfuly. Current interval is: " << candlestickInterval << "\n" << std::endl;
    printCandlestick();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}
void MerkelMain::exitDrawingPage()
{
    std::cout << "Back to main menu." << std::endl;
    indexOfMenus = 0;
}

int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-" << menus[indexOfMenus].size() << std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
        // 
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    // bad input
    if (userOption <= 0 || userOption > menus[indexOfMenus].size())
    {
        std::cout << "Invalid choice. Choose 1-" << menus[indexOfMenus].size() << std::endl;
        return;
    }

    // Wrote by AI
    // Prompt: How to store a function into a pair and call it
    voidFunc functionPointer = menus[indexOfMenus][userOption - 1].second;
    (this->*functionPointer)();
}

void MerkelMain::cleanConsole()
{
    // code from https://stackoverflow.com/questions/31201631/execute-cmd-commands-using-c
    std::system("cls");
}

std::string MerkelMain::getCurrentSystemTimestamp()
{
    std::time_t t = std::time(0);
    std::tm* now = std::localtime(&t);
    std::string timestamp = std::to_string(now->tm_year + 1900) + '/' +
                            std::to_string(now->tm_mon + 1) + '/' +
                            std::to_string(now->tm_mday) + ' ';

    std::string hour = std::to_string(now->tm_hour);
    std::string min = std::to_string(now->tm_min);
    std::string sec = std::to_string(now->tm_sec);
    if (hour.length() < 2) hour = "0" + hour;
    if (min.length() < 2) min = "0" + min;
    if (sec.length() < 2) sec = "0" + sec;

    timestamp += hour + ':' + min + ':' + sec;

    return timestamp;
}