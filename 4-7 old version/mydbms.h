#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <iostream>
#include <string>
#include <sqlite3.h>
#include <vector>
#include "Account.h"
using namespace std;
class mydbms {
private:

    sqlite3* db;
    string dbName;
    char* errMsg = nullptr;


   


public:
    bool createDB(string name);
    bool createTable(string tableName);
    bool insertData(string accountHolderName, string tableName, int initialBalance, string currency, string userName, string passWord);
    Account queryData(string tableName, string userName, string passWord);
    vector<Account> queryAllData(string tableName)
    bool deleteData(string tableName, string userName, string passWord);
    bool updataeData(string tableName, string userName, string passWord, string colname, auto value);
    };

#endif
