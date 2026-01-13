#pragma once

/*
 * 仿函数（Functor）
 * - PointsCalculator：积分策略（按实付每满10元积1分）
 */
namespace functor {

    struct PointsCalculator {
        int operator()(double pay) const {
            if (pay <= 0) return 0;
            // static_cast<int> 向下取整 68.0->6
            return static_cast<int>(pay / 10.0);
        }
    };

} 
