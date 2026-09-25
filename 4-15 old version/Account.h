#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <iostream>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>
using namespace std;

class Account {
private:
    int accountId;
    string accountHolderName;
    double balance;
    mutable shared_mutex accountMutex; 
    string currency;
    string userName;
    string passWord;
    bool isActive;
    
    // Transaction logging
    void logTransaction(const string& type, double amount, double newBalance);

public:
    // 构造函数声明修正
    Account(int id, const string& name, double initialBalance, 
            const string& curr, const string& username, const string& password);
    Account(int id, const string& name, const string& username, const string& password);
    
    // Core operations
    bool deposit(double amount);
    bool withdraw(double amount);
    double getBalance() const;
    int getId() const { return accountId; }
    string getHolderName() const { return accountHolderName; }
    bool isAccountActive() const { return isActive; }
    void deactivateAccount() { isActive = false; }
    
    // Lock management for transfer operations
    shared_mutex& getMutex() const { return accountMutex; }  // 返回 shared_mutex&
    string getuserName() const;
    string getpassword() const;
    
    // Display account info
    void displayAccountInfo() const;
    
    // Get account summary
    string getAccountSummary() const;
};

#endif