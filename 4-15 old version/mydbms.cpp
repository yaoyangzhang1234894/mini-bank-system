#include <iostream>
#include <string>
#include <sqlite3.h>
#include <vector>
#include "mydbms.h"
#include "Account.h"
using namespace std;




     bool mydbms::createDB(string name){
        dbName=name;
        if (sqlite3_open((dbName + ".db").c_str(), &db) != SQLITE_OK) {
            cerr << "error database fail created: " << sqlite3_errmsg(db) << endl;
            return 0;
        }
        return 1;
}
    bool mydbms::createTable(string tableName){
        string createTableSQL = "CREATE TABLE IF NOT EXISTS " + tableName + " (" \
                                "accountId INTEGER PRIMARY KEY AUTOINCREMENT," \
                                "accountHolderName TEXT NOT NULL," \
                                "balance DOUBLE," \
                                "currency TEXT," \
                                "userName TEXT," \
                                "passWord TEXT," \
                                "isActive INTEGER," \
                                "UNIQUE(userName, passWord) );";

        int rc = sqlite3_exec(db, createTableSQL.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            cerr << "failed: " << errMsg << endl;
            sqlite3_free(errMsg);
            return 0;
        } else {
            cout << "sucessful" << endl;
            return 1;
        }

        }
    bool mydbms::insertData(string accountHolderName, string tableName, int initialBalance, string currency, string userName, string passWord){
        string insertSQL = "INSERT INTO " + tableName + " (accountHolderName, balance, currency, userName, passWord, isActive) VALUES " \
                                "('" + accountHolderName + "', " + to_string(initialBalance) + ", '"+ currency +  "', '" + userName + "', '" + passWord + "', " +  "1);";
        if (sqlite3_exec(db, insertSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            cerr << "insert error: " << errMsg << endl;
            sqlite3_free(errMsg);
            return 0;
        } else {
            cout << "insert sucessful"<< endl;
            return 1;
        }
    }
       
    Account mydbms::queryData(string tableName, string userName, string passWord){
        // store data into vector(call back fucntion for query function)
        auto lambda_callback = [](void* data, int argc, char** argv, char** colName) -> int {
            vector<Account>* users = (vector<Account>*)data;
            if (argc >= 7) {
                Account u(stoi(argv[0]), argv[1], stoi(argv[2]), argv[3], argv[4], argv[5]);
                users->push_back(u);
            }

    
        return 0;
    };


            string querySQL = "SELECT * FROM " + tableName + " WHERE userName = '" + userName + "' AND passWord = '" + passWord + "';";
            vector<Account> allUsers;
           int rc =sqlite3_exec(db, querySQL.c_str(), lambda_callback, &allUsers, nullptr);
            if ( rc != SQLITE_OK) {
            cerr << "query error: " << errMsg << endl;
            sqlite3_free(errMsg);
            throw runtime_error("Query failed");
        }
        
        if (allUsers.empty()) {
            throw runtime_error("User not found");
        }
        
        return allUsers[0];
    }

    vector<Account> mydbms::queryAllData(string tableName){
        // store data into vector(call back fucntion for query function)
        auto lambda_callback = [](void* data, int argc, char** argv, char** colName) -> int {
            vector<Account>* users = (vector<Account>*)data;
            if (argc >= 7) {
                Account u(stoi(argv[0]), argv[1], stoi(argv[2]), argv[3], argv[4], argv[5]);
                users->push_back(u);
            }

    
        return 0;
    };


            string querySQL = "SELECT * FROM " + tableName;
            vector<Account> allUsers;
           int rc =sqlite3_exec(db, querySQL.c_str(), lambda_callback, &allUsers, nullptr);
            if ( rc != SQLITE_OK) {
            cerr << "query error: " << errMsg << endl;
            sqlite3_free(errMsg);
            throw runtime_error("Query failed");
        }
        
        if (allUsers.empty()) {
            throw runtime_error("User not found");
        }
        
        return allUsers;
    }
    
        bool mydbms::deleteData(string tableName, string userName, string passWord){
            string deleteSQL = "DELETE FROM " + tableName + " WHERE userName = '" + userName + "' AND passWord = '" + passWord + "';";
            if (sqlite3_exec(db, deleteSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
                cerr << "fail delete: " << errMsg << endl;
                sqlite3_free(errMsg);
                return 0;
            } else {
                cout << "delete successfully " << endl;
                return 1;
            }

        }




    


     bool mydbms::updataeData(string tableName, string userName, string passWord, string colname, double value){
        string updateSQL = "UPDATE " + tableName + " SET " + colname + " = " + to_string(value) + 
                       " WHERE userName = '" + userName + "' AND passWord = '" + passWord + "';";
        if (sqlite3_exec(db, updateSQL.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            cerr << "update failed: " << errMsg << endl;
            sqlite3_free(errMsg);
            return 0;
        } else {
            cout << "update successful " <<  endl;
        }
        
        
        return 1;


     }
    

    
