#include "AccountManager.h"
#include "CSVReader.h"
#include <iostream>
#include <fstream>
#include <random>
#include <vector>

AccountManager::AccountManager() {}

bool AccountManager::login()
{
    
    loadAccounts("accounts.csv");
    std::hash<std::string> hasher;
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
                std::cout << "Password: " << std::flush;
                std::cin >> password;
                if (hasher(password) == cache[uuid].password_h)
                {
                    std::cout << "Login in successfully" << std::endl;
                }
                else
                {
                    std::cout << "Wrong password. Please try again." << std::endl;
                    continue;
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
                    return false;
                }
                else
                {
                    continue;
                }
            }

            return true;
        }
        else
        {
            createAccount();
            return false;
        }
    }
}

void AccountManager::createAccount()
{
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    std::hash<std::string> hasher;
    std::string username;
    std::string password;
    std::string email;
    std::cout << "Please give a username." << std::endl;
    std::cout << "Username: " << std::flush;
    std::getline(std::cin, username);

    std::cout << "Please set a password." << std::endl;
    std::cout << "Password: " << std::flush;
    std::cin >> password;

    std::cout << "Please connect an email." << std::endl;
    std::cout << "Email: " << std::flush;
    std::cin >> email;
    UserInfo info = {username, hasher(password), email};
    std::string uuid = generateUUID(10);
    existUUID.insert(uuid);
    cache[uuid] = info;
    updateUserCSV();
    std::cout << "Create successfully.\nYour UUID is " << uuid << std::endl;
}

void AccountManager::loadAccounts(std::string filename)
{
    cache.clear();
    std::hash<std::string> hasher;
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
    std::ofstream accounts("accounts.csv", std::ios::out | std::ios::trunc);
    if (accounts.is_open())
    {
        for (auto& data: cache)
        {
            std::string uuid = data.first;
            std::string username = data.second.username;
            std::string password = std::to_string(data.second.password_h);
            std::string email = data.second.email;
            std::string newLine = uuid + ',' + username + ',' + password + ',' + email + '\n';
            accounts << newLine;
        }

        accounts.close();
    }
}

bool AccountManager::findUsername(std::string username)
{
    return cache.find(username) != cache.end();
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