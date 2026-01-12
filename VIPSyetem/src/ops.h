// ops.h
// 各个子进程要执行的操作接口声明（带中文说明）
// 设计要点：
// - 父进程负责与用户交互并将输入序列化到临时 request 文件（位于 /tmp），子进程读取 request 并将结果写回 result 文件，
//   父进程等待子进程结束后统一读取 result 并打印到终端，避免子进程直接向终端写造成刷屏或输出混淆。
// - 所有 request/result 均使用简单的 `|` 分隔字段，格式在每个函数声明上注明。

#ifndef VIP_OPS_H
#define VIP_OPS_H

#include <string>

// 列出商品。对于只读操作，request_path 可为空，结果写到 result_path
// result 例子（换行分隔）：
// -- 产品列表 --
// 1 - 名称 (Physical) : $price + ship $ship = $final
void op_list_products(const std::string &product_path, const std::string &request_path, const std::string &result_path);

// 添加商品。request 内容为：type|name|price|extra
// type = P 或 D。执行后将写入 "添加完成，ID=xx" 到 result
void op_add_product(const std::string &product_path, const std::string &request_path, const std::string &result_path);

// 列出用户，result 为多行用户信息
void op_list_users(const std::string &user_path, const std::string &request_path, const std::string &result_path);

// 注册用户。request 内容为：name|balance，result 写入注册结果
void op_register_user(const std::string &user_path, const std::string &request_path, const std::string &result_path);

// 购买商品。request 内容为：uid|pid，result 写入购买结果或错误信息
void op_purchase(const std::string &product_path, const std::string &user_path, const std::string &request_path, const std::string &result_path);

// 升级 VIP。request 内容为：uid，result 写入升级结果
void op_upgrade_vip(const std::string &user_path, const std::string &request_path, const std::string &result_path);

#endif
