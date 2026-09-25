#ifndef MYDBMS_H
#define MYDBMS_H

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
    char* errMsg;
    int rc;

public:
    mydbms() : db(nullptr), errMsg(nullptr), rc(0) {}
    ~mydbms() { if (db) sqlite3_close(db); }
    
    bool createDB(string name);
    bool createTable(string tableName); 
    bool insertData(string accountHolderName, string tableName, double initialBalance, string currency, string userName, string passWord);
    Account queryData(string tableName, string userName, string passWord);
    vector<Account> queryAllData(string tableName);
    bool deleteData(string tableName, string userName, string passWord);
    bool updateData(string tableName, string userName, string passWord, string colname, double value);
};

#endif