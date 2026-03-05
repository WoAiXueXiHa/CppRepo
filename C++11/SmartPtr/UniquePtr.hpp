#pragma once

// 手撕unique_ptr
// 本质是“独占所有权”--->我独居的公寓，钥匙只有一把，绝不共享
// C++98有个auto_ptr：核心是管理权限的转移，原本的资源直接释放
// 而实现unique_prt的核心就是“禁用拷贝，启用移动”

#include <iostream>

template <typename T>
class UniquePtr{
private:
    T* _ptr;
public:
    // 1. 构造时接管内存
    // explicit 禁止构造函数自动转换类型
    explicit UniquePtr(T* ptr) : _ptr(ptr) { }

    // 2. 析构时自动释放内存
    ~UniquePtr() { delete _ptr; }

    // 绝对禁止拷贝构造和赋值重载
    // 如果不禁用，两个UniquePtr内部的_ptr指向同一块内存，会析构两次
    UniquePtr(const UniquePtr&) = delete;
    UniquePtr& operator=(const UniquePtr&) = delete;

    // 4. 允许移动构造和移动赋值：转移资源所有全
    // noexcept 明确告诉编译器，整个函数绝对不会抛异常

    // 举个例子理解这两个函数
    // 一个指针管理一台车
    // 场景A有车钥匙，现在要把车过户给B
    UniquePtr(UniquePtr&& other) noexcept
        :_ptr(other._ptr)   // B拿到了A的车钥匙
    {
        // 必须把A的钥匙拿走
        other._ptr = nullptr;   // 把原对象的所有权置空，防止析构时释放内存
    }
    
    // A有一台保时捷，B原本就有一台奥迪，现在A把保时捷过户给B
    UniquePtr& operator=(UniquePtr&& other) noexcept{
        if(this != &other) {        // 自己过户给自己，啥也不干
            delete _ptr;            // 先释放自身原有资源，B得先把原来奥迪报废掉，不然停在车库没人管（内存泄漏）
            _ptr = other._ptr;      // 接管新资源，B拿到A的钥匙
            other._ptr = nullptr;   // 没收A的钥匙
        }
        return *this;
    }

    // 模拟指针行为
    // 解引用* 拿实体，要的是指针指向的那个东西本身
    T& operator*() const { return *_ptr; }
    // 成员访问符-> 拿动作，要调用整个对象的成员
    T* operator-> const { return _ptr; }
};