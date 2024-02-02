//
// Created by Peter Cheung on 2024-02-02.
//
#pragma once


#include <functional>

template<typename T>
class smart_ptr {
public:
    smart_ptr()
            : p_object(nullptr), sp_use_count(new unsigned(1)) {};

    explicit smart_ptr(T *p) : p_object(p), sp_use_count(new unsigned(1)) {};

    smart_ptr(const smart_ptr &rhs) : p_object(rhs.p_object), sp_use_count(rhs.sp_use_count), m_del(rhs.m_del) {};

    smart_ptr<T> &operator=(const smart_ptr &);

    T &operator*() const;

    T *operator->() const;

    ~smart_ptr();

    explicit operator bool() const;

    bool unique();

    void reset();

    void reset(T *);

    void reset(T *, std::function<void(T *)>);

    T *release();

    T *get() const;

private:
    static std::function<void(T *)> default_del;
    std::function<void(T *)> m_del = default_del;
    unsigned *sp_use_count = nullptr;
    T *p_object = nullptr;
};

template<typename T>
inline T *smart_ptr<T>::get() const {
    return p_object;
}

template<typename T>
inline T *smart_ptr<T>::release() {
    auto temp = p_object; // 保存原始指针
    p_object = nullptr; // 将智能指针内的指针设为nullptr
    if (--(*sp_use_count) == 0) {
        delete sp_use_count; // 如果没有其他智能指针指向对象，则释放引用计数
        sp_use_count = nullptr;
    }
    return temp; // 返回原始指针
}

template<typename T>
inline void smart_ptr<T>::reset(T *p, std::function<void(T *)> del) {
    (*sp_use_count)--;
    if (*sp_use_count == 0) {
        m_del(p_object);
    }
    p_object = p;
    *sp_use_count = 1;
    m_del = del;
}

template<typename T>
inline void smart_ptr<T>::reset(T *p) {
    (*sp_use_count)--;
    if (*sp_use_count == 0) {
        m_del(p_object);
    }
    p_object = p;
    *sp_use_count = 1;
    m_del = default_del;

}

template<typename T>
inline void smart_ptr<T>::reset() {
    (*sp_use_count)--;
    if (*sp_use_count == 0) {
        m_del(p_object);
    }
    p_object = nullptr;
    *sp_use_count = 1;
    m_del = default_del;


}

template<typename T>
inline bool smart_ptr<T>::unique() {
    return *sp_use_count == 1;
}

template<typename T>
inline smart_ptr<T>::operator bool() const {
    return p_object != nullptr;
}

template<typename T>
inline smart_ptr<T>::~smart_ptr() {
    if (--(*sp_use_count) == 0) {
        m_del(p_object);
        p_object = nullptr;
        delete sp_use_count;
        p_object = nullptr;
    }
}

template<typename T>
inline T *smart_ptr<T>::operator->() const {
    return p_object;
}

template<typename T>
inline T &smart_ptr<T>::operator*() const {
    return *p_object;
}

template<typename T>
inline smart_ptr<T> &smart_ptr<T>::operator=(const smart_ptr &rhs) {
    m_del = rhs.m_del;
    ++(*rhs.sp_use_count);

    if (--(*sp_use_count) == 0) {
        m_del(p_object);
        delete sp_use_count;
    }
    sp_use_count = rhs.sp_use_count;
    p_object = rhs.p_object;
    return *this;
}

template<typename T>
std::function<void(T *)> smart_ptr<T>::default_del = [](T *ptr) {
    delete ptr;
};







