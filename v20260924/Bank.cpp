#include "Bank.h"
#include <random>
#include <algorithm>
#include <numeric>
#include <sqlite3.h>
using namespace std;

Bank::Bank(const string& name, const string& tablename) : bankName(name) {
    db.createDB(tablename);
    db.createTable(tablename);
}

bool Bank::createTable(const string& dbname, const string& tablename){
    return db.createTable(tablename);
}

bool Bank::createAccount(const string& holderName, double initialBalance, 
                         const string& tableName, const string& currency, 
                         const string& userName, const string& passWord) {
    lock_guard<mutex> lock(bankMutex);
    shared_ptr<Account> result = getAccount(tableName, userName, passWord);
    if(result){
        cout << "Account already exists!" << endl;
        return false;
    }
    return db.insertData(holderName, tableName, initialBalance, currency, userName, passWord);
}

bool Bank::closeAccount(const string& tableName, const string& userName, const string& passWord) {
    lock_guard<mutex> lock(bankMutex);
    bool it = db.deleteData(tableName, userName, passWord);
    if(it == true){
        cout << "Account closed successfully!" << endl;
        return true;
    }
    cout << "error" << endl;
    return false;
}

shared_ptr<Account> Bank::getAccount(const string& tableName, const string& userName, const string& passWord) {
    try {
        Account result = db.queryData(tableName, userName, passWord);
        return make_shared<Account>(move(result));  
    } catch (...) {
        return nullptr;
    }
}

vector<shared_ptr<Account>> Bank::getAllAccounts(const string& tableName) {
    lock_guard<mutex> lock(bankMutex);
    vector<Account> result = db.queryAllData(tableName);
    vector<shared_ptr<Account>> accounts;
    for (auto& acc : result) {
        accounts.push_back(make_shared<Account>(move(acc)));
    }
    return accounts;
}
// without this transfer will have deadlock with itsself
bool Bank::depositwithoutlock(const string& tableName, const string& userName, const string& passWord, double amount) {
    shared_ptr<Account> x = getAccount(tableName, userName, passWord);
    if (!x) return false;
    
    double balance = x->getBalance();
    bool suc = db.updateData(tableName, userName, passWord, "balance", balance + amount);
    
    if (suc) {
        auto transaction = make_shared<Transaction>("deposit", x, amount, "Bank deposit");
        if (transaction->execute()) {  
            transactionHistory.push_back(transaction);
            totalTransactions++;
        }
        cout << "Deposit successful! New balance: " << (balance + amount) << endl;
    }
    
    return suc;
}
bool Bank::deposit(const string& tableName, const string& userName, const string& passWord, double amount) {
    lock_guard<mutex> lock(bankMutex);
    return depositwithoutlock(tableName,userName, passWord,amount);
}
// without this transfer will have deadlock with itsself
bool Bank::withdrawwithoutlock(const string& tableName, const string& userName, const string& passWord, double amount) {
    shared_ptr<Account> x = getAccount(tableName, userName, passWord);
    if (!x) return false;
    
    double balance = x->getBalance();
    if (balance < amount) {
        cerr << "Insufficient funds!" << endl;
        return false;
    }
    
    bool suc = db.updateData(tableName, userName, passWord, "balance", balance - amount);
    
    if (suc) {
        auto transaction = make_shared<Transaction>("withdraw", x, amount, "Bank withdrawal");
        if (transaction->execute()) {  
            transactionHistory.push_back(transaction);
            totalTransactions++;
        }
        cout << "Withdrawal successful! New balance: " << (balance - amount) << endl;
    }
    
    return suc;
}
bool Bank::withdraw(const string& tableName, const string& userName, const string& passWord, double amount){
    lock_guard<mutex> lock(bankMutex);
    return withdrawwithoutlock(tableName, userName, passWord, amount);
}
bool Bank::transfer(const string& tableName1, const string& tableName2, 
                    const string& userName1, const string& passWord1,
                    const string& userName2, const string& passWord2, double amount) {
    lock_guard<mutex> lock(bankMutex);
    
    if (userName1 == userName2 && passWord1 == passWord2) {
        cerr << "Cannot transfer to the same account!" << endl;
        return false;
    }
    
    bool x = withdrawwithoutlock(tableName1, userName1, passWord1, amount);
    if (!x) return false;

    bool y = depositwithoutlock(tableName2, userName2, passWord2, amount);
    if (!y) {
        depositwithoutlock(tableName1, userName1, passWord1, amount);
        return false;
    }

    shared_ptr<Account> fromAcc = getAccount(tableName1, userName1, passWord1);
    auto transaction = make_shared<Transaction>("transfer", fromAcc, amount, "Bank transfer");
    if (transaction->execute()) {  
            transactionHistory.push_back(transaction);
            totalTransactions++;
        }
    
    cout << "Transfer successful!" << endl;
    return true;
}

double Bank::getAccountBalance(const string& tableName, const string& userName, const string& passWord) {
    shared_ptr<Account> x = getAccount(tableName, userName, passWord);
    if (x) return x->getBalance();
    return 0;
}

void Bank::displayAllAccounts(const string& tableName) {
    vector<shared_ptr<Account>> accounts = getAllAccounts(tableName);
    
    cout << "\n=== All Accounts in " << bankName << " ===" << endl;
    for (const auto& record : accounts) {
        cout << record->getAccountSummary() << endl;
    }
    cout << "Total Accounts: " << accounts.size() << endl;
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

int Bank::getTotalAccounts(const string& tableName) {
    return getAllAccounts(tableName).size();
}

double Bank::getTotalBankBalance(const string& tableName) {
    vector<shared_ptr<Account>> accounts = getAllAccounts(tableName);
    
    double total = 0.0;
    for (const auto& record : accounts) {
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
            
            vector<shared_ptr<Account>> accs = getAllAccounts(tableName);
            if (fromIdx >= (int)accs.size()) return;
            shared_ptr<Account> fromAccount = accs[fromIdx];
            
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
                            vector<shared_ptr<Account>> accs2 = getAllAccounts(tableName);
                            if (toIdx < (int)accs2.size()) {
                                shared_ptr<Account> toAccount = accs2[toIdx];
                                if (toAccount) {
                                    cout << "Thread " << i << ": Transferring $" << amount 
                                         << " from " << fromAccount->getuserName() 
                                         << " to " << toAccount->getuserName() << endl;
                                    transfer(tableName, tableName, 
                                            fromAccount->getuserName(), fromAccount->getpassword(),
                                            toAccount->getuserName(), toAccount->getpassword(), amount);
                                }
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