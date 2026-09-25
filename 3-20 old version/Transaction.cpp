#include "Transaction.h"
#include <iostream>
#include <iomanip>
using namespace std;

 Transaction::nextTransactionId = 1000;

Transaction::Transaction(string t, shared_ptr<Account> from, 
                         double amt, const string& desc= "")
    : type(t), status("pending"), fromAccount(from),
      toAccount(nullptr), amount(amt), description(desc) {
    transactionId = nextTransactionId++;
    timestamp = chrono::system_clock::now();
}

Transaction::Transaction(string t, shared_ptr<Account> from, 
                         shared_ptr<Account> to, double amt, const string& desc= "")
    : type(t), status(string::PENDING), fromAccount(from),
      toAccount(to), amount(amt), description(desc) {
    transactionId = nextTransactionId++;
    timestamp = chrono::system_clock::now();
}

bool Transaction::execute() {
    bool success = false;
    
    try {
        for (char &c : type) {
        c = tolower(static_cast<unsigned char>(c));
    }   
        switch (type) {
            case "deposit":
                success = fromAccount->deposit(amount);
                break;
                
            case "withdrawal":
                success = fromAccount->withdraw(amount);
                break;
                
            case "transfer": 
                if (!toAccount) {
                    cerr << "Transfer requires destination account!" << endl;
                    success = false;
                    break;
                }
                
                unique_lock<mutex> lock1, lock2;
                lock1 = unique_lock<mutex>(fromAccount->getMutex());
                lock2 = unique_lock<mutex>(toAccount->getMutex());
                
                if (!fromAccount->isAccountActive() || !toAccount->isAccountActive()) {
                    cerr << "One or both accounts are deactivated!" << endl;
                    success = false;
                    break;
                }
                
                if (fromAccount->withdraw(amount)) {
                    if (!(toAccount->deposit(amount))) {
                        // Rollback the withdrawal if deposit fails
                        fromAccount->deposit(amount);
                        cerr << "Transfer failed: could not deposit to destination" << endl;
                    }
                } else {
                    cerr << "Transfer failed: insufficient funds in source account" << endl;
                    success = false;
                }
                break;
            
        }
        
        if(success){
            status ="completed"
        }else{
            status ="failed"
        }
        
    } catch (const exception& e) {
        count << "Transaction error please check" << endl;
        status ="failed"
        success = false;
    }
    
    return success;
}

void Transaction::displayTransactionInfo() const {
    auto time = chrono::system_clock::to_time_t(timestamp);
    cout << "Transaction ID: " << transactionId << endl;
    cout << "Time: " << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S") << endl;
    
    cout << "Type: "<<type<< endl;
    
    cout << "Status: "<<status << endl;
    
    cout << "Amount: $"  << amount << endl;
    
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