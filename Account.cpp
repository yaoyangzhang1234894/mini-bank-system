#include "Account.h"
#include <stdexcept>
#include <regex>
#include <iostream>
#include <sstream>
#include <iomanip>
using namespace std;
//consturctors might be useful in the mydbms queryData look back function(lameda funtion)
Account::Account() : accountId(0), accountHolderName(""), balance(0), 
                    accountMutex(make_unique<mutex>()), currency("CAD"), 
                    userName(""), passWord(""), isActive(true) {}

Account::Account(int id, const string& name, double initialBalance, const string curr, string username, string password)
    : accountId(id), accountHolderName(name), balance(initialBalance), 
      accountMutex(make_unique<mutex>()), currency(curr), userName(username), 
      passWord(password), isActive(true) {

}

Account::Account(Account&& other) noexcept
    : accountId(other.accountId), accountHolderName(move(other.accountHolderName)),
      balance(other.balance), accountMutex(move(other.accountMutex)),
      currency(move(other.currency)), userName(move(other.userName)),
      passWord(move(other.passWord)), isActive(other.isActive) {}

Account& Account::operator=(Account&& other) noexcept {
    if (this != &other) {
        accountId = other.accountId;
        accountHolderName = move(other.accountHolderName);
        balance = other.balance;
        accountMutex = move(other.accountMutex);
        currency = move(other.currency);
        userName = move(other.userName);
        passWord = move(other.passWord);
        isActive = other.isActive;
    }
    return *this;
}

void Account::logTransaction(const string& type, double amount, double newBalance) {
    cout << "successfuly " << type << " " << amount << " in Account " << accountId << " current balance: " << newBalance << endl;
}

bool Account::deposit(double amount) {
    lock_guard<mutex> lock(*accountMutex);
    
    if (!isActive) {
        cerr << "Account " << accountId << " is deactivated!" << endl;
        return false;
    }
    
    if (amount <= 0) {
        cerr << "Deposit amount must be positive!" << endl;
        return false;
    }
    
    balance += amount;
    logTransaction("DEPOSIT", amount, balance);
    return true;
}

bool Account::withdraw(double amount) {
    lock_guard<mutex> lock(*accountMutex);
    
    if (!isActive) {
        cerr << "Account " << accountId << " is deactivated!" << endl;
        return false;
    }
    
    if (amount <= 0) {
        cerr << "Withdrawal amount must be positive!" << endl;
        return false;
    }
    
    if (balance < amount) {
        cerr << "Insufficient funds in account " << accountId << endl
                  << "Available: " << balance << " " << currency << endl
                  << "Requested: " << amount << " " << currency << endl;
        return false;
    }
    
    balance -= amount;
    logTransaction("WITHDRAWAL", amount, balance);
    return true;
}

double Account::getBalance() const {
    lock_guard<mutex> lock(*accountMutex);
    return balance;
}

void Account::displayAccountInfo() const {
    string x = getAccountSummary();
    cout << x << endl;
}

string Account::getAccountSummary() const {
    lock_guard<mutex> lock(*accountMutex);
    ostringstream oss;
    oss << "ID: " << accountId << " | Name: " << accountHolderName 
        << " | Balance: " << currency << " " << fixed << setprecision(2) << balance
        << " | Status: " << (isActive ? "Active" : "Inactive");
    return oss.str();
}