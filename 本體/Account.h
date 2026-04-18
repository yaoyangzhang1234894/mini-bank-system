#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <iostream>
#include <mutex>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <memory>
using namespace std;

class Account {
private:
    int accountId;
    string accountHolderName;
    double balance;
    unique_ptr<mutex> accountMutex;
    string currency;
    string userName;
    string passWord;
    bool isActive;
    
    void logTransaction(const string& type, double amount, double newBalance);

public:
    Account();
    Account(int id, const string& name, double initialBalance, const string curr, string username, string password);
    
    // Move constructor
    Account(Account&& other) noexcept;
    
    // Copy constructor - deleted because of mutex
    Account(const Account&) = delete;
    Account& operator=(const Account&) = delete;
    Account& operator=(Account&& other) noexcept;
    
    bool deposit(double amount);
    bool withdraw(double amount);
    double getBalance() const;
    int getId() const { return accountId; }
    string getHolderName() const { return accountHolderName; }
    bool isAccountActive() const { return isActive; }
    void deactivateAccount() { isActive = false; }
    
    string getuserName() const { return userName; }
    string getpassword() const { return passWord; }
    
    void displayAccountInfo() const;
    string getAccountSummary() const;
};

#endif