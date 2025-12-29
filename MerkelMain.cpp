#include "MerkelMain.h"
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include "AccountManager.h"
#include "CandleStick.h"
#include <iostream>
#include <vector>

MerkelMain::MerkelMain(Wallet _wallet)
: wallet(_wallet)
{

}

void MerkelMain::init()
{
    int input;
    currentTime = orderBook.getEarliestTime();

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
void MerkelMain::jumpToWallet()
{
    std::cout << "Switch to wallet menu." << std::endl;
    indexOfMenus = 1;
}
void MerkelMain::jumpToCandleStick()
{
    std::cout << "Switch to candle stick page.\n" << std::endl;
    printCandleStick();
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
void MerkelMain::printCandleStick()
{
    // get default range of the candleSticks
    std::vector<candleStickEntry> candleSticks = orderBook.generateCnadleSticks(
        "2020/06/01 11:57:30",
        "2020/06/01 11:57:40",
        candleStickInterval
    );

    std::cout << "==============" << std::endl;

    // generate a candleStick vector by orderbook
    CandleStick::printCandleStick(candleSticks);
    
}
void MerkelMain::switchCandleStickInterval()
{
    std::string input;
    int num;
    std::cout << "Input a new timestamp interval of the candleStick. (Will floor to multiples of five)" << std::endl;
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
        
        std::cout << "Please input a positive integer." << std::endl;
    }

    candleStickInterval = num - (num % 5);
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