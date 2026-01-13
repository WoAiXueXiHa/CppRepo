#pragma once
#include <string>

/*
 * 一条消费记录
 *  date: 用于展示/保存（YYYY-MM-DD）
 *  dateKey: 用于比较/筛选（yyyymmdd）
 */

class Transaction {
public:
    long        transactionId; // 自增交易号
    string memberId;      // 会员号
    string date;          // YYYY-MM-DD
    int         dateKey;       // yyyymmdd
    string item;          // 商品/备注
    double      amount;        // 原价
    double      pay;           // 实付
    int         pointsEarned;  // 本次获得积分

public:
   
    Transaction()
        : transactionId(0)
        ,dateKey(0)
        ,amount(0.0)
        ,pay(0.0)
        ,pointsEarned(0) 
        {}

    // 写入 txt文件
    // transactionId | memberId | date | item | amount | pay | pointsEarned
    string infoTxt() const{
        ostringstream oss;
        oss << transactionId << " | "
            << memberId << " | "
            << date << " | "
            << item << " | "
            << fixed << setprecision(2) << amount << " | "
            << fixed << setprecision(2) << pay << " | "
            << pointsEarned;
        return oss.str();
    }
};

