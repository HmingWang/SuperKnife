-- 创建用户表
CREATE TABLE users (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL,
    password_hash TEXT NOT NULL,  -- 存储加盐哈希后的密码
    salt TEXT NOT NULL,           -- 密码盐值
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- 插入测试用户 (密码: admin123)
INSERT INTO users (username, password_hash, salt) 
VALUES ('admin', 'a9b9e9e6d6a5c5b4a3b2c1d0e9f8g7h6', 'randomsalt');