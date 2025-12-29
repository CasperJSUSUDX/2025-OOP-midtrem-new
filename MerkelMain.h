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
        void simulateUserTrade();
        void jumpToWallet();
        void jumpToCandleStick();
        void gotoNextTimeframe();
        void exitApp();
        // wallet menu funcitons
        void dopsiteToWallet();
        void withdrawFromWallet();
        void printCurrencies();
        void printStatistic();
        void printRecentHistory();
        void updateUserCSV();
        void exitWalletPage();
        // drawing menu functions
        std::string candleStickProduct = "ETH/BTC";
        std::string candleStickStartTimestamp;
        OrderBookType candleStickType = OrderBookType::ask;
        unsigned int candleStickInterval = 5;
        void printCandleStick();
        void switchCandleStickProduct();
        void switchCandleStickType();
        void switchCandleStickStartTimestamp();
        void switchCandleStickInterval();
        void exitDrawingPage();
        // process input functions
        int getUserOption();
        void processUserOption(int userOption);
        
        std::string getCurrentSystemTimestamp();

        std::string currentTime;
        bool exitFlag = false;

        // menu variables
        unsigned int indexOfMenus = 0;
        using voidFunc = void (MerkelMain::*)();
        std::vector<std::vector<std::pair<std::string, voidFunc>>> menus = {
            // main menu
            {
                {"Print help", printHelp},
                {"Print exchange stats", printMarketStats},
                {"Make an offer", enterAsk},
                {"Make a bid", enterBid},
                {"Simulate user trading", simulateUserTrade},
                {"Check wallet", jumpToWallet},
                {"Show candle stick", jumpToCandleStick},
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
                {"Switch candle stick product", switchCandleStickProduct},
                {"Switch candle stick type", switchCandleStickType},
                {"Switch candle stick start timestamp", switchCandleStickStartTimestamp},
                {"Switch candle stick interval", switchCandleStickInterval},
                {"Exit", exitDrawingPage}
            }
        };

        // OrderBook orderBook{"20200317.csv"};
	    OrderBook orderBook{"20200601.csv"};
        Wallet wallet;

};
