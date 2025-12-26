#pragma once

#include "OrderBookEntry.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <random>

class Wallet 
{
    public:
        Wallet(std::string _uuid, std::string walletString);
        /** insert currency to the wallet */
        void insertCurrency(std::string type, double amount);
        /** remove currency from the wallet */
        bool removeCurrency(std::string type, double amount);
        
        /** check if the wallet contains this much currency or more */
        bool containsCurrency(std::string type, double amount);
        /** checks if the wallet can cope with this ask or bid.*/
        bool canFulfillOrder(OrderBookEntry order);
        /** update the contents of the wallet
         * assumes the order was made by the owner of the wallet
        */
        void processSale(OrderBookEntry& sale);
        /** log user transition and the balance */
        void logInCSV();

        /** generate a string representation of the wallet */
        std::string toString();
        friend std::ostream& operator<<(std::ostream& os, Wallet& wallet);
        /** convert to a format that can store in CSV and read by the code */
        std::string storeInString();

        /** this funciton will read the history csv and display a specific pieces by the arugment */
        void showTansitionOrTradingHistory(unsigned int pieces);
        /** this function will read the history csv and statistic all of the activity */
        void statisticsUserActivity();

        /** pass a trading history to analyze and call simulateUserTrade() to simulate */
        static std::vector<std::string> analyzeAndSimulateUserTrade(unsigned int simulateTimes, std::vector<std::string> tradingHistory);

        void updateUserWalletCSV();
        
        std::string uuid;
    private:
        struct CurrencyHistory
        {
            double total;
            std::vector<double> historyAmounts;
        };
        /** simulate a user tarde activity and return by a string */
        static std::string simulateUserTrade(
            std::string operate,
            std::map<std::string, CurrencyHistory>& currenciesTable,
            std::vector<std::string>& currencies,
            std::vector<double>& amounts,
            std::mt19937& gen
        );

        // there are two format of the operate
        // if only have input or output: {action},{currency},{amount}
        // if have both input and output: {action},{outgoingCurrency},{outgoingAmount},{incomingCurrency},{incomingAmount}
        std::vector<std::string> operatesCache;
        std::map<std::string,double> currencies;

};



	

