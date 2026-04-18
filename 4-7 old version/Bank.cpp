#include "mydbms.h"
#include <random>
#include <algorithm>
#include <numeric>
#include <sqlite3.h>
using namespace std;


Bank::Bank(const string& name) : bankName(name) {
    

}

bool Bank::createAccount(const string& holderName, 
                                            double initialBalance, string tableName
                                            const string& currency, string userName, string passWord) {
    boost::interprocess::named_mutex mutex(
        boost::interprocess::open_or_create, 
        tableName+userName+passWord
    );
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);
    
    db.insertData(holderName, tableName, initialBalance, currency,  userName,  passWord);
    Account* a=db.queryData(tableName,  userName,  passWord);
    if(a->getId()==0){
        return 0;
    }
    return 1;
}

bool Bank::closeAccount(string tableName, string userName, string passWord) {
     boost::interprocess::named_mutex mutex(
        boost::interprocess::open_or_create, 
        tableName+userName+passWord
    );
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);

    bool it = db.deleteData(tableName, userName, passWord);
    if(it==true){
        cout << "Account " << accountId << " closed successfully!" << endl;
        return true;
    }
    cout<<"error"
    return 0;
    
}

Account Bank::getAccount(string tableName, string userName, string passWord) {
     boost::interprocess::named_mutex mutex(
        boost::interprocess::open_or_create, 
        tableName+userName+passWord
    );
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);

    Account result=queryData(tableName, userName, passWord);
    if(a->getId()==0){
        return result;
    }
    return result;
}


vector<shared_ptr<Account>> Bank::getAllAccounts(string tableName) const {
    boost::interprocess::named_mutex mutex(
        boost::interprocess::open_or_create, 
        tableName+userName+passWord
    );
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);

    vector<Account> result = queryAllData(tableName);
    vector<shared_ptr<Account>> result;
    return result;
}

bool Bank::deposit(string tableName, string userName, string passWord ,double amount) {
    Account x = getAccount(string tableName, string userName, string passWord);

    if (!x) return false;
        
        // Update database
        boost::interprocess::named_mutex mutex(
        boost::interprocess::open_or_create, 
        tableName+userName+passWord
        );
        boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);
        int balance= x.getBalance();
        bool suc= updataeData( tableName,  userName, passWord, "balance", balance+amount);


        //print for Receipt 
        auto transaction = make_shared<Transaction>(
        "deposit", account, amount, "Bank deposit"
    );
    
        transactionHistory.push_back(transaction);
        totalTransactions++;

    
    return suc ;
}

bool Bank::withdraw(string tableName, string userName, string passWord ,double amount) {
    Account x = getAccount(string tableName, string userName, string passWord);
    if (!x) return false;
    
        
        // Update database
        boost::interprocess::named_mutex mutex(
        boost::interprocess::open_or_create, 
        tableName+userName+passWord
        );
        boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);
        int balance= x.getBalance();
        if(balance>amount)bool suc= updataeData( tableName,  userName, passWord, "balance", balance-amount);


        //print for Receipt 
        auto transaction = make_shared<Transaction>(
        "withdraw", account, amount, "Bank withdrawal"
    );
    
        transactionHistory.push_back(transaction);
        totalTransactions++;
    
    return (suc && success);
}

bool Bank::transfer(string tableName1,string tableName2, string userName1, string passWord1 ,string userName2, string passWord2, double amount) {
    if (userName1 == userName2 && passWord1==passWord2) {
        cerr << "Cannot transfer to the same account!" << endl;
        return false;
    }
        // Update database
        bool x=this.withdraw( tableName1,  userName1,  passWord1, amount);
        bool y=this.deposit( tableName2,  userName2,  passWord2 , amount);


    //print for Receipt 
        auto transaction = make_shared<Transaction>(
        "transfer", account, amount, "Bank transfer"
    );
    
        transactionHistory.push_back(transaction);
        totalTransactions++;
    
    return (x && y);
}

double Bank::getAccountBalance(string tableName, string userName, string passWord) const {
    Account x=this.getAccount( tableName,  userName,  passWord);
    
    return x.getBalance()
    
}

void Bank::displayAllAccounts(string tableName) const {
    vector<shared_ptr<Account>> db=getAllAccounts(string tableName);
    
    cout << "\n=== All Accounts in " << bankName << " ===" << endl;
    for (const auto& record : db) {
        cout << record->getAccountSummary() << endl;
    }
    cout << "Total Accounts: " << db.size() << endl;
    cout << "=====================================\n" << endl;
}

void Bank::displayTransactionHistory() const {
    cout << "\n=== Transaction History ===" << endl;
    if (transactionHistory.empty()) {
        cout << "No transactions yet." << endl;
    } else {
        for (const auto& transaction : transactionHistory) {
            transaction->displayTransactionInfo();
        }
    }
    cout << "==========================\n" << endl;
}


double Bank::getTotalBankBalance(string tableName) const {
    vector<shared_ptr<Account>> db=getAllAccounts(tableName);
    
    double total = 0.0;
    for (const auto& record : db) {
            total += record->getBalance();
        
    }
    return total;
}


//到这了
void Bank::simulateConcurrentTransactions(string tableName) {
    cout << "\n=== Simulating Concurrent Transactions ===" << endl;
    
    // Create multiple threads to simulate concurrent banking operations
    vector<thread> threads;
    random_device rd;               
    std::mt19937 gen(rd());
    
    vector<shared_ptr<Account>> db = getAllAccounts(tableName);

    uniform_int_distribution<> accountDist(0, db.size());  // First 5 accounts
    uniform_int_distribution<> amountDist(0, db.size());
    uniform_int_distribution<> opDist(0, 2);  // 0: deposit, 1: withdraw, 2: transfer
    
    // Create 10 concurrent transactions
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([this, i, &gen, &accountDist, &amountDist, &opDist]() {
            int from = accountDist(gen);
            int to = accountDist(gen);
            double amount = amountDist(gen);
            int op = opDist(gen);

            auto fromacc=db[from];
            auto toacc=db[to];
            
            cout << "Thread " << i << ": Starting operation..." << endl;
            
            switch (op) {
                case 0:
                    cout << "Thread " << i << ": Depositing $" << amount 
                              << " to account " << fromId << endl;
                    deposit(tableName, fromacc->getuserName(), fromacc->getpassword(),amount);
                    break;
                    
                case 1:
                    cout << "Thread " << i << ": Withdrawing $" << amount 
                              << " from account " << fromId << endl;
                    withdraw(tableName, fromacc->getuserName(), fromacc->getpassword(),amount);
                    break;
                    
                case 2:
                    if (fromId != toId) {
                        cout << "Thread " << i << ": Transferring $" << amount 
                                  << " from account " << fromId << " to " << toId << endl;
                        transfer(tableName,tableName, fromacc->getuserName(), fromacc->getpassword(),toacc->getuserName(), toacc->getpassword(), amount);
                    }
                    break;
            }
            
            cout << "Thread " << i << ": Operation completed." << endl;
        });
    }
    
    // Wait for all threads to complete
    for (auto& thread : threads) {
        thread.join();
    }
    
    cout << "All concurrent transactions completed!" << endl;
    cout << "======================================\n" << endl;
}