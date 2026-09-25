#include "mydbms.h"
#include "Bank.h"
#include "Transaction.h"
#include <random>
#include <algorithm>
#include <numeric>
#include <sqlite3.h>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/scoped_lock.hpp>
using namespace std;


Bank::Bank(const string& name, const string& dbname) : bankName(name) {
    db.createDB(dbname);

}
Bank::createTable(const string& dbname, const string& tablename) {
   return db.createTable(tablename);
     

}


bool Bank::createAccount(const string& holderName, 
                                            double initialBalance, string tableName
                                            const string& currency, string userName, string passWord) {
    boost::interprocess::named_mutex mutex(
        boost::interprocess::open_or_create, 
        tableName+userName+passWord
    );
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);
    
    return db.insertData(holderName, tableName, initialBalance, currency,  userName,  passWord);
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

shared_ptr<Account> Bank::getAccount(string tableName, string userName, string passWord) {
     boost::interprocess::named_mutex mutex(
        boost::interprocess::open_or_create, 
        tableName+userName+passWord
    );
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);
    try {
        Account result=db.queryData(tableName, userName, passWord);
        return make_shared<Account>(result);  
    } catch (...) {
        return nullptr;
    }
}


vector<shared_ptr<Account>> Bank::getAllAccounts(string tableName) const {
    boost::interprocess::named_mutex mutex(
        boost::interprocess::open_or_create, 
        tableName+userName+passWord
    );
    boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);

    vector<Account> result = db.queryAllData(tableName);
    vector<shared_ptr<Account>> result;
    return result;
}

bool Bank::deposit(string tableName, string userName, string passWord ,double amount) {
    shared_ptr<Account> x = db.getAccount(string tableName, string userName, string passWord);

    if (!x) return false;
        
        // Update database
        //create a lock based on tablename username and passwrod
        boost::interprocess::named_mutex mutex(
        boost::interprocess::open_or_create, tableName+userName+passWord);
        boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);
        double  balance= x->getBalance();
        bool suc= db.updataeData( tableName,  userName, passWord, "balance", balance+amount);


        //print for Receipt 
        auto transaction = make_shared<Transaction>(
        "deposit", account, amount, "Bank deposit"
    );
    
        transactionHistory.push_back(transaction);
        totalTransactions++;

    
    return suc;
}

bool Bank::withdraw(string tableName, string userName, string passWord ,double amount) {
    shared_ptr<Account> x = db.getAccount(string tableName, string userName, string passWord);
    if (!x) return false;
    
        
        // Update database
        //create a lock based on tablename username and passwrod
        boost::interprocess::named_mutex mutex(
        boost::interprocess::open_or_create, tableName+userName+passWord);
        boost::interprocess::scoped_lock<boost::interprocess::named_mutex> lock(mutex);
        
        int balance= x->getBalance();
        if (balance < amount) {
        cerr << "Insufficient funds!" << endl;
        return false;
        }
        
        bool suc= db.updataeData( tableName,  userName, passWord, "balance", balance-amount);
        //print for Receipt 
        auto transaction = make_shared<Transaction>(
        "withdraw", account, amount, "Bank withdrawal"
    );
    
        transactionHistory.push_back(transaction);
        totalTransactions++;
    
    return suc;
}

bool Bank::transfer(string tableName1,string tableName2, string userName1, string passWord1 ,string userName2, string passWord2, double amount) {
    if (userName1 == userName2 && passWord1==passWord2) {
        cerr << "Cannot transfer to the same account!" << endl;
        return false;
    }
        // Update database
        bool x=this.withdraw( tableName1,  userName1,  passWord1, amount);
        if (!x) return false;


        bool y=this.deposit( tableName2,  userName2,  passWord2 , amount);
        if (!y) {
        // rollback add amount into account 1
        deposit(tableName1, userName1, passWord1, amount);
        return false;
    }


    //print for Receipt 
        auto transaction = make_shared<Transaction>(
        "transfer", account, amount, "Bank transfer"
    );
    
        transactionHistory.push_back(transaction);
        totalTransactions++;
    
    return true;
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


void Bank::simulateConcurrentTransactions(const string& tableName) {
    cout << "\n=== Simulating Concurrent Transactions ===" << endl;
    
    vector<thread> threads;
    random_device rd;
    mt19937 gen(rd());
    
    vector<shared_ptr<Account>> accounts = getAllAccounts(tableName);
    
    if (accounts.size() < 2) {
        cout << "Need at least 2 accounts for concurrent transactions demo!" << endl;
        cout << "Creating test accounts first..." << endl;
        
        // Create test accounts if not enough
        createAccount("TestUser1", 1000, tableName, "USD", "test1", "pass1");
        createAccount("TestUser2", 2000, tableName, "USD", "test2", "pass2");
        createAccount("TestUser3", 500, tableName, "USD", "test3", "pass3");
        
        accounts = getAllAccounts(tableName);
    }
    
    uniform_int_distribution<> accountDist(0, accounts.size() - 1);
    uniform_int_distribution<> amountDist(50, 500);
    uniform_int_distribution<> opDist(0, 2);
    
    cout << "Starting 10 concurrent threads..." << endl;
    
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([this, i, &gen, &accountDist, &amountDist, &opDist, tableName]() {
            int fromIdx = accountDist(gen);
            int toIdx = accountDist(gen);
            double amount = amountDist(gen);
            int op = opDist(gen);
            
            shared_ptr<Account> fromAccount = accounts[fromIdx];
            
            if (!fromAccount) return;
            
            cout << "Thread " << i << ": Starting operation..." << endl;
            
            try {
                switch (op) {
                    case 0: {
                        cout << "Thread " << i << ": Depositing $" << amount 
                             << " to account " << fromAccount->getuserName() << endl;
                        deposit(tableName, fromAccount->getuserName(), fromAccount->getpassword(), amount);
                        break;
                    }
                    case 1: {
                        cout << "Thread " << i << ": Withdrawing $" << amount 
                             << " from account " << fromAccount->getuserName() << endl;
                        withdraw(tableName, fromAccount->getuserName(), fromAccount->getpassword(), amount);
                        break;
                    }
                    case 2: {
                        if (fromIdx != toIdx) {
                            shared_ptr<Account> toAccount = accounts[toIdx];
                            if (toAccount) {
                                cout << "Thread " << i << ": Transferring $" << amount 
                                     << " from " << fromAccount->getuserName() 
                                     << " to " << toAccount->getuserName() << endl;
                                transfer(tableName, tableName, 
                                        fromAccount->getuserName(), fromAccount->getpassword(),
                                        toAccount->getuserName(), toAccount->getpassword(), amount);
                            }
                        }
                        break;
                    }
                }
            } catch (const exception& e) {
                cerr << "Thread " << i << ": Error - " << e.what() << endl;
            }
            
            cout << "Thread " << i << ": Operation completed." << endl;
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
    
    cout << "All concurrent transactions completed!" << endl;
    cout << "Total transactions executed: " << totalTransactions.load() << endl;
    cout << "======================================\n" << endl;
}