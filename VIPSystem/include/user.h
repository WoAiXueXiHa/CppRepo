#ifndef USER_H
#define USER_H

#include <string>

// 基类 Person（演示继承）
// Person: 抽象出姓名与 id 的基类，用于演示继承关系。
class Person {
public:
    Person() {}
    // 构造函数：将唯一 id 和姓名保存为成员变量
    Person(int id, const std::string &name) : _id(id), _name(name) {}
    virtual ~Person() {}

    // 访问器：获取 id / 姓名
    int getId() const { return _id; }
    std::string getName() const { return _name; }

protected:
    int _id;                 // 唯一用户/实体 id (前缀 _ 表示成员变量)
    std::string _name;       // 姓名 / 名称
};

// User: 商场用户（VIP）信息，继承自 Person。
// 成员包含 VIP 等级与积分，提供简单的积分操作与 CSV 序列化接口。
class User : public Person {
public:
    User();
    User(int id, const std::string &name, int vipLevel, int points);

    int getVipLevel() const;
    int getPoints() const;
    void addPoints(int p);

    // CSV 序列化与反序列化，用于文件读写
    std::string toCsv() const;
    static User fromCsv(const std::string &line);

private:
    int _vipLevel;           // VIP 等级（整数，用于示例）
    int _points;             // 积分
};

#endif // USER_H
