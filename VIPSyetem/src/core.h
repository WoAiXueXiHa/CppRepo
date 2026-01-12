#ifndef VIP_CORE_H
#define VIP_CORE_H

// core.h
// 核心模块：数据模型（Product / User）与持久化接口
// 设计说明（中文注释）
// - 目的：为项目提供清晰的类结构与文件持久化能力，示范面向对象三大特性（封装/继承/多态），
//   并用 POSIX 系统调用（open/read/write/close）实现持久化，同时用 fcntl 文件锁保护并发写入。
// - 文件格式：
//   products.txt 每行为：id|type|name|price|extra
//     type: P 表示实物（extra=邮费），D 表示数字商品（extra=0）
//   users.txt 每行为：id|name|balance|is_vip
// - 并发与锁：所有对数据文件的读写均通过 core.cpp 中的 `read_file`/`write_file` 进行，
//   内部使用 `fcntl` 的记录级（此处为整文件）锁：读操作使用读锁（F_RDLCK），写操作使用写锁（F_WRLCK），
//   并使用阻塞获取锁（F_SETLKW），以避免并发写入导致的数据损坏。
// - 命名约定：所有类的私有成员变量以 `_` 前缀命名（例如 `_id`），以便与访问器区分。

#include <string>
#include <vector>

// 商品基类：封装商品通用属性，并提供虚方法以支持多态
class Product {
public:
    /**
     * 构造函数
     * 参数：
     *  - id_: 唯一商品 ID（正整数）
     *  - name_: 商品名称（非空字符串）
     *  - price_: 基础价格（浮点，>=0）
     * 说明：此构造函数仅在内存中构造对象；持久化由 `load_products` / `save_products` 负责。
     */
    Product(int id_, const std::string &name_, double price_);
    virtual ~Product();
    // 访问器：获取 ID/名称/价格
    /** 返回商品 ID */
    int getId() const;
    /** 返回商品名称 */
    std::string getName() const;
    /** 返回商品基础价格（不含邮费） */
    double getPrice() const;
    // 返回商品最终价格（虚方法，子类可覆盖，例如加邮费）
    /**
     * 计算并返回最终价格（虚方法）
     * 子类可重载，例如 `PhysicalProduct` 会加上邮费。
     */
    virtual double finalPrice() const;
    /**
     * 将对象序列化为文件行格式（用于写入 products.txt）
     * 格式示例："1|P|Wireless Mouse|25.99|3.50"
     */
    virtual std::string serialize() const;
    /** 返回用于显示在终端的摘要信息字符串（单行） */
    virtual std::string info() const;
private:
    int _id;                 // 商品 ID（私有，带下划线前缀）
    std::string _name;       // 商品名称
    double _price;           // 商品基础价格
};

// 实物商品：包含额外的邮费字段
class PhysicalProduct : public Product {
public:
    /**
     * 实物商品构造：包含邮费字段
     * 参数：shipping_ 表示邮费（>=0）
     */
    PhysicalProduct(int id_, const std::string &name_, double price_, double shipping_);
    virtual double finalPrice() const;
    virtual std::string serialize() const;
    virtual std::string info() const;
private:
    double _shipping;        // 邮费（实物商品特有，>=0）
};

// 数字商品：没有邮费
class DigitalProduct : public Product {
public:
    // 数字商品没有邮费
    /** 数字商品没有邮费 */
    DigitalProduct(int id_, const std::string &name_, double price_);
    virtual double finalPrice() const;
    virtual std::string serialize() const;
    virtual std::string info() const;
};

// 用户数据结构：封装用户信息与账户操作
class User {
public:
    /**
     * 无参构造：默认值（_id=0, _name="", _balance=0, _is_vip=false）
     */
    User();
    /**
     * 完整构造：用于从文件加载或测试
     * 参数：balance_ >= 0，vip_ 为 true 则表示 VIP
     */
    User(int id_, const std::string &name_, double balance_, bool vip_);
    /** 返回用户 ID */
    int getId() const;
    /** 返回用户名 */
    std::string getName() const;
    /** 返回当前余额 */
    double getBalance() const;
    /** 返回是否为 VIP */
    bool isVIP() const;
    /** 设置 VIP 状态 */
    void setVIP(bool v);
    /** 增加余额（可以为负以减少，调用方需保证合法性） */
    void addBalance(double x);
    /**
     * 尝试扣款，余额充足则返回 true 并修改余额；否则返回 false 且余额不变
     * 注意：函数本身不做并发控制，调用方在并发场景下需使用文件锁保证一致性（本项目通过文件级锁实现）。
     */
    bool charge(double amount);
    /** 将用户信息序列化为文件行格式，如 "1|alice|100.00|0" */
    std::string serialize() const;
    /** 返回用于显示的字符串摘要 */
    std::string info() const;
private:
    int _id;                 // 用户 ID
    std::string _name;       // 用户名
    double _balance;         // 余额
    bool _is_vip;            // 是否为 VIP
};

// file io with locking
std::string read_file(const char *path);
bool write_file(const char *path, const std::string &content);

void load_products(const std::string &path, std::vector<Product*> &out);
void save_products(const std::string &path, const std::vector<Product*> &v);
void load_users(const std::string &path, std::vector<User> &out);
void save_users(const std::string &path, const std::vector<User> &v);

bool product_cmp(Product* a, Product* b);

// helpers
std::string make_temp_path(const std::string &prefix);

#endif
