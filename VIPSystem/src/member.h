#pragma once
#include <string>
#include <sstream>
#include <iostream>

using namespace std;

/*
 * 会员体系（继承 + 动态）
 * - Member：抽象基类（虚函数：折扣/等级）
 * - Regular/VIP/SVIP：派生类
 */

class Member {
protected:
    string mId;
    string mName;
    string mPhone;
    int    mPoints;
    string mJoinDate;

public:
    Member() : mPoints(0) {}

    Member(const string& id, const string& name, const string& phone,
           int points, const string& joinDate)
        : mId(id), mName(name), mPhone(phone), mPoints(points), mJoinDate(joinDate) {}
    
    //必须vir析构 保证delete子类正常
    virtual ~Member() {}

    // 多态接口 不同等级返回不同折扣/名称
    // 只拿Member指针 运行时自动分配到子类
    virtual double discountRate() const = 0;
    virtual string levelName() const = 0;
    virtual int    levelCode() const = 0;

    // get函数
    const string& getId() const { return mId; }
    const string& getName() const { return mName; }
    const string& getPhone() const { return mPhone; }
    int getPoints() const { return mPoints; }
    const string& getJoinDate() const { return mJoinDate; }

    // set函数
    void setName(const string& name) { mName = name; }
    void setPhone(const string& phone) { mPhone = phone; }

    void addPoints(int delta) {
        mPoints += delta;
        if (mPoints < 0) mPoints = 0;   // 积分不可能为负数
    }

    // 写入文件 统一用 | 分隔 和读取逻辑完全一致 
    string infoTxt() const {
        ostringstream oss;
        oss << mId << " | " << mName << " | " << mPhone << " | "
            << levelCode() << " | " << mPoints << " | " << mJoinDate;
        return oss.str();
    }
};

class RegularMember : public Member {
public:
    using Member::Member;
    double discountRate() const override { return 1.0; }
    string levelName() const override { return "普通"; }
    int levelCode() const override { return 0; }
};

class VipMember : public Member {
public:
    using Member::Member;
    double discountRate() const override { return 0.95; }
    string levelName() const override { return "VIP"; }
    int levelCode() const override { return 1; }
};

class SvipMember : public Member {
public:
    using Member::Member;
    double discountRate() const override { return 0.85; }
    string levelName() const override { return "SVIP"; }
    int levelCode() const override { return 2; }
};

/*
 * 从文件读 levelCode 后创建对应对象
 * 返回 new 出来的指针 由 VipSystem 统一 delete（避免内存泄漏）
 */
inline Member* createMemberByLevel(int levelCode,
                                  const string& id,
                                  const string& name,
                                  const string& phone,
                                  int points,
                                  const string& joinDate) {
    if (levelCode == 1) return new VipMember(id, name, phone, points, joinDate);
    if (levelCode == 2) return new SvipMember(id, name, phone, points, joinDate);
    return new RegularMember(id, name, phone, points, joinDate);
}
