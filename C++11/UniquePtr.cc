#include <iostream>
template <typename T>
class UniquePtr {
private:
    T* _ptr;
public:
    // 构造获取资源
    UniquePtr(T* ptr) :_ptr(ptr) { }
    // 析构释放资源
    ~UniquePtr() { if(_ptr) delete _ptr; }

    // 禁止拷贝、赋值重载
    UniquePtr(const UniquePtr& other) = delete;
    UniquePtr& operator=(const UniquePtr& other) = delete;

    // 移动构造，_ptr <- other._ptr
    UniquePtr(UniquePtr&& other) :_ptr(other._ptr) { other._ptr = nullptr; }
    // 移动赋值，我接收别人的所有权，我先把我的资源扔了，再接收被人的资源
    UniquePtr& operator=(UniquePtr&& other) {
        if(this != &other) {
            if(_ptr) delete _ptr;
            _ptr = other._ptr;
            other._ptr = nullptr;
        }
        return *this;
    }
    // 指针行为
    // 解引用，取到具体的值
    T& operator*() const { return *_ptr; }
    // 找地址，取到地址
    T* operator->() const { return _ptr; }
    T* get() const { return _ptr; }
};