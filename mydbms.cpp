#include <iostream>
#include <string>
#include <sqlite3.h>
#include <vector>
#include "mydbms.h"
#include "Account.h"
using namespace std;

bool mydbms::createDB(string name){
    dbName = name;
    if (sqlite3_open((dbName + ".db").c_str(), &db) != SQLITE_OK) {
        cerr << "error database fail created: " << sqlite3_errmsg(db) << endl;
        return false;
    }
    return true;
}
//how to create and manage database in c++ is similar to javas
bool mydbms::createTable(string tableName){
    string createTableSQL = "CREATE TABLE IF NOT EXISTS " + tableName + " (" \
                            "accountId INTEGER PRIMARY KEY AUTOINCREMENT," \
                            "accountHolderName TEXT NOT NULL," \
                            "balance DOUBLE," \
                            "currency TEXT," \
                            "userName TEXT," \
                            "passWord TEXT," \
                            "isActive INTEGER," \
                            "UNIQUE(userName) );";

    rc = sqlite3_exec(db, createTableSQL.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "failed: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    } else {
        cout << "Table created successfully" << endl;
        return true;
    }
}

bool mydbms::insertData(string accountHolderName, string tableName, double initialBalance, string currency, string userName, string passWord){
    string insertSQL = "INSERT INTO " + tableName + " (accountHolderName, balance, currency, userName, passWord, isActive) VALUES " \
                            "('" + accountHolderName + "', " + to_string(initialBalance) + ", '"+ currency +  "', '" + userName + "', '" + passWord + "', " +  "1);";
    if (sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "insert error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    } else {
        cout << "insert successful" << endl;
        return true;
    }
}
   
Account mydbms::queryData(string tableName, string userName, string passWord){
    // look back function if sqlite3_exec has somthing to return use this fucntion to get the return data detail see docxs file 
    auto lambda_callback = [](void* data, int argc, char** argv, char** colName) -> int {
        vector<Account>* users = (vector<Account>*)data;
        if (argc >= 7) {
            Account u(stoi(argv[0]), argv[1], stod(argv[2]), argv[3], argv[4], argv[5]);
            users->push_back(move(u));
        }
        return 0;
    };

    string querySQL = "SELECT * FROM " + tableName + " WHERE userName = '" + userName + "' AND passWord = '" + passWord + "';";
    vector<Account> allUsers;
    rc = sqlite3_exec(db, querySQL.c_str(), lambda_callback, &allUsers, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "query error: " << errMsg << endl;
        sqlite3_free(errMsg);
        throw runtime_error("Query failed");
    }
    
    if (allUsers.empty()) {
        throw runtime_error("User not found");
    }
    
    return move(allUsers[0]);
}

vector<Account> mydbms::queryAllData(string tableName){
    auto lambda_callback = [](void* data, int argc, char** argv, char** colName) -> int {
        vector<Account>* users = (vector<Account>*)data;
        if (argc >= 7) {
            Account u(stoi(argv[0]), argv[1], stod(argv[2]), argv[3], argv[4], argv[5]);
            users->push_back(move(u));
        }
        return 0;
    };

    string querySQL = "SELECT * FROM " + tableName;
    vector<Account> allUsers;
    rc = sqlite3_exec(db, querySQL.c_str(), lambda_callback, &allUsers, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "query error: " << errMsg << endl;
        sqlite3_free(errMsg);
        throw runtime_error("Query failed");
    }
    
    return allUsers;
}

bool mydbms::deleteData(string tableName, string userName, string passWord){
    string deleteSQL = "DELETE FROM " + tableName + " WHERE userName = '" + userName + "' AND passWord = '" + passWord + "';";
    if (sqlite3_exec(db, deleteSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "fail delete: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    } else {
        cout << "delete successfully " << endl;
        return true;
    }
}

bool mydbms::updateData(string tableName, string userName, string passWord, string colname, double value){
    string updateSQL = "UPDATE " + tableName + " SET " + colname + " = " + to_string(value) + 
                   " WHERE userName = '" + userName + "' AND passWord = '" + passWord + "';";
    if (sqlite3_exec(db, updateSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
        cerr << "update failed: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    } else {
        cout << "update successful " <<  endl;
    }
    return true;
}