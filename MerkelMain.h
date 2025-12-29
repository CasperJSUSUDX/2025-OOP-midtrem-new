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
        void jumpToWallet();
        void jumpToCandleStick();
        void gotoNextTimeframe();
        void exitApp();
        // wallet menu funcitons
        void printCurrencies();
        void printStatistic();
        void printRecentHistory();
        void updateUserCSV();
        void exitWalletPage();
        // drawing menu functions
        std::string candleStickStartTimestamp;
        unsigned int candleStickInterval = 5;
        void printCandleStick();
        void switchCandleStickStartTimestamp();
        void switchCandleStickInterval();
        void exitDrawingPage();
        // process input functions
        int getUserOption();
        void processUserOption(int userOption);

        std::string currentTime;
        bool exitFlag = false;

        unsigned int indexOfMenus = 0;
        using voidFunc = void (MerkelMain::*)();
        std::vector<std::vector<std::pair<std::string, voidFunc>>> menus = {
            // main menu
            {
                {"Print help", printHelp},
                {"Print exchange stats", printMarketStats},
                {"Make an offer", enterAsk},
                {"Make a bid", enterBid},
                {"Check wallet", jumpToWallet},
                {"Show candle stick", jumpToCandleStick},
                {"Continue", gotoNextTimeframe},
                {"Exit", exitApp}
            },
            // wallet menu
            {
                {"Print currencies", printCurrencies},
                {"Print statistic", printStatistic},
                {"Print recent activity", printRecentHistory},
                {"Update user history", updateUserCSV},
                {"Exit", exitWalletPage}
            },
            // drawing menu
            {
                {"Switch candle stick start timestamp", switchCandleStickStartTimestamp},
                {"Switch candle stick interval", switchCandleStickInterval},
                {"Exit", exitDrawingPage}
            }
        };

        // OrderBook orderBook{"20200317.csv"};
	    OrderBook orderBook{"20200601.csv"};
        Wallet wallet;

};
