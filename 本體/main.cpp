#include <iostream>
#include <memory>
#include <thread>
#include <iomanip>
#include "Bank.h"

using namespace std;
//a short code to test all function in this bank project
int main() {
    cout << "=== Mini Bank Application ===\n" << endl;
    
    string tableName = "accounts";
    auto bank = make_unique<Bank>("MyBank", tableName);
    
    // Create some accounts
    cout << "\n--- Creating Accounts ---" << endl;
    
    bank->createAccount("John Doe", 1000.0, tableName, "USD", "john_doe", "pass123");
    bank->createAccount("Jane Smith", 2000.0, tableName, "USD", "jane_smith", "pass456");
    bank->createAccount("Bob Johnson", 500.0, tableName, "USD", "bob_johnson", "pass789");
    bank->createAccount("Alice Brown", 1500.0, tableName, "EUR", "alice_brown", "pass321");
    
    // Display all accounts
    bank->displayAllAccounts(tableName);
    
    // Perform some basic transactions
    cout << "\n--- Performing Basic Transactions ---" << endl;
    
    // Deposit
    cout << "\n1. Deposit $500 to John's account:" << endl;
    bank->deposit(tableName, "john_doe", "pass123", 500.0);
    
    // Withdrawal
    cout << "\n2. Withdraw $200 from Jane's account:" << endl;
    bank->withdraw(tableName, "jane_smith", "pass456", 200.0);
    
    // Transfer
    cout << "\n3. Transfer $300 from John to Bob:" << endl;
    bank->transfer(tableName, tableName, "john_doe", "pass123", "bob_johnson", "pass789", 300.0);
    
    // Failed transaction (insufficient funds)
    cout << "\n4. Attempt to withdraw $1000 from Bob's account (insufficient funds):" << endl;
    bank->withdraw(tableName, "bob_johnson", "pass789", 1000.0);
    
    // Display updated account information
    bank->displayAllAccounts(tableName);
    
    // Demonstrate concurrent transactions
    cout << "\n--- Concurrent Transactions Demo ---" << endl;
    bank->simulateConcurrentTransactions(tableName);
    
    // Final account status
    cout << "\n--- Final Account Status ---" << endl;
    bank->displayAllAccounts(tableName);
    
    // Display transaction history
    bank->displayTransactionHistory();
    
    // Bank statistics
    cout << "\n=== Bank Statistics ===" << endl;
    cout << "Total Accounts: " << bank->getTotalAccounts(tableName) << endl;
    cout << "Total Transactions: " << bank->getTotalTransactions() << endl;
    cout << "Total Bank Balance: $" << fixed << setprecision(2) 
         << bank->getTotalBankBalance(tableName) << endl;
    
    // Close an account
    cout << "\n--- Closing Account ---" << endl;
    bank->closeAccount(tableName, "bob_johnson", "pass789");
    bank->displayAllAccounts(tableName);
    
    cout << "\n=== Mini Bank Application Shutdown ===" << endl;
    
    return 0;
}