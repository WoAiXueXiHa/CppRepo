#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

// Record 基类
// Record: 基础记录类型，包含唯一 id
class Record {
public:
    Record() {}
    Record(int id) : _id(id) {}
    virtual ~Record() {}
    int getId() const { return _id; }
protected:
    int _id;                 // 记录 id
};

// Transaction: 交易记录，包含用户 id、商品 id、数量与总价
class Transaction : public Record {
public:
    Transaction();
    Transaction(int id, int userId, int productId, int qty, double total);

    int getUserId() const;
    int getProductId() const;
    int getQty() const;
    double getTotal() const;

    // CSV 序列化/反序列化
    std::string toCsv() const;
    static Transaction fromCsv(const std::string &line);

private:
    int _userId;             // 购买用户 id
    int _productId;          // 商品 id
    int _qty;                // 数量
    double _total;           // 总价
};

#endif // TRANSACTION_H
