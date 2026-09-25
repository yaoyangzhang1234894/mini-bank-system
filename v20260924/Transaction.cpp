#include "Transaction.h"
#include <iostream>
#include <iomanip>
#include <ctime>
using namespace std;

int Transaction::nextTransactionId = 1000;

Transaction::Transaction(string t, shared_ptr<Account> from, double amt, const string& desc)
    : type(t), status("pending"), fromAccount(from),
      toAccount(nullptr), amount(amt), description(desc) {
    transactionId = nextTransactionId++;
    timestamp = chrono::system_clock::now();
}

Transaction::Transaction(string t, shared_ptr<Account> from, 
                         shared_ptr<Account> to, double amt, const string& desc)
    : type(t), status("pending"), fromAccount(from),
      toAccount(to), amount(amt), description(desc) {
    transactionId = nextTransactionId++;
    timestamp = chrono::system_clock::now();
}

bool Transaction::execute() {
    if (type == "deposit" && fromAccount) {
        if (fromAccount->deposit(amount)) {
            status = "completed";
            return true;
        }
    } else if (type == "withdraw" && fromAccount) {
        if (fromAccount->withdraw(amount)) {
            status = "completed";
            return true;
        }
    } else if (type == "transfer" && fromAccount && toAccount) {
        if (fromAccount->withdraw(amount)) {
            if (toAccount->deposit(amount)) {
                status = "completed";
                return true;
            } else {
                // Rollback
                fromAccount->deposit(amount);
                status = "failed";
                return false;
            }
        }
    }
    status = "failed";
    return false;
}

void Transaction::displayTransactionInfo() const {
    auto time = chrono::system_clock::to_time_t(timestamp);
    cout << "Transaction ID: " << transactionId << endl;
    cout << "Time: " << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S") << endl;
    cout << "Type: " << type << endl;
    cout << "Status: " << status << endl;
    cout << "Amount: $" << amount << endl;
    
    if (fromAccount) {
        cout << "From Account: " << fromAccount->getId() 
             << " (" << fromAccount->getHolderName() << ")" << endl;
    }
    
    if (toAccount && type == "transfer") {
        cout << "To Account: " << toAccount->getId() 
             << " (" << toAccount->getHolderName() << ")" << endl;
    }
    
    if (!description.empty()) {
        cout << "Description: " << description << endl;
    }
    cout << "-----------------------------------" << endl;
}