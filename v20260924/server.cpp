#include <iostream>
#include <thread>
#include "httplib.h"      // HTTP服务器
#include "json.hpp" // JSON处理
#include "Bank.h"

using namespace std;
using json = nlohmann::json;

// 全局Bank实例（你的现有代码完全不用改）
unique_ptr<Bank> bank;

// 辅助函数：从请求中解析JSON
json parseRequestBody(const httplib::Request& req) {
    try {
        return json::parse(req.body);
    } catch (...) {
        return json::object();
    }
}

// 辅助函数：返回JSON响应
void sendJsonResponse(httplib::Response& res, const json& data, int statusCode = 200) {
    res.set_content(data.dump(), "application/json");
    res.status = statusCode;
}

int main() {
    string tableName = "accounts";
    bank = make_unique<Bank>("MyBank", tableName);
    
    // 确保数据库和表存在
    bank->createTable("", tableName);
    
    // 创建httplib服务器
    httplib::Server svr;

    // ============================================================
    // API 1: 获取所有账户
    // GET /api/accounts
    // 返回: [{accountId, accountHolderName, balance, currency, userName, isActive}, ...]
    // ============================================================
    svr.Get("/api/accounts", [&](const httplib::Request& req, httplib::Response& res) {
        cout << "[API] GET /api/accounts called" << endl;
        
        auto accounts = bank->getAllAccounts(tableName);
        json result = json::array();
        
        for (auto& acc : accounts) {
            result.push_back({
                {"accountId", acc->getId()},
                {"accountHolderName", acc->getHolderName()},
                {"balance", acc->getBalance()},
                {"currency", acc->getCurrency()},      // 你需要加这个getter
                {"userName", acc->getuserName()},
                {"isActive", acc->isAccountActive()}
            });
        }
        
        sendJsonResponse(res, {{"success", true}, {"data", result}});
    });

    // ============================================================
    // API: 获取单个账户
    // GET /api/accounts/{userName}
    // 例如: GET /api/accounts/zhangsan
    // ============================================================
    svr.Get(R"(/api/accounts/(\w+))", [&](const httplib::Request& req, httplib::Response& res) {
        // 从URL中提取用户名
        string userName = req.matches[1];  // matches[0]是完整匹配，matches[1]是第一个括号里的内容
        
        cout << "[API] GET /api/accounts/" << userName << " called" << endl;
        
        auto accounts = bank->getAllAccounts(tableName);
        
        // 遍历查找匹配的用户名
        for (auto& acc : accounts) {
            if (acc->getuserName() == userName) {
                json result = {
                    {"accountId", acc->getId()},
                    {"accountHolderName", acc->getHolderName()},
                    {"balance", acc->getBalance()},
                    {"currency", acc->getCurrency()},
                    {"userName", acc->getuserName()},
                    {"isActive", acc->isAccountActive()}
                };
                sendJsonResponse(res, {{"success", true}, {"data", result}});
                return;
            }
        }
        
        // 没找到用户
        sendJsonResponse(res, {{"success", false}, {"error", "User not found"}}, 404);
    });

    // ============================================================
    // API 2: 创建账户
    // POST /api/accounts
    // Body: {holderName, initialBalance, currency, userName, passWord}
    // ============================================================
    svr.Post("/api/accounts", [&](const httplib::Request& req, httplib::Response& res) {
        cout << "[API] POST /api/accounts called" << endl;
        
        json body = parseRequestBody(req);
        
        string holderName = body.value("holderName", "");
        double initialBalance = body.value("initialBalance", 0.0);
        string currency = body.value("currency", "CAD");
        string userName = body.value("userName", "");
        string passWord = body.value("passWord", "");
        
        if (holderName.empty() || userName.empty() || passWord.empty()) {
            sendJsonResponse(res, {{"success", false}, {"error", "Missing required fields"}}, 400);
            return;
        }
        
        bool success = bank->createAccount(holderName, initialBalance, tableName, currency, userName, passWord);
        sendJsonResponse(res, {{"success", success}});
    });

    // ============================================================
    // API 3: 存款
    // POST /api/deposit
    // Body: {userName, passWord, amount}
    // ============================================================
    svr.Post("/api/deposit", [&](const httplib::Request& req, httplib::Response& res) {
        cout << "[API] POST /api/deposit called" << endl;
        
        json body = parseRequestBody(req);
        string userName = body.value("userName", "");
        string passWord = body.value("passWord", "");
        double amount = body.value("amount", 0.0);
        
        if (amount <= 0) {
            sendJsonResponse(res, {{"success", false}, {"error", "Amount must be positive"}}, 400);
            return;
        }
        
        bool success = bank->deposit(tableName, userName, passWord, amount);
        
        // 获取更新后的余额
        double newBalance = 0;
        if (success) {
            auto acc = bank->getAccount(tableName, userName, passWord);
            if (acc) newBalance = acc->getBalance();
        }
        
        sendJsonResponse(res, {
            {"success", success},
            {"newBalance", newBalance}
        });
    });

    // ============================================================
    // API 4: 取款
    // POST /api/withdraw
    // Body: {userName, passWord, amount}
    // ============================================================
    svr.Post("/api/withdraw", [&](const httplib::Request& req, httplib::Response& res) {
        cout << "[API] POST /api/withdraw called" << endl;
        
        json body = parseRequestBody(req);
        string userName = body.value("userName", "");
        string passWord = body.value("passWord", "");
        double amount = body.value("amount", 0.0);
        
        if (amount <= 0) {
            sendJsonResponse(res, {{"success", false}, {"error", "Amount must be positive"}}, 400);
            return;
        }
        
        bool success = bank->withdraw(tableName, userName, passWord, amount);
        
        double newBalance = 0;
        if (success) {
            auto acc = bank->getAccount(tableName, userName, passWord);
            if (acc) newBalance = acc->getBalance();
        }
        
        sendJsonResponse(res, {
            {"success", success},
            {"newBalance", newBalance}
        });
    });

    // ============================================================
    // API 5: 转账
    // POST /api/transfer
    // Body: {fromUserName, fromPassword, toUserName, toPassword, amount}
    // ============================================================
    svr.Post("/api/transfer", [&](const httplib::Request& req, httplib::Response& res) {
        cout << "[API] POST /api/transfer called" << endl;
        
        json body = parseRequestBody(req);
        string fromUser = body.value("fromUserName", "");
        string fromPass = body.value("fromPassword", "");
        string toUser = body.value("toUserName", "");
        string toPass = body.value("toPassword", "");
        double amount = body.value("amount", 0.0);
        
        if (amount <= 0) {
            sendJsonResponse(res, {{"success", false}, {"error", "Amount must be positive"}}, 400);
            return;
        }
        
        bool success = bank->transfer(tableName, tableName, fromUser, fromPass, toUser, toPass, amount);
        sendJsonResponse(res, {{"success", success}});
    });

    // ============================================================
    // API 6: 删除账户
    // DELETE /api/accounts
    // Body: {userName, passWord}
    // ============================================================
    svr.Delete("/api/accounts", [&](const httplib::Request& req, httplib::Response& res) {
        cout << "[API] DELETE /api/accounts called" << endl;
        
        json body = parseRequestBody(req);
        string userName = body.value("userName", "");
        string passWord = body.value("passWord", "");
        
        bool success = bank->closeAccount(tableName, userName, passWord);
        sendJsonResponse(res, {{"success", success}});
    });
    // ============================================================
    // API 7: 获取交易历史（真正可用的版本）
    // GET /api/transactions
    // ============================================================
    svr.Get("/api/transactions", [&](const httplib::Request& req, httplib::Response& res) {
        cout << "[API] GET /api/transactions called" << endl;
        
        // 1. 从 Bank 中拿到所有交易记录的引用
        const auto& history = bank->getTransactionHistory();
        
        // 2. 创建一个 JSON 数组
        json result = json::array();
        
        // 3. 遍历每一条交易记录，转成 JSON 对象
        for (const auto& tx : history) {
            // tx 是 shared_ptr<Transaction>
            result.push_back({
                {"id", tx->getId()},
                {"type", tx->getType()},           
                {"amount", tx->getAmount()},       
                {"status", tx->getStatus()},       
                {"description", tx->getDescription()}, 
                {"timestamp", ""}        
            });
        }
        
        // 4. 返回给前端
        sendJsonResponse(res, {{"success", true}, {"data", result}});
    });
    // ============================================================
    // 跨域支持 (CORS)
    // 让浏览器允许来自 http://localhost:5173 的前端访问
    // ============================================================
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type"}
    });

    // 处理浏览器的预检请求（OPTIONS）
    svr.Options(".*", [](const httplib::Request& req, httplib::Response& res) {
        res.status = 204;
    });
    // ============================================================
    // 启动服务器
    // ============================================================
    cout << "\n========================================" << endl;
    cout << "Bank API Server is running!" << endl;
    cout << "Listening on http://localhost:8080" << endl;
    cout << "Available endpoints:" << endl;
    cout << "   GET  /api/accounts     - List all accounts" << endl;
    cout << "   POST /api/accounts     - Create account" << endl;
    cout << "   POST /api/deposit      - Deposit money" << endl;
    cout << "   POST /api/withdraw     - Withdraw money" << endl;
    cout << "   POST /api/transfer     - Transfer money" << endl;
    cout << "   DELETE /api/accounts   - Close account" << endl;
    cout << "   GET  /api/transactions - Transaction history" << endl;
    cout << "========================================\n" << endl;
    
    svr.listen("0.0.0.0", 8080);
    
    return 0;
}