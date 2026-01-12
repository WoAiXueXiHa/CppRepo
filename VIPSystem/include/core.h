#ifndef CORE_H
#define CORE_H

#include <vector>
#include <string>
#include "user.h"
#include "product.h"
#include "transaction.h"

// 系统核心：使用 STL 容器管理对象，提供加载/保存/查询/增删改接口。
// 实现思路：在内存中用 std::vector 保存对象，读写时将整个 CSV 文件读入/写出。

class VIPSystemCore {
public:
    // 构造时传入三种数据文件路径
    VIPSystemCore(const std::string &usersPath,
                  const std::string &productsPath,
                  const std::string &transPath);

    // 加载/保存全部数据到内存/磁盘
    bool loadAll();
    bool saveAll();

    // 用户操作：添加与按 id 查找
    bool addUser(const User &u);
    User *findUserById(int id);

    // 商品操作：添加与按 id 查找
    bool addProduct(const Product &p);
    Product *findProductById(int id);

    // 交易操作：完成购买并生成交易记录；按用户查询交易记录
    bool makePurchase(int userId, int productId, int qty);
    std::vector<Transaction> queryTransactionsByUser(int userId) const;

    // 列表访问（只读）
    const std::vector<User> &listUsers() const;
    const std::vector<Product> &listProducts() const;
    const std::vector<Transaction> &listTransactions() const;

private:
    std::string _usersPath;               // 用户 CSV 文件路径
    std::string _productsPath;            // 商品 CSV 文件路径
    std::string _transPath;               // 交易 CSV 文件路径

    std::vector<User> _users;             // 内存中用户列表
    std::vector<Product> _products;       // 内存中商品列表
    std::vector<Transaction> _transactions; // 内存中交易记录

    // 生成下一个 id（简单策略：当前最大 id + 1）
    int nextUserId() const;
    int nextProductId() const;
    int nextTransactionId() const;
};

#endif // CORE_H
