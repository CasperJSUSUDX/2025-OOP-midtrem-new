#pragma once

#include <string>
#include <map>
#include <set>

struct UserInfo
{
    std::string username;
    std::size_t password_h;
    std::string email;
};

class AccountManager
{
    public:
        AccountManager();
        static bool login();
        static void createAccount();
    private:
        static void loadAccounts(std::string filename);
        static void updateUserCSV();
        static bool findUsername(std::string username);
        static std::string generateUUID(int length);
        static std::map<std::string, UserInfo> cache;
        static std::set<std::string> existUUID;
};
