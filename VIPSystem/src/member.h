#pragma once
#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;
/*
 * 会员体系 继承 + 动态
 *  Member 为抽象父类：discountRate/levelName/levelCode/clone
 *  RegularMember / VipMember / SvipMember 为子类
 */

class Member {
protected:
    string mId;                // ID
    string mName;              // 姓名
    string mPhone;             // 手机号
    int         mPoints;            // 积分
    string mJoinDate;          // 入会时间

public:
    Member() : mPoints(0) {}

    Member(const string& id, const string &name, 
           const string& phone, int points, 
           const string& joinDate)
        :mId(id)
        ,mName(name)
        ,mPhone(phone)
        ,mPoints(points)
        ,mJoinDate(joinDate) 
        {}

    virtual ~Member(){}

    // 动态多态接口
    // 折扣率
    virtual double discountRate() const;
    // 会员级别
    virtual string levelName() const;
    // 会员级别编码
    virtual int levelCode() const;
    // 拷贝
    virtual Member* clone() const;

    // get函数
    const string& getId() const { return mId; }
    const string& getName() const { return mName; }
    const string& getPhone() const { return mPhone; }
    int getPoints() const { return mPoints; }
    const string& getJoinDate() const { return mJoinDate; }


    // set函数
    void setName(const string& name){ mName = name; }
    void setPhone(const string& phone){ mPhone = phone; }

    // 积分更新
    void addPoints(int delta) {
        mPoints += delta;
        if (mPoints < 0) 
            mPoints = 0;
    }

    // 写入文件一行
    string infoTxt() const {
    // id | name | phone | level | points | joinDate
    ostringstream oss;
    oss << mId << " | "
        << mName << " | "
        << mPhone << " | "
        << levelCode() << " | "
        << mPoints << " | "
        << mJoinDate;
    return oss.str();
}

    // 会员列表展示
    void printRow() const {
        // 处理控制默认值
        const string& showName 
            = mName.empty() ? "未知姓名" : mName;
        const string& showJoinDate 
            = mJoinDate.empty() ? "未知日期" : mJoinDate;
        // 格式化输出
        // 左对齐/右对齐结合 固定列宽 
        cout << left << setw(12) << mId          // 左对齐 列宽12
                  << left << setw(10) << showName     // 左对齐 列宽10
                  << left << setw(14) << mPhone       // 左对齐 列宽14
                  << left << setw(8)  << levelName()  // 左对齐 列宽8
                  << right << setw(8) << mPoints      // 右对齐 列宽8
                  << left << setw(12) << showJoinDate;// 左对齐 列宽12
        cout << "\n";
    }
};

// 常规会员
class RegularMember : public Member {
public:
    RegularMember(){}
    RegularMember(const string &id,
                  const string &name,
                  const string &phone,
                  int points,
                  const string &joinDate)
        :Member(id, name, phone, points, joinDate)
        {}

    virtual double discountRate() const { return 1.0; }
    virtual string levelName() const{ return "普通";}
    virtual int levelCode() const{ return 0; }
    virtual Member* clone() const{ return new RegularMember(*this); }
};

// VIP
class VipMember : public Member {
public:
    VipMember(){}
    VipMember(const string &id,
              const string &name,
              const string &phone,
              int points,
              const string &joinDate)
        :Member(id, name, phone, points, joinDate)
        {}
    virtual double discountRate() const{ return 0.95; }
    virtual string levelName() const{ return "VIP"; }
    virtual int levelCode() const{ return 1; }
    virtual Member* clone() const{ return new VipMember(*this); }
};

// 超级VIP
class SvipMember : public Member {
public:
    SvipMember(){}
    SvipMember(const string &id,
               const string &name,
               const string &phone,
               int points,
               const string &joinDate)
        :Member(id, name, phone, points, joinDate)
        {}

    virtual double discountRate() const{ return 0.85; }
    virtual string levelName() const{ return "SVIP"; }
    virtual int levelCode() const{ return 2; }
    virtual Member* clone() const{ return new SvipMember(*this); }
};

// 根据等级编码创建对应类型的派生类对象
// 主要用于从文件读取数据后 恢复对应类型的会员对象 不用直接实例化派生类
Member* createMemberByLevel(int levelCode,
                            const string &id,
                            const string &name,
                            const string &phone,
                            int points,
                            const string &joinDate){
    if (levelCode == 1) 
        return new VipMember(id, name, phone, points, joinDate);

    if (levelCode == 2) return 
        new SvipMember(id, name, phone, points, joinDate);

    return new RegularMember(id, name, phone, points, joinDate);                           
}

