#ifndef BANK_H
#define BANK_H

#include "mydbms.h"
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
    mydbms db;

public:
    Bank(const string& name);
    
    // Account management
    bool createAccount(const string& holderName, 
                                            double initialBalance, string tableName
                                            const string& currency, string userName, string passWord);
    bool closeAccount(string tableName, string userName, string passWord);
    Account getAccount(string tableName, string userName, string passWord);
    vector<shared_ptr<Account>> getAllAccounts(string tableName) const;
    
    // Banking operations
    bool deposit(string tableName, string userName, string passWord ,double amount) ;
    bool withdraw(string tableName, string userName, string passWord ,double amount);
    bool transfer(string tableName1,string tableName2, string userName1, string passWord1 ,string userName2, string passWord2, double amount);
    
    // Query operations
    double getAccountBalance(string tableName, string userName, string passWord) const;
    void displayAllAccounts(string tableName) const;
    void displayTransactionHistory() const;
    
    // Statistics
    int getTotalAccounts() const { return accounts.size(); }
    double getTotalBankBalance(string tableName) const;
    
    // Concurrency demonstration
    void simulateConcurrentTransactions(string tableName);
};

#endif