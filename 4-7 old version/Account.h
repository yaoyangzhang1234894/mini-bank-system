#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <iostream>
#include <mutex>
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
    mutable shared_mutex accountMutex;  // Per-account lock
    string currency;
    string userName;
    string passWord;
    bool isActive;
    
    
    // Transaction logging
    void logTransaction(const string& type, double amount, double newBalance);

u.accountId = stoi(argv[0]);
                u.accountHolderName = argv[1];
                u.balance = stoi(argv[2]);
                u.currency = argv[3];
                u.userName = argv[4];
                u.passWord = argv[5];
                u.isActive = stoi(argv[6]);



public:
    Account(int id, const string& name, double initialBalance, 
            const string curr = "CAD", string username, string password);
    Account(int id, const string& name, string username, string password);
    
    // Core operations
    bool deposit(double amount);
    bool withdraw(double amount);
    double getBalance() const;
    int getId() const { return accountId; }
    string getHolderName() const { return accountHolderName; }
    bool isAccountActive() const { return isActive; }
    void deactivateAccount() { isActive = false; }
    
    // Lock management for transfer operations
    mutex& getMutex() { return accountMutex; }
    string getuserName() const;
    string getpassword() const;
    // Display account info
    void displayAccountInfo() const;
    
    // Get account summary
    string getAccountSummary() const;
};

#endif