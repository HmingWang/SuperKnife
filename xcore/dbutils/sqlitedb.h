#include <sqlite3.h>
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <memory>
#include <iostream>

class SQLiteDB
{
public:
    // 打开数据库连接
    explicit SQLiteDB(const std::string &db_path);

    // 关闭数据库连接
    ~SQLiteDB();

    // 执行不返回结果的SQL语句
    void execute(const std::string &sql);

    // 执行带参数的SQL语句（使用预处理语句）
    void execute(const std::string &sql, const std::vector<std::string> &params);

    // 查询数据（返回结果集）
    std::vector<std::vector<std::string>> query(const std::string &sql, const std::vector<std::string> &params);

    // 开始事务
    void beginTransaction();

    // 提交事务
    void commit();

    // 回滚事务
    void rollback();

    // 事务范围保护（RAII方式）
    class Transaction
    {
    public:
        explicit Transaction(SQLiteDB &db);

        void commit();

        ~Transaction();

        // 禁用复制
        Transaction(const Transaction &) = delete;
        Transaction &operator=(const Transaction &) = delete;

    private:
        SQLiteDB &db_;
        bool committed;
    };

    // 获取最后插入的ID
    int64_t lastInsertId() const;

private:
    sqlite3 *db_ = nullptr;

    // 获取最后的错误信息
    std::string getLastError() const;

    // 禁用复制
    SQLiteDB(const SQLiteDB &) = delete;
    SQLiteDB &operator=(const SQLiteDB &) = delete;
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