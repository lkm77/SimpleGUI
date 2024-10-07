#pragma once

namespace SimpleGUI {


    //不管理内存，只负责管理指针
    template <typename T>
    class Pointer
    {
    public:
        // 构造函数
        Pointer(T* ptr = nullptr) : ptr_(ptr) {}

        // 获取原始指针
        T* get() const
        {
            return ptr_;
        }

        // 检查指针是否有效
        bool isValid() const
        {
            return ptr_ != nullptr;
        }

        // 重载布尔转换运算符
        explicit operator bool() const
        {
            return isValid();
        }

        // 重载解引用运算符
        T& operator*() const
        {
            return *ptr_;
        }

        // 重载箭头运算符
        T* operator->() const
        {
            return ptr_;
        }

        // 重置指针
        void reset(T* newPtr) {
            ptr_ = newPtr;
        }

    private:
        T* ptr_; // 原始指针
    };
}