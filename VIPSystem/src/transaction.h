#pragma once
#include <string>
#include <sstream>
#include <iomanip>

using namespace std;

/*
 * 消费记录 文件存储 + 查询展示
 * - dateKey：yyyymmdd 
 */

class Transaction {
public:
    long   transactionId;
    string memberId;
    string date;
    int    dateKey;
    string item;
    double amount;
    double pay;
    int    pointsEarned;

public:
    Transaction()
        : transactionId(0), dateKey(0), amount(0.0), pay(0.0), pointsEarned(0) {}

    // transactionId | memberId | date | item | amount | pay | pointsEarned
    string infoTxt() const {
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
