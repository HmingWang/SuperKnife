#include "sqlitedb.h"
#include <string>
#include <vector>
#include <stdexcept>
#include <functional>
#include <memory>
#include <iostream>

// 打开数据库连接
SQLiteDB::SQLiteDB(const std::string &db_path)
{
    if (sqlite3_open(db_path.c_str(), &db_) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to open database: " + std::string(sqlite3_errmsg(db_)));
    }
}

// 关闭数据库连接
SQLiteDB::~SQLiteDB()
{
    if (db_)
    {
        sqlite3_close(db_);
    }
}


// 执行不返回结果的SQL语句
void SQLiteDB::execute(const std::string &sql)
{
    char *errMsg = nullptr;
    if (sqlite3_exec(db_, sql.c_str(), nullptr, nullptr, &errMsg) != SQLITE_OK)
    {
        std::string error = errMsg;
        sqlite3_free(errMsg);
        throw std::runtime_error("SQL error: " + error);
    }
}

// 执行带参数的SQL语句（使用预处理语句）
void SQLiteDB::execute(const std::string &sql, const std::vector<std::string> &params)
{
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to prepare statement: " + getLastError());
    }

    // 绑定参数
    for (int i = 0; i < params.size(); i++)
    {
        sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
    }

    // 执行语句
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        sqlite3_finalize(stmt);
        throw std::runtime_error("Execution failed: " + getLastError());
    }

    sqlite3_finalize(stmt);
}

// 查询数据（返回结果集）
std::vector<std::vector<std::string>> SQLiteDB::query(const std::string &sql, const std::vector<std::string> &params)
{
    std::vector<std::vector<std::string>> results;
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db_, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
    {
        throw std::runtime_error("Failed to prepare query: " + getLastError());
    }

    // 绑定参数
    for (int i = 0; i < params.size(); i++)
    {
        sqlite3_bind_text(stmt, i + 1, params[i].c_str(), -1, SQLITE_TRANSIENT);
    }

    int columnCount = sqlite3_column_count(stmt);

    while (sqlite3_step(stmt) == SQLITE_ROW)
    {
        std::vector<std::string> row;
        for (int i = 0; i < columnCount; i++)
        {
            const char *value = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));
            row.push_back(value ? value : "");
        }
        results.push_back(row);
    }

    sqlite3_finalize(stmt);
    return results;
}

// 开始事务
void SQLiteDB::beginTransaction()
{
    execute("BEGIN TRANSACTION;");
}

// 提交事务
void SQLiteDB::commit()
{
    execute("COMMIT;");
}

// 回滚事务
void SQLiteDB::rollback()
{
    execute("ROLLBACK;");
}

SQLiteDB::Transaction::Transaction(SQLiteDB &db) : db_(db), committed(false)
{
    db_.beginTransaction();
}

void SQLiteDB::Transaction::commit()
{
    db_.commit();
    committed = true;
}

SQLiteDB::Transaction::~Transaction()
{
    if (!committed)
    {
        try
        {
            db_.rollback();
        }
        catch (...)
        {
            // 确保不抛出异常
        }
    }
}

// 获取最后插入的ID
int64_t SQLiteDB::lastInsertId() const
{
    return sqlite3_last_insert_rowid(db_);
}

// 获取最后的错误信息
std::string SQLiteDB::getLastError() const
{
    return sqlite3_errmsg(db_);
}
