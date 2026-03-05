#include <iostream>
#include <memory>

struct LeakB;
struct LeakA {
    std::shared_ptr<LeakB> _ptr2B;
    LeakA() { std::cout << "LeakA 构造\n"; }
    ~LeakA() { std::cout << "LeakA 析构\n"; }
};

struct LeakB {
    std::shared_ptr<LeakA> _ptr2A;
    LeakB() { std::cout << "LeakB 构造\n"; }
    ~LeakB() { std::cout << "LeakB 析构\n"; }
};

void badFunc() {
    std::cout << "\n测试一开始，全是shared_ptr的循环引用 <<<\n";
    {
        std::shared_ptr<LeakA> a(new LeakA());  // A的计数=1
        std::shared_ptr<LeakB> b(new LeakB());  // B的计数=1

        a->_ptr2B = b;  // B的计数变2
        b->_ptr2A = a;  // A的计数变2
    }   // 离开作用域，泄漏了
    std::cout << ">>> 测试一结束，看到析构了吗？没有！内存泄露了！\n";
}

struct SafeB;
struct SafeA {
    std::shared_ptr<SafeB> _ptr2b;  // A强引用B
    SafeA() { std::cout << "SafeA 构造\n"; }
    ~SafeA() { std::cout << "SafeA 析构\n"; }
};

struct SafeB {
    std::weak_ptr<SafeA> _ptr2a;    // B弱引用A，B只是看着A，不增加计数
    SafeB() { std::cout << "SafeB 构造\n"; }
    ~SafeB() { std::cout << "SafeB 析构\n"; }
};

void goodFunc() {
    std::cout << "\n测试二开始，引入weak_ptr <<<\n";
    {
        std::shared_ptr<SafeA> a(new SafeA());  // A的计数=1
        std::shared_ptr<SafeB> b(new SafeB());  // B的计数=1

        a->_ptr2b = b;  // B的计数变2
        b->_ptr2a = a;  // A的计数不变
    }   // 离开作用域，安全回收
    std::cout << ">>> 测试二结束，触发析构，内存清空\n";
}

int main() {
    badFunc();
    goodFunc();

    return 0;
}