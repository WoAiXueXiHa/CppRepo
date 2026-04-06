template <typename T>
class SharedPtr {
private:
    T* _ptr;
    int* _cnt;  // 放到堆上，多个对象共享堆资源

    // 手动实现释放逻辑
    void Release() {
        if(0 == --(*_cnt)) {
            delete _ptr;
            delete _cnt;
        }
    }
public:
    // 构造获取资源，计数++
    SharedPtr(T* ptr = nullptr) : _ptr(ptr), _cnt(new int(1)) { }
    // 析构直接调用Release
    ~SharedPtr() { Release(); }

    // 拷贝构造和赋值
    SharedPtr(const SharedPtr& other) :_ptr(other._ptr), _cnt(other._cnt) {
        ++(*_cnt);
    }
    SharedPtr& operator=(const SharedPtr& other) {
        if(this != &other) {
            // other覆盖_ptr
            Release();
            _ptr = other._ptr;
            _cnt = other._cnt;
            ++(*_cnt);
        }
        return *this;
    }

    // 指针行为
    T& operator*() const { return *_ptr; }
    T* operator->() const { return _ptr; }
    T* get() const { return _ptr; }
    int cnt() const { return *_cnt; }
};

#include <memory>


// // 循环引用问题
// class B;
// class A {
// public:
//     std::shared_ptr<B> _pb;
// };
// class B {
// public:
//     std::shared_ptr<A> _pa;
// };
// int main() {
//     std::shared_ptr<A> a(new A());
//     std::shared_ptr<B> b(new B());
//     a->_pb = b;
//     b->_pa = a;
//     return 0;
// }

// weak_ptr解决循环引用
class B;
class A {
public:
    std::shared_ptr<B> _pb;
};

class B {
public:
    std::weak_ptr<A> _pa;
};

int main() {
    // A 强计数1 B 强计数1
    std::shared_ptr<A> a(new A());
    std::shared_ptr<B> b(new B());

    // B 强计数2
    a->_pb = b; // b赋给_pb了，_pb是shared，B的强计数+1
    // A 强计数1 弱计数1
    b->_pa = a; // a赋给_pa了，_pa是weak，A的强计数不变
    return 0;
}