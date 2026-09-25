#include <iostream>
#include "mydbms.cpp"
using namespace std;

void simpleTest() {
    cout << "===== Simple Test Started =====" << endl;
    
    mydbms db;
    
    // 1. Create database
    cout << "1. Creating database..." << endl;
    if (db.createDB("test")) {
        cout << "    Success" << endl;
    } else {
        cout << "    Failed" << endl;
        return;
    }
    
    // 2. Create table
    cout << "2. Creating table..." << endl;
    if (db.createTable("users")) {
        cout << "    Success" << endl;
    } else {
        cout << "    Failed" << endl;
        return;
    }
    
    // 3. Insert test data
    cout << "3. Inserting data..." << endl;
    if (db.insertData("Zhang San", "users", 1000, "CNY", "zhang3", "123456")) {
        cout << "   Zhang San inserted successfully" << endl;
    }
    
    if (db.insertData("Li Si", "users", 2000, "USD", "li4", "abcdef")) {
        cout << "    Li Si inserted successfully" << endl;
    }
    
    // 4. Query data
    cout << "4. Querying data..." << endl;
    try {
        mydbms::User user = db.queryData("users", "zhang3", "123456");
        cout << "   Query results:" << endl;
        cout << "     Account Name: " << user.accountHolderName << endl;
        cout << "     Balance: " << user.balance << " " << user.currency << endl;
        cout << "     Username: " << user.userName << endl;
        cout << "    Query successful" << endl;
    } catch (const exception& e) {
        cout << "   Query failed: " << e.what() << endl;
    }
    
    // 5. Update data
    cout << "5. Updating data..." << endl;
    if (db.updataeData("users", "li4", "abcdef", "balance", 2500)) {
        cout << "    Update successful" << endl;
    } else {
        cout << "    Update failed" << endl;
    }
    
    // 6. Delete data
    cout << "6. Deleting data..." << endl;
    if (db.deleteData("users", "zhang3", "123456")) {
        cout << "    Delete successful" << endl;
    } else {
        cout << "    Delete failed" << endl;
    }
    
    cout << "===== Test Completed =====" << endl;
}

int main() {
    simpleTest();
    return 0;
}