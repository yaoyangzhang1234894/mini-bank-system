#ifndef BANK_H
#define BANK_H

#include "Account.h"
#include "Transaction.h"
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <thread>
#include <queue>
#include <atomic>  
#include <chrono> 
#include <sqlite3.h>
using namespace std;
class Bank {
private:
    string bankName;
    map<int, shared_ptr<Account>> accounts;
    vector<shared_ptr<Transaction>> transactionHistory;
    mutable mutex bankMutex;
    atomic<int> totalTransactions{0}; 
    sqlite3* db;


public:
    Bank(const string& name);
    
    // Account management
    shared_ptr<Account> createAccount(const string& holderName, 
                                          double initialBalance, 
                                          const string& currency = "CAD");
    bool closeAccount(int accountId);
    shared_ptr<Account> getAccount(int accountId);
    vector<shared_ptr<Account>> getAllAccounts() const;
    
    // Banking operations
    bool deposit(int accountId, double amount);
    bool withdraw(int accountId, double amount);
    bool transfer(int fromAccountId, int toAccountId, double amount);
    
    // Query operations
    double getAccountBalance(int accountId) const;
    void displayAllAccounts() const;
    void displayTransactionHistory() const;
    
    // Database operations (simulated)
    void syncToDatabase();  // Simulate DB sync
    void loadFromDatabase();  // Simulate DB load
    void optimizeDatabaseQueries();  // Simulated query optimization
    
    // Statistics
    int getTotalAccounts() const { return accounts.size(); }
    int getTotalTransactions() const { return totalTransactions.load(); }
    double getTotalBankBalance() const;
    
    // Concurrency demonstration
    void simulateConcurrentTransactions();
};

#endif