
#pragma once

#include "exceptions.h"
#include <cstring> // for memcpy
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
class File
{
public:
  // 打开模式枚举
  enum OpenMode
  {
    ReadOnly = std::ios::in,
    WriteOnly = std::ios::out,
    ReadWrite = std::ios::in | std::ios::out,
    Append = std::ios::app,
    BinaryReadWrite = std::ios::in | std::ios::out | std::ios::binary,
    Truncate = std::ios::trunc,
    AtEnd = std::ios::ate
  };

  // 构造函数
  explicit File(std::string_view filename) { open(filename.data()); }
  File(std::string_view filename, int mode = ReadWrite) { open(filename.data(), mode); }

  // 打开文件
  void open(const std::string &filename, int mode = ReadWrite)
  {
    close(); // 先关闭已打开的文件

    this->filename = filename;
    fileStream.open(filename, static_cast<std::ios_base::openmode>(mode));

    if (!isOpen())
      throw SystemException("Error open file :" + filename);
  }

  // 关闭文件
  void close()
  {
    if (isOpen())
    {
      fileStream.close();
      filename.clear();
    }
  }

  // 检查文件是否打开
  bool isOpen() const { return fileStream.is_open(); }
  bool good() const { return fileStream.good(); }

  // 获取文件名
  std::string getFilename() const { return filename; }

  // ========== 文本操作 ==========

  // 读取一行
  void readLine(std::string &line)
  {
    check_open();

    if (!std::getline(fileStream, line))

      if (!fileStream.eof())
        throw SystemException("failed to read line from file");
  }

  // 读取所有行
  void readAllLines(std::vector<std::string> &lines)
  {
    check_open();

    std::streampos originalPos = tell();
    seek(0, Beg);

    std::string line;
    while (std::getline(fileStream, line))
    {
      lines.push_back(line);
    }

    // 如果不是因为EOF导致的读取失败，恢复原始位置
    if (!fileStream.eof())
    {
      seek(originalPos, Beg);
    }
  }

  // 写入字符串
  void write(const std::string &content)
  {
    check_open();

    fileStream << content;
    if (fileStream.fail())
      throw SystemException("failed to write content from file");
  }

  // 写入一行
  void writeLine(const std::string &line) { return write(line + "\n"); }

  // ========== 二进制操作 ==========

  auto read_binary(Bytes &buffer)
  {
    check_open();

    fileStream.read((char *)buffer.c_ptr(), buffer.size());
    if (fileStream.fail() && !fileStream.eof())
      throw SystemException("failed to read binary from file");
    return fileStream.gcount();
  }

  // 写入二进制数据
  void writeBinary(const char *data, size_t size)
  {
    check_open();

    fileStream.write(data, size);
    if (fileStream.fail())
      throw SystemException("failed to write binary from file");
  }

  void write_binary(Bytes &data)
  {
    check_open();

    fileStream.write((const char *)data.c_cptr(), data.size());
    if (fileStream.fail())
      throw SystemException("failed to write binary from file");
  }

  // ========== 文件定位 ==========

  // 定位枚举
  enum SeekDir
  {
    Beg = std::ios::beg,
    Cur = std::ios::cur,
    End = std::ios::end
  };

  // 获取当前位置
  std::streampos tell()
  {
    check_open();

    return fileStream.tellg();
  }

  // 设置位置
  void seek(std::streampos pos, SeekDir dir = Beg)
  {
    check_open();

    fileStream.seekg(pos, static_cast<std::ios_base::seekdir>(dir));
    fileStream.seekp(pos, static_cast<std::ios_base::seekdir>(dir));

    if (fileStream.fail())
      throw SystemException("failed to seek file opsion");
  }

  // 移动到文件开头
  void rewind() { return seek(0, Beg); }

  // ========== 文件信息 ==========

  // 获取文件大小
  std::streampos size()
  {
    check_open();

    std::streampos current = tell();
    seek(0, End);
    std::streampos size = tell();
    seek(current, Beg);

    return size;
  }

  // 检查是否到达文件末尾
  bool eof() const
  {
    check_open();

    return fileStream.eof();
  }

  // 清空文件内容
  void truncate()
  {
    check_open();

    close();
    fileStream.open(filename, std::ios::out | std::ios::trunc);
  }

  // 刷新缓冲区
  void flush()
  {
    check_open();

    fileStream.flush();
    if (fileStream.fail())
      throw SystemException("failed to flush file ");
  }

  // 析构函数
  ~File() { close(); }

private:
  std::fstream fileStream;
  std::string filename;

  void check_open() const
  {
    if (!isOpen())
      throw SystemException("file not open");
  }
};
