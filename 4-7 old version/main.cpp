#include "Bank.h"
#include <iostream>
#include <memory>
#include <thread>

int main() {
    std::cout << "=== Mini Bank Application ===\n" << std::endl;
    
    // Create a new bank
    auto bank = std::make_unique<Bank>("MyBank");
    
    // Create some accounts
    std::cout << "\n--- Creating Accounts ---" << std::endl;
    auto acc1 = bank->createAccount("John Doe", 1000.0);
    auto acc2 = bank->createAccount("Jane Smith", 2000.0);
    auto acc3 = bank->createAccount("Bob Johnson", 500.0);
    auto acc4 = bank->createAccount("Alice Brown", 1500.0, "EUR");
    
    // Display all accounts
    bank->displayAllAccounts();
    
    // Perform some basic transactions
    std::cout << "\n--- Performing Basic Transactions ---" << std::endl;
    
    // Deposit
    std::cout << "\n1. Deposit $500 to John's account:" << std::endl;
    bank->deposit(1000, 500.0);
    
    // Withdrawal
    std::cout << "\n2. Withdraw $200 from Jane's account:" << std::endl;
    bank->withdraw(1001, 200.0);
    
    // Transfer
    std::cout << "\n3. Transfer $300 from John to Bob:" << std::endl;
    bank->transfer(1000, 1002, 300.0);
    
    // Failed transaction (insufficient funds)
    std::cout << "\n4. Attempt to withdraw $1000 from Bob's account (insufficient funds):" << std::endl;
    bank->withdraw(1002, 1000.0);
    
    // Display updated account information
    bank->displayAllAccounts();
    
    // Demonstrate database operations
    std::cout << "\n--- Database Operations ---" << std::endl;
    bank->syncToDatabase();
    bank->optimizeDatabaseQueries();
    
    // Demonstrate concurrent transactions
    std::cout << "\n--- Concurrent Transactions Demo ---" << std::endl;
    bank->simulateConcurrentTransactions();
    
    // Final account status
    bank->displayAllAccounts();
    
    // Display transaction history
    bank->displayTransactionHistory();
    
    // Bank statistics
    std::cout << "\n=== Bank Statistics ===" << std::endl;
    std::cout << "Total Accounts: " << bank->getTotalAccounts() << std::endl;
    std::cout << "Total Transactions: " << bank->getTotalTransactions() << std::endl;
    std::cout << "Total Bank Balance: $" << std::fixed << std::setprecision(2) 
              << bank->getTotalBankBalance() << std::endl;
    
    // Close an account
    std::cout << "\n--- Closing Account ---" << std::endl;
    bank->closeAccount(1002);
    bank->displayAllAccounts();
    
    std::cout << "\n=== Mini Bank Application Shutdown ===" << std::endl;
    
    return 0;
}