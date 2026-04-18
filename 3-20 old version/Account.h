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
    bool isActive;
    
    
    // Transaction logging
    void logTransaction(const string& type, double amount, double newBalance);

public:
    Account(int id, const string& name, double initialBalance, 
            const string curr = "CAD");
    Account(int id, const string& name);
    
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
    
    // Display account info
    void displayAccountInfo() const;
    
    // Get account summary
    string getAccountSummary() const;
};

#endif