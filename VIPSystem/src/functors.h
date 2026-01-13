#pragma once
/*
 * 仿函数 Functor
 * 1) 积分策略 PointsCalculator
 * 2) 日期范围筛选 DateRange（依赖 Transaction 的 dateKey）
 * 3) 降序排序 PayDescending（依赖对象有 pay 字段）
 */

namespace functor {

    struct PointsCalculator {
        int operator()(double pay) const {
            if (pay <= 0) return 0;
            return (int)(pay / 10.0);
        }
    };

    // 日期范围：使用 yyyymmdd int 比较
    struct DateRange {
        int beginKey;
        int endKey;

        DateRange(int beginKey_, int endKey_)
            : beginKey(beginKey_), endKey(endKey_) {}

        template <typename T>
        bool operator()(const T &obj) const {
            return obj.dateKey >= beginKey && obj.dateKey <= endKey;
        }
    };

    struct PayDescending {
        template <typename T>
        bool operator()(const T &a, const T &b) const {
            return a.pay > b.pay;
        }
    };

} 
