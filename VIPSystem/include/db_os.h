#ifndef DB_OS_H
#define DB_OS_H

#include <string>

// db_os: 基于 POSIX 系统调用的简单文件读写封装
// 目的：展示操作系统层面的文件管理（open/read/write/close）思想，
// 供上层模块以字符串为单位读写小规模 CSV 文本数据。
// 注意：实现以简单、可靠为主，适合教学与作业场景，不做复杂错误恢复或并发控制。

namespace db_os {

// 读取整个文本文件到 out 字符串中，成功返回 true，失败返回 false。
// path: 文件路径（相对或绝对），out: 输出的文件内容（覆盖原有内容）。
bool read_entire_file(const std::string &path, std::string &out);

// 覆盖写入文本文件：如果文件不存在则创建，写入完成后会调用 fsync 确保持久化。
// 成功返回 true，失败返回 false。
bool write_entire_file(const std::string &path, const std::string &data);

// 追加写入文本文件（以便做日志或追加记录使用）。
// 成功返回 true，失败返回 false。
bool append_text_file(const std::string &path, const std::string &data);

} // namespace db_os

#endif // DB_OS_H
