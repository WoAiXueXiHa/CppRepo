#include <iostream>
#include <string>

// // 左值和右值
// int main() {
//     int a = 10;
//     // a是左值，在栈上有确定的空间，有名字叫a
//     // 10是右值，只是寄存器里的一个临时数字，用完即销毁
//     std::cout << &a << std::endl;   // 左值可以取地址
//     // &10; // 右值不能取地址

//     std::string s1 = "hello";
//     std::string s2 = "world";
//     std::string s3 = s1 + s3;
//     std::cout << &s1 << " : " << &s2 << " : " << &s3 << std::endl;
//     // s1 s2 s3都是左值
//     // (s1+s2)产生了一个没有名字的临时字符串，是右值，赋值给3后就销毁

//     return 0;
// }

// // 左值引用和右值引用
// // 坑：右值引用变量本身，是一个左值！
// void func(int&& x) {
//     // x是什么值？
//     // 左值！
//     // 虽然x的类型是右值引用，但是在函数内部有了名字x，也有了固定的内存地址
//     std::cout << &x << std::endl;
// }
// int main() {
//     int a = 10;
//     int& ref1 = a;      // 正确：左值引用绑定到左值
//     // int& ref2 = 20;  // 错误：普通的左值引用不能绑到右值上

//     int&& r_ref1 = 20;  // 正确：右值引用绑定到右值，20被续命了，生命周期和r_ref1一样
//     // int&& r_ref2 = a;// 错误：右值引用不能绑定到活着的左值上

//     func(1);

//     return 0;
// }

// // 移动语义
// #include <cstring>
// class MyString {
// public:
//     char* data;

//     // 1. 传统的拷贝构造（极其昂贵）
//     // 遇到左值时触发：我要完整克隆一份你的数据
//     MyString(const MyString& other) {
//         std::cout << "触发深拷贝！耗时操作！\n";
//         data = new char[strlen(other.data) + 1];
//         strcpy(data, other.data);
//     }

//     // 2. 现代的移动构造（零开销抢劫）
//     // 遇到右值时自动触发：反正你马上要死了，你的堆内存直接归我了！
//     MyString(MyString&& other) noexcept {
//         std::cout << "触发移动语义！瞬间完成！\n";
//         data = other.data;      // 1. 偷走底层堆内存指针
//         other.data = nullptr;   // 2. 必须把原主人的指针置空，防止他析构时把内存炸了
//     }

//     ~MyString() { delete[] data; }
// };

// 完美转发
void process(int& x)  { std::cout << "左值处理（深拷贝）\n"; }
void process(int&& x) { std::cout << "右值处理（零开销抢劫）\n"; }

// // 作为一个极其平庸的中间商：
// template <typename T>
// void proxy(T&& arg) { 
//     // 灾难发生：arg 进了门，有了名字，它变成了左值！
//     // 无论外面传什么，这里永远只会调用 process(int& x)！
//     process(arg); 
// }

// int main() {
//     int a = 10;
//     proxy(a);          // 期望输出：左值处理 -> 实际输出：左值处理（没毛病）
//     proxy(20);         // 期望输出：右值处理 -> 实际输出：左值处理（血亏！性能暴跌！）
// }

// 优化：
template <typename T>
void proxy(T&& arg) { // T&& 是万能引用，吸纳一切
    // 使用 std::forward<T> 恢复其出厂设置，完美转交给下一层
    process(std::forward<T>(arg)); 
}

int main() {
    int a = 10;
    proxy(a);  // 输出：左值处理（完美）
    proxy(20); // 输出：右值处理（完美！成功触发移动语义！）
}