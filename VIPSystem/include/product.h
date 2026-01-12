#ifndef PRODUCT_H
#define PRODUCT_H

#include <string>

// 基类 Item，Product 继承于 Item
// Item: 商品/实体基类，包含 id 与名称
class Item {
public:
    Item() {}
    Item(int id, const std::string &name) : _id(id), _name(name) {}
    virtual ~Item() {}

    int getId() const { return _id; }
    std::string getName() const { return _name; }

protected:
    int _id;                 // 商品唯一 id
    std::string _name;       // 商品名称
};

// Product: 商品信息（价格、库存），继承自 Item
class Product : public Item {
public:
    Product();
    Product(int id, const std::string &name, double price, int stock);

    double getPrice() const;
    int getStock() const;
    void reduceStock(int qty);
    void increaseStock(int qty);

    // CSV 序列化/反序列化
    std::string toCsv() const;
    static Product fromCsv(const std::string &line);

private:
    double _price;           // 单价
    int _stock;              // 库存数量
};

#endif // PRODUCT_H
