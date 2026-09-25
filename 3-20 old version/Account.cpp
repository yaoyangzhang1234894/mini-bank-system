#include "Account.h"
#include <stdexcept>
#include <regex>
using namespace std;
void Account::logTransaction(const string& type, double amount, double newBalance) {
    cout << "successfuly " << type <<amount<< "in "<<"Account " << accountId <<"current blance: "<<newBalance;

}

Account::Account(int id, const string& name, double initialBalance, const string curr)
    : accountId(id), accountHolderName(name), balance(initialBalance), 
      currency(curr), isActive(true) {
    regex specialChars(R"([a-zA-Z])");
    if (regex_search(accountHolderName, specialChars) || accountHolderName=="" || initialBalance < 0 || id<=0)
        throw invalid_argument("error");
    
}
Account::Account(int id, const string& name)
    : accountId(id), accountHolderName(name), balance(0), 
      currency("CAD"), isActive(true) {
    regex specialChars(R"([a-zA-Z])");
    if (regex_search(accountHolderName, specialChars) || accountHolderName=="" || id<=0)
        throw invalid_argument("error");
    
}

bool Account::deposit(double amount) {
    lock_guard<mutex> lock(accountMutex);
    
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
    lock_guard<mutex> lock(accountMutex);
    
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
    lock_guard<mutex> lock(accountMutex);
    return balance;
}

void Account::displayAccountInfo() const {
    string x=getAccountSummary();
    cout<<x;

}

string Account::getAccountSummary() const {
    lock_guard<mutex> lock(accountMutex);
    ostringstream oss;
    oss << "ID: " << accountId << " | Name: " << accountHolderName 
        << " | Balance: " << currency << " " << fixed << setprecision(2) << balance
        << " | Status: " << (isActive ? "Active" : "Inactive");
    return oss.str();
}