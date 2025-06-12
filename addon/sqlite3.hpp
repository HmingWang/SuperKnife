#include <sqlite3.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <memory>
#include <iostream>

class SQLiteDB {
public:
    // 打开数据库连接
    explicit SQLiteDB(const std::string& db_path) {
        if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK) {
            throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db_)));
        }
    }

    // 关闭数据库连接
    ~SQLiteDB() {
        if (db_) {
            sqlite3_close(db_);
        }
    }

    // 执行不返回结果的SQL语句
    void execute(const std::string& sql) {
        char* errMsg = nullptr;
        if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK) {
            std::string error = errMsg;
            sqlite3_free(errMsg);
            throw std::runtime_error("SQL error: " + error);
        }
    }

    // 执行带参数的SQL语句（使用预处理语句）
    void execute(const std::string& sql, const std::vector<std::string>& params) {
        sqlite3_stmt* stmt;
        if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare statement: " + getLastError());
        }

        // 绑定参数
        for (int i = 0; i < params.size(); i++) {
            sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
        }

        // 执行语句
        if (sqlite3_step(stmt) != SQLITE_DONE) {
            sqlite3_finalize(stmt);
            throw std::runtime_error("Execution failed: " + getLastError());
        }

        sqlite3_finalize(stmt);
    }

    // 查询数据（返回结果集）
    std::vector<std::vector<std::string>> query(const std::string& sql,const std::vector<std::string>& params) {
        std::vector<std::vector<std::string>> results;
        sqlite3_stmt* stmt;

        if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK) {
            throw std::runtime_error("Failed to prepare query: " + getLastError());
        }

        // 绑定参数
        for (int i = 0; i < params.size(); i++) {
            sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
        }

        int columnCount = sqlite3_column_count(stmt);
        
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            std::vector<std::string> row;
            for (int i = 0; i < columnCount; i++) {
                const char* value = reinterpret_cast<const char*>(sqlite3_column_text(stmt, i));
                row.push_back(value ? value : "");
            }
            results.push_back(row);
        }

        sqlite3_finalize(stmt);
        return results;
    }

    // 开始事务
    void beginTransaction() {
        execute("BEGIN TRANSACTION;");
    }

    // 提交事务
    void commit() {
        execute("COMMIT;");
    }

    // 回滚事务
    void rollback() {
        execute("ROLLBACK;");
    }

    // 事务范围保护（RAII方式）
    class Transaction {
    public:
        explicit Transaction(SQLiteDB& db) : db_(db), committed(false) {
            db_.beginTransaction();
        }
        
        void commit() {
            db_.commit();
            committed = true;
        }
        
        ~Transaction() {
            if (!committed) {
                try {
                    db_.rollback();
                } catch (...) {
                    // 确保不抛出异常
                }
            }
        }
        
        // 禁用复制
        Transaction(const Transaction&) = delete;
        Transaction& operator=(const Transaction&) = delete;
        
    private:
        SQLiteDB& db_;
        bool committed;
    };

    // 获取最后插入的ID
    int64_t lastInsertId() const {
        return sqlite3_last_insert_rowid(db_);
    }

private:
    sqlite3* db_ = nullptr;

    // 获取最后的错误信息
    std::string getLastError() const {
        return sqlite3_errmsg(db_);
    }

    // 禁用复制
    SQLiteDB(const SQLiteDB&) = delete;
    SQLiteDB& operator=(const SQLiteDB&) = delete;
};

//=================================================================
// // 示例用法
// int main() {
//     try {
//         // 打开数据库（内存数据库）
//         SQLiteDB db(":memory:");
//
//         // 创建表
//         db.execute("CREATE TABLE users (id INTEGER PRIMARY KEY, name TEXT, email TEXT);");
//
//         // 插入数据（带参数）
//         db.execute("INSERT INTO users (name, email) VALUES (?, ?);", 
//                   {"Alice", "alice@example.com"});
//
//         // 查询数据
//         auto results = db.query("SELECT * FROM users;");
//         for (const auto& row : results) {
//             for (const auto& col : row) {
//                 std::cout << col << "\t";
//             }
//             std::cout << "\n";
//         }
//
//         // 使用事务
//         {
//             SQLiteDB::Transaction trans(db);
//
//             // 在事务中执行多个操作
//             db.execute("INSERT INTO users (name, email) VALUES (?, ?);", 
//                       {"Bob", "bob@example.com"});
//             db.execute("INSERT INTO users (name, email) VALUES (?, ?);", 
//                       {"Charlie", "charlie@example.com"});
//
//         }
//             // 提交事务
//             trans.commit();
//         }
//
//         // 获取最后插入ID
//         std::cout << "Last insert ID: " << db.lastInsertId() << std::endl;
//
//     } catch (const std::exception& e) {
//         std::cerr << "Database error: " << e.what() << std::endl;
//         return 1;
//     }
//
//     return 0;
// }