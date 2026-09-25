#ifndef BANK_H
#define BANK_H

#include "mydbms.h"
#include "Transaction.h"
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <thread>
#include <queue>
#include <atomic>  
#include <chrono> 
#include <random>
#include <sqlite3.h>
using namespace std;

class Bank {
private:
    string bankName;
    map<int, shared_ptr<Account>> accounts;
    vector<shared_ptr<Transaction>> transactionHistory;
    mutable mutex bankMutex; 
    mydbms db;
    atomic<int> totalTransactions{0};

public:
    Bank(const string& name, const string& tablename);
    
    bool createTable(const string& dbname, const string& tablename);
    
    bool createAccount(const string& holderName, double initialBalance, 
                       const string& tableName, const string& currency, 
                       const string& userName, const string& passWord);
    bool closeAccount(const string& tableName, const string& userName, const string& passWord);
    shared_ptr<Account> getAccount(const string& tableName, const string& userName, const string& passWord);
    vector<shared_ptr<Account>> getAllAccounts(const string& tableName);
    bool depositwithoutlock(const string& tableName, const string& userName, const string& passWord, double amount);
    bool withdrawwithoutlock(const string& tableName, const string& userName, const string& passWord, double amount);
    bool deposit(const string& tableName, const string& userName, const string& passWord, double amount);
    bool withdraw(const string& tableName, const string& userName, const string& passWord, double amount);
    bool transfer(const string& tableName1, const string& tableName2, 
                  const string& userName1, const string& passWord1,
                  const string& userName2, const string& passWord2, double amount);
    
    double getAccountBalance(const string& tableName, const string& userName, const string& passWord);
    void displayAllAccounts(const string& tableName);
    void displayTransactionHistory() const;
    
    int getTotalAccounts(const string& tableName);
    double getTotalBankBalance(const string& tableName);
    int getTotalTransactions() const { return totalTransactions.load(); }
    
    void simulateConcurrentTransactions(const string& tableName);
};

#endif