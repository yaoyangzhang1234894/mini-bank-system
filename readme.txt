MultiThreaded Banking System
Overview
A productiongrade banking system implemented in C++17 that demonstrates concurrent transaction processing, threadsafe account management, and persistent data storage using SQLite3. The system simulates realworld banking operations, including deposits, withdrawals, transfers, and concurrent transaction handling.
Project Highlights 
•	Threadsafe operations using mutex locks and atomic variables
•	SQLite3 integration for persistent data storage
•	Concurrent transaction simulation with 10+ parallel threads
•	RAII principles and modern C++17 features
•	Realworld banking operations: deposits, withdrawals, transfers
•	Transaction logging with timestamps and audit trail
•	Move semantics and smart pointer management


Compile
g++ -std=c++17 -pthread -o bank main.cpp Account.cpp Bank.cpp Transaction.cpp mydbms.cpp -lsqlite3
# Run
bank.exe

Sample Output
See readme.docx file

architecture
text
Main → Bank (Mutex) → mydbms (SQLite3) → Account (PerAccount Mutex)
Skills Demonstrated
•	C++17 (move semantics, smart pointers, chrono, thread library)
•	Multithreading (mutex, atomic, lock_guard)
•	SQLite3 (C API, callbacks, CRUD operations)
•	RAII (automatic lock management, database cleanup)
•	Smart pointers (unique_ptr, shared_ptr)
•	Move semantics (efficient transfer of Account objects)
•	Object-oriented Design (Bank, Account, Transaction, mydbms)
•	Concurrent transaction processing (10+ simultaneous threads)
•	Database persistence (automatic DB creation, table schema)
•	Error handling with rollback (transfer failure recovery)
•	Transaction logging (timestamps, status tracking, audit trail)
•	Thread safe account management (per account mutex locking)
•	Deadlock free design (consistent lock ordering)
•	Input validation (positive amounts, insufficient funds)
•	Production ready banking logic (deposit, withdraw, transfer)
•	Testing simulation (randomized concurrent transactions)
Test Coverage
•	Basic operations (deposit/withdraw/transfer)
•	Edge cases (insufficient funds, duplicate users)
•	Concurrent operations (10+ threads)
•	Data persistence (CRUD validation)

for future update:
1.	Try to fix error in 4-15 old version or 4-7 old version(Try to use named_mutex (lock Based on the input of the lock, for example, if you lock (name) and then lock the same name again, you will have to wait.) to lock deposit, withdraw, transfer method base on the combination of username and password and Try to use lock in Boost.Interprocess)
2.	add Currency exchange eg 1 CAD= x USD
3.	add more general mydbms system (user can define the content of each row)
4.	add a bank UI with account verification 
5.	add user manager mode




