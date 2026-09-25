#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "Account.h"
#include <memory>
#include <chrono>
#include <string>
#include <atomic>
using namespace std;

class Transaction {
private:
    static int nextTransactionId;
    int transactionId;
    string type;
    string status;
    shared_ptr<Account> fromAccount;
    shared_ptr<Account> toAccount;  // nullptr for deposits/withdrawals
    double amount;
    string description;
    chrono::system_clock::time_point timestamp;

public:
    Transaction(string t, shared_ptr<Account> from, 
                double amt, const string& desc);
    Transaction(string t, shared_ptr<Account> from, 
                shared_ptr<Account> to, double amt, const string& desc);
    
    bool execute();
    string getStatus() const { return status; }
    int getId() const { return transactionId; }
    void displayTransactionInfo() const;
};

#endif