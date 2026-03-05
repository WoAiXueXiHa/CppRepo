#include <iostream>
#include <functional>

using namespace std;
using namespace std::placeholders;  // 占位符

// 本质：一个函数模板，把“函数+部分/参数”打包成一个可以直接调用的新对象

// =========================== 用法演示 =========================
void printSum(int a, int b) {
    cout << a << "+" << b << " = " << a + b << endl;
}

class MyClass {
public:
    int val = 10;
    void printVal(int x) {
        cout << "Class val: " << val << ", x: " << x << endl;
    }
};

int main() {
    // 用法一：绑定普通安徽念书，固定部分参数
    // 我只有一个参数，但是原函数要两个参数，提前把第一个参数绑死为1024
    // _1代表调用f1时传入的第一个参数 
    auto f1 = std::bind(printSum, 1024, _1);
    f1(50); // printSum(1024, 50)

    // 用法二：改变参数顺序
    // 原函数先传a再传b，我把它反过来
    auto f2 = std::bind(printSum, _2, _1);
    f2(10, 20); // printSum(20, 10)

    // 用法三：绑定类成员函数
    // 类成员函数包含this指针，第二个参数必须传对象地址，后面再跟函数参数
    MyClass obj;
    auto f3 = std::bind(&MyClass::printVal, &obj, _1);
    f3(99); obj.printVal(99);

    // 用法四：绑定成员变量
    // 直接把对象的变量提取成一个可以调用的函数对象
    auto f4 = std::bind(&MyClass::val, &obj);
    cout << "Member value: " << f4() << endl; // obj.val

    // 用法五：打破默认的值拷贝
    // bind默认把参数拷贝一份存起来，外部修改无效，想传引用必须加std::ref
    int num = 5;
    auto f5 = std::bind(printSum, std::ref(num), _1);
    num = 1111;
    f5(1);  // printSum(1111, 1) 如果不加引用，就是printSum(5, 1)

    return 0;
}