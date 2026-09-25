#include "Bank.h"
#include <random>
#include <algorithm>
#include <numeric>
#include <sqlite3.h>
using namespace std;


Bank::Bank(const string& name) : bankName(name) {
    

}

shared_ptr<Account> Bank::createAccount(const string& holderName, 
                                            double initialBalance, 
                                            const string& currency) {
    lock_guard<mutex> lock(bankMutex);
    
    static int nextAccountId = 1000;
    int accountId = nextAccountId++;
    
    auto account = make_shared<Account>(accountId, holderName, initialBalance, currency);
    accounts[accountId] = account;
    
    // Simulate database insertion
    DatabaseRecord record{
        accountId,
        holderName,
        initialBalance,
        currency,
        true,
        chrono::system_clock::now()
    };
    databaseSim[accountId] = record;
    
    cout << "Account created successfully! ID: " << accountId << endl;
    return account;
}

bool Bank::closeAccount(int accountId) {
    lock_guard<mutex> lock(bankMutex);
    
    auto it = accounts.find(accountId);
    if (it == accounts.end()) {
        cerr << "Account " << accountId << " not found!" << endl;
        return false;
    }
    
    it->second->deactivateAccount();
    
    // Update database
    if (databaseSim.find(accountId) != databaseSim.end()) {
        databaseSim[accountId].isActive = false;
        databaseSim[accountId].lastUpdated = chrono::system_clock::now();
    }
    
    cout << "Account " << accountId << " closed successfully!" << endl;
    return true;
}

shared_ptr<Account> Bank::getAccount(int accountId) {
    lock_guard<mutex> lock(bankMutex);
    
    auto it = accounts.find(accountId);
    if (it != accounts.end()) {
        return it->second;
    }
    
    cerr << "Account " << accountId << " not found!" << endl;
    return nullptr;
}

vector<shared_ptr<Account>> Bank::getAllAccounts() const {
    lock_guard<mutex> lock(bankMutex);
    
    vector<shared_ptr<Account>> result;
    for (const auto& pair : accounts) {
        result.push_back(pair.second);
    }
    return result;
}

bool Bank::deposit(int accountId, double amount) {
    auto account = getAccount(accountId);
    if (!account) return false;
    
    auto transaction = make_shared<Transaction>(
        TransactionType::DEPOSIT, account, amount, "Bank deposit"
    );
    
    bool success = transaction->execute();
    if (success) {
        transactionHistory.push_back(transaction);
        totalTransactions++;
        
        // Update database
        lock_guard<mutex> lock(bankMutex);
        if (databaseSim.find(accountId) != databaseSim.end()) {
            databaseSim[accountId].balance = account->getBalance();
            databaseSim[accountId].lastUpdated = chrono::system_clock::now();
        }
    }
    
    return success;
}

bool Bank::withdraw(int accountId, double amount) {
    auto account = getAccount(accountId);
    if (!account) return false;
    
    auto transaction = make_shared<Transaction>(
        TransactionType::WITHDRAWAL, account, amount, "Bank withdrawal"
    );
    
    bool success = transaction->execute();
    if (success) {
        transactionHistory.push_back(transaction);
        totalTransactions++;
        
        // Update database
        lock_guard<mutex> lock(bankMutex);
        if (databaseSim.find(accountId) != databaseSim.end()) {
            databaseSim[accountId].balance = account->getBalance();
            databaseSim[accountId].lastUpdated = chrono::system_clock::now();
        }
    }
    
    return success;
}

bool Bank::transfer(int fromAccountId, int toAccountId, double amount) {
    if (fromAccountId == toAccountId) {
        cerr << "Cannot transfer to the same account!" << endl;
        return false;
    }
    
    auto fromAccount = getAccount(fromAccountId);
    auto toAccount = getAccount(toAccountId);
    
    if (!fromAccount || !toAccount) return false;
    
    auto transaction = make_shared<Transaction>(
        TransactionType::TRANSFER, fromAccount, toAccount, amount, "Bank transfer"
    );
    
    bool success = transaction->execute();
    if (success) {
        transactionHistory.push_back(transaction);
        totalTransactions++;
        
        // Update database
        lock_guard<mutex> lock(bankMutex);
        if (databaseSim.find(fromAccountId) != databaseSim.end()) {
            databaseSim[fromAccountId].balance = fromAccount->getBalance();
            databaseSim[fromAccountId].lastUpdated = chrono::system_clock::now();
        }
        if (databaseSim.find(toAccountId) != databaseSim.end()) {
            databaseSim[toAccountId].balance = toAccount->getBalance();
            databaseSim[toAccountId].lastUpdated = chrono::system_clock::now();
        }
    }
    
    return success;
}

double Bank::getAccountBalance(int accountId) const {
    lock_guard<mutex> lock(bankMutex);
    
    auto it = accounts.find(accountId);
    if (it != accounts.end()) {
        return it->second->getBalance();
    }
    
    cerr << "Account " << accountId << " not found!" << endl;
    return -1;
}

void Bank::displayAllAccounts() const {
    lock_guard<mutex> lock(bankMutex);
    
    cout << "\n=== All Accounts in " << bankName << " ===" << endl;
    for (const auto& pair : accounts) {
        cout << pair.second->getAccountSummary() << endl;
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
    cout << "Total Transactions: " << totalTransactions.load() << endl;
    cout << "==========================\n" << endl;
}

void Bank::syncToDatabase() {
    lock_guard<mutex> lock(bankMutex);
    
    cout << "Syncing to database..." << endl;
    for (auto& dbRecord : databaseSim) {
        auto it = accounts.find(dbRecord.first);
        if (it != accounts.end()) {
            dbRecord.second.balance = it->second->getBalance();
            dbRecord.second.isActive = it->second->isAccountActive();
            dbRecord.second.lastUpdated = chrono::system_clock::now();
        }
    }
    cout << "Database sync completed!" << endl;
}

void Bank::loadFromDatabase() {
    lock_guard<mutex> lock(bankMutex);
    
    cout << "Loading from database..." << endl;
    // In a real application, this would load from an actual database
    // Here we're just simulating
    for (const auto& dbRecord : databaseSim) {
        if (accounts.find(dbRecord.first) == accounts.end()) {
            // Account not in memory, would need to create it
            cout << "Account " << dbRecord.first << " loaded from database" << endl;
        }
    }
    cout << "Database load completed!" << endl;
}

void Bank::optimizeDatabaseQueries() {
    cout << "Optimizing database queries..." << endl;
    
    // Simulate query optimization techniques
    // 1. Index optimization
    cout << "  - Creating indexes on frequently queried fields..." << endl;
    
    // 2. Query caching
    cout << "  - Implementing query result caching..." << endl;
    
    // 3. Connection pooling
    cout << "  - Setting up connection pooling..." << endl;
    
    // 4. Batch processing
    cout << "  - Enabling batch processing for multiple transactions..." << endl;
    
    // Simulate 5% performance improvement
    this_thread::sleep_for(chrono::milliseconds(100));
    cout << "Database query optimization complete! (5% performance improvement expected)" << endl;
}

double Bank::getTotalBankBalance() const {
    lock_guard<mutex> lock(bankMutex);
    
    double total = 0.0;
    for (const auto& pair : accounts) {
        if (pair.second->isAccountActive()) {
            total += pair.second->getBalance();
        }
    }
    return total;
}

void Bank::simulateConcurrentTransactions() {
    cout << "\n=== Simulating Concurrent Transactions ===" << endl;
    
    // Create multiple threads to simulate concurrent banking operations
    vector<thread> threads;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> accountDist(1000, 1004);  // First 5 accounts
    uniform_int_distribution<> amountDist(10, 500);
    uniform_int_distribution<> opDist(0, 2);  // 0: deposit, 1: withdraw, 2: transfer
    
    // Create 10 concurrent transactions
    for (int i = 0; i < 10; ++i) {
        threads.emplace_back([this, i, &gen, &accountDist, &amountDist, &opDist]() {
            int fromId = accountDist(gen);
            int toId = accountDist(gen);
            double amount = amountDist(gen);
            int op = opDist(gen);
            
            cout << "Thread " << i << ": Starting operation..." << endl;
            
            switch (op) {
                case 0:
                    cout << "Thread " << i << ": Depositing $" << amount 
                              << " to account " << fromId << endl;
                    deposit(fromId, amount);
                    break;
                    
                case 1:
                    cout << "Thread " << i << ": Withdrawing $" << amount 
                              << " from account " << fromId << endl;
                    withdraw(fromId, amount);
                    break;
                    
                case 2:
                    if (fromId != toId) {
                        cout << "Thread " << i << ": Transferring $" << amount 
                                  << " from account " << fromId << " to " << toId << endl;
                        transfer(fromId, toId, amount);
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