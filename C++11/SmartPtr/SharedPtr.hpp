#pragma once

// 共享所有权，大学宿舍，大家都有钥匙，
// 最后退宿的人负责锁门并归还钥匙
// 外部维护一块引用计数实现
// 核心：提取公共的引用计数块，确保所有拷贝的智能指针都能看到同一个计数器

#include <iostream>

template <typename T>
class SharedPtr {
private:
    T* _ptr;
    int* _ref_cnt;   // 必须分配到堆上，让所有副本共享同一块计数内存

    // 封装释放逻辑
    void release() {
        if(_ref_cnt) (*_ref_cnt)--;
        if(*_ref_cnt == 0) {
            delete _ptr;
            delete _ref_cnt;
        }
    }
public:
    // 1. 构造：分配资源和计数器
    explicit SharedPtr(T* ptr = nullptr)
        :_ptr(ptr)
    {
        if(_ptr) _ref_cnt = new int(1);
        else _ref_cnt = nullptr;
    }

    // 2. 拷贝构造：共享资源，计数+1
    SharedPtr(const SharedPtr& other) 
        :_ptr(other._ptr)
        ,_ref_cnt(other._ref_cnt)
    {
        if(_ref_cnt) (*_ref_cnt)++;
    }

    // 3. 赋值重载
    SharedPtr& operator=(const SharedPtr& other) {
        if(this != &other)  {
            release();  // 释放自身指向的旧资源：旧计数-1
            _ptr = other._ptr;
            _ref_cnt = other._ref_cnt;
            if(_ref_cnt) (*_ref_cnt)++; // 新计数+1
        }
        return *this;
    }

    // 4. 析构：最后一个人还钥匙
    ~SharedPtr() { release(); }

    T* operator->() const { return _ptr; }
    T& operator*() const { return *_ptr; }
};