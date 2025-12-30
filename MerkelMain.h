#pragma once

#include "OrderBookEntry.h"
#include "OrderBook.h"
#include "Wallet.h"
#include <vector>

class MerkelMain
{
    public:
        MerkelMain(Wallet _wallet);
        /** Call this to start the sim */
        void init();
    private: 
        void printMenu();
        void cleanConsole();
        // main menu functions
        void printHelp();
        void printMarketStats();
        void enterAsk();
        void enterBid();
        // Made by myself - start
        void simulateTrade();
        void jumpToWallet();
        void jumpToCandlestick();
        // Made by myself - end
        void gotoNextTimeframe();
        void exitApp();
        // Made by myself - start
        // wallet menu funcitons
        void dopsiteToWallet();
        void withdrawFromWallet();
        void printCurrencies();
        void printStatistic();
        void printRecentHistory();
        void updateUserCSV();
        void exitWalletPage();
        // drawing menu functions
        std::string candlestickProduct = "ETH/BTC";
        std::string candlestickStartTimestamp;
        OrderBookType candlestickType = OrderBookType::ask;
        unsigned int candlestickInterval = 5;
        void printCandlestick();
        void switchCandlestickProduct();
        void switchCandlestickType();
        void switchCandlestickStartTimestamp();
        void switchCandlestickInterval();
        void exitDrawingPage();
        // Made by myself - end
        // process input functions
        int getUserOption();
        void processUserOption(int userOption);
        
        std::string getCurrentSystemTimestamp();

        std::string currentTime;
        bool exitFlag = false;
        unsigned int simulateTimes = 5;
        bool debug = true;

        // menu variables
        // Made by myself - start
        unsigned int indexOfMenus = 0;
        // Made by myself - end
        using voidFunc = void (MerkelMain::*)();
        // Made by myself - start
        std::vector<std::vector<std::pair<std::string, voidFunc>>> menus = {
            // main menu
            {
                {"Print help", printHelp},
                {"Print exchange stats", printMarketStats},
                {"Make an offer", enterAsk},
                {"Make a bid", enterBid},
                {"Simulate trades", simulateTrade},
                {"Check wallet", jumpToWallet},
                {"Show candle stick", jumpToCandlestick},
                {"Continue", gotoNextTimeframe},
                {"Exit", exitApp}
            },
            // wallet menu
            {
                {"Deposit", dopsiteToWallet},
                {"Withdraw", withdrawFromWallet},
                {"Print currencies", printCurrencies},
                {"Print statistic", printStatistic},
                {"Print recent activity", printRecentHistory},
                {"Update user history", updateUserCSV},
                {"Exit", exitWalletPage}
            },
            // drawing menu
            {
                {"Switch candle stick product", switchCandlestickProduct},
                {"Switch candle stick type", switchCandlestickType},
                {"Switch candle stick start timestamp", switchCandlestickStartTimestamp},
                {"Switch candle stick interval", switchCandlestickInterval},
                {"Exit", exitDrawingPage}
            }
        };
        // Made by myself - end

        // OrderBook orderBook{"20200317.csv"};
	    OrderBook orderBook{"20200601.csv"};
        Wallet wallet;

};
