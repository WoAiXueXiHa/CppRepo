#pragma once
/*
 * 仿函数（Functor）集中管理：
 * 1) 积分策略 PointsCalculator
 * 2) 日期范围筛选 DateRange（依赖 Transaction 的 dateKey）
 * 3) 降序排序 PayDescending（依赖对象有 pay 字段）
 */

namespace functor {

    // 积分策略：按实付 pay 每满 10 元积 1 分
    struct PointsCalculator {
        int operator()(double pay) const {
            if (pay <= 0) return 0;
            return (int)(pay / 10.0);
        }
    };

    // 日期范围：使用yyyymmdd int比较
    struct DateRange {
        int beginDate;
        int endDate;

        DateRange(int beginKey_, int endKey_)
            : beginDate(beginKey_), endDate(endKey_) {}

        template <typename T>
        bool operator()(const T &obj) const {
            return obj.dateKey >= beginKey && obj.dateKey <= endKey;
        }
    };

    // 按 pay 降序
    struct PayDescending {
        template <typename T>
        bool operator()(const T &a, const T &b) const {
            return a.pay > b.pay;
        }
    };
} 

