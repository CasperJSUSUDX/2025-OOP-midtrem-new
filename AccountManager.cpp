#include "AccountManager.h"
#include "CSVReader.h"
#include <iostream>
#include <fstream>
#include <random>
#include <vector>

AccountManager::AccountManager() {}

Wallet AccountManager::login()
{
    
    loadAccounts("accounts.csv");
    std::string uuid;
    std::string password;
    std::string mode;
    std::cout << "Please choose Login or Create a new account. (Type login or create)" << std::endl;
    std::cin >> mode;
    while (true)
    {
        if (mode == "login")
        {
            std::cout << "UUID: " << std::flush;
            std::cin >> uuid;
            if (findUsername(uuid))
            {
                int attempts = 0;
                while (true)
                {
                    std::cout << "Password: " << std::flush;
                    std::cin >> password;
                    if (hasher(password) == cache[uuid].password_h)
                    {
                        std::cout << "Login in successfully" << std::endl;
                        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                        return getWallet(uuid);
                    }
                    else
                    {
                        if (attempts > 1)
                        {
                            std::string answer;
                            std::cout << "Forget password?. (Type retry or reset)" << std::endl;
                            std::cin >> answer;

                            if (answer == "retry")
                            {
                                continue;
                            }
                            else
                            {
                                resetPassword(uuid);
                                break;
                            }
                        }
                        else
                        {
                            std::cout << "Wrong password. Please try again." << std::endl;
                        }
                        attempts++;
                        continue;
                    }
                }
            }
            else
            {
                std::string answer;
                std::cout << "This UUID is not exist.\nDo u want to create a account or try again. (Input create or retry)" << std::endl;
                while (true)
                {
                    std::cin >> answer;
                    if (answer != "retry" && answer != "create")
                    {
                        std::cout << "Unknown answer, please try again." << std::endl;
                    }
                    else
                    {
                        break;
                    }
                }
                
                if (answer == "create")
                {
                    createAccount();
                }
                else
                {
                    continue;
                }
            }

        }
        else if (mode == "create")
        {
            if (createAccount())
            {
                mode = "login";
            }
        }
        else
        {
            std::cout << "Unknow input. Please Try again. (Type login or create)" << std::endl;
            std::cin >> mode;
        }
    }
}

bool AccountManager::createAccount()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::string username;
    std::string password;
    std::string email;
    std::cout << "Please type your full name." << std::endl;
    while (true)
    {
        bool duplicates = false;
        std::cout << "Full name: " << std::flush;
        std::getline(std::cin, username);

        for (auto& data: cache)
        {
            if (data.second.username == username)
            {
                std::cout << "This name is already have an account. Please use other name." <<std::endl;
                duplicates = true;
            }
        }

        if (!duplicates)
        {
            break;
        }
    }

    std::cout << "Please set a password." << std::endl;
    std::cout << "Password: " << std::flush;
    std::cin >> password;

    std::cout << "Please connect an email." << std::endl;
    while (true)
    {
        bool duplicates = false;
        std::cout << "Email: " << std::flush;
        std::cin >> email;

        for (auto& data: cache)
        {
            if (data.second.username == username)
            {
                std::cout << "This email is already been use. Please use other email." <<std::endl;
                duplicates = true;
            }
        }

        if (!duplicates)
        {
            break;
        }
    }

    UserInfo info = {username, hasher(password), email};
    std::string uuid = generateUUID(10);
    existUUID.insert(uuid);
    cache[uuid] = info;
    updateUserCSV();

    Wallet wallet{uuid, ""};
    wallet.insertCurrency("BTC", 10);
    wallet.insertCurrency("USDT", 100);
    wallet.updateUserWalletCSV();
    std::cout << "Create successfully.\nYour UUID is " << uuid << std::endl;
    return true;
}

void AccountManager::loadAccounts(std::string filename)
{
    cache.clear();
    std::fstream accounts(filename);
    if (accounts.is_open())
    {
        std::string line;
        while (std::getline(accounts, line))
        {
            std::vector<std::string> tokens = CSVReader::tokenise(line, ',');
            UserInfo info = { tokens[1], std::stoull(tokens[2]), tokens[3] };
            cache[tokens[0]] = info;
            existUUID.insert(tokens[0]);
        }

        accounts.close();
    }
}

void AccountManager::updateUserCSV()
{
    std::ofstream accounts("accounts.csv", std::ios::trunc);
    if (accounts.is_open())
    {       
        for (std::pair<const std::string, UserInfo>& pair: cache)
        {
            std::string uuid = pair.first;
            std::string username = pair.second.username;
            std::string password = std::to_string(pair.second.password_h);
            std::string email = pair.second.email;
            std::string newLine = uuid + ',' + username + ',' + password + ',' + email + '\n';
            accounts << newLine;
        }

        accounts.close();
    }
}

Wallet AccountManager::getWallet(std::string uuid)
{
    std::ifstream table("walletTable.csv");
    std::string walletSetting;
    if (table.is_open())
    {
        std::string line;
        while (std::getline(table, line))
        {
            std::vector<std::string> tokens = CSVReader::tokenise(line, ',');
            if (tokens[0] == uuid)
            {
                walletSetting = tokens[1];
                break;
            }
        }
        
        table.close();
    }
    Wallet wallet{uuid, walletSetting};
    return wallet;
}

bool AccountManager::findUsername(std::string username)
{
    return cache.find(username) != cache.end();
}

bool AccountManager::resetPassword(std::string uuid)
{
    std::string OTP;
    while (true)
    {
        // Generate OTP
        OTP = "1234";
        // send OTP to email
        while (true)
        {
            std::string input;
            std::cout << "OTP: " << std::flush;
            std::cin >> input;

            if (input == OTP)
            {
                std::string newPassword;
                std::cout << "New Password: " << std::flush;
                std::cin >> newPassword;
                
                cache[uuid].password_h = hasher(newPassword);
                updateUserCSV();
                return true;
            }
            else
            {
                std::string answer;
                std::cout << "Wrong OTP. You want to try again or resent a new one. (Type retry or send)" << std::endl;
                std::cin >> answer;

                if (answer == "send")
                {
                    break;
                }
            }
        }
    }

    return false;
}

std::string AccountManager::generateUUID(int length)
{
    const std::string characters = "0123456789";
    std::string uuid;
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_int_distribution<> distribution(0, characters.size() - 1);

    while (true)
    {
        uuid = "";
        for (int i = 0; i < length; i++)
        {
            int index = distribution(generator);
            uuid += characters[index];
        }

        if (existUUID.find(uuid) != existUUID.end())
        {
            continue;
        }
        else
        {
            return uuid;
        }
    }
}

std::map<std::string, UserInfo> AccountManager::cache;
std::set<std::string> AccountManager::existUUID;
std::hash<std::string> AccountManager::hasher;