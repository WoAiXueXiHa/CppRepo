// strong_weak_singal.cc
#include <iostream>

// 定义两个强符号，在.data中连续存放
int x = 10;
int y = 20;

extern void assign(); // 外部函数

int main()
{
    std::cout << "==== 前：x = " << x << ", y = " << y << std::endl;
    assign();
    std::cout << "==== 后：x = " << x << ", y = " << y << std::endl;
    return 0;
}