/*
 * Copyright (C) 2014 Pavel Kirienko <pavel.kirienko@gmail.com>
 */

#pragma once

#include <cstdlib>
#include <uavcan/error.hpp>
#include <uavcan/build_config.hpp>
#include <uavcan/util/templates.hpp>

#ifndef UAVCAN_CPP_VERSION
# error UAVCAN_CPP_VERSION
#endif

namespace uavcan
{
/**
 * This class allows to postpone the object contruction.
 * It statically allocates a pool of memory of just enough size to fit the object being constructed.
 * Later call to construct<>() calls the constructor of the object.
 * The object will be destroyed automatically when the container class destructor is called.
 * The memory pool is aligned at the size of the largest primitive type (long double or long long int).
 */
template <typename T>
class UAVCAN_EXPORT LazyConstructor
{
    union
    {
        unsigned char pool[sizeof(T)];
        long double _aligner1;
        long long _aligner2;
    } data_;

    T* ptr_;

    void ensureConstructed() const
    {
        if (!ptr_)
        {
            handleFatalError("LazyConstructor<T>");
        }
    }

    void ensureNotConstructed() const
    {
        if (ptr_)
        {
            handleFatalError("LazyConstructor<T>");
        }
    }

    template <typename U> struct ParamType { typedef const U& Type; };
    template <typename U> struct ParamType<U&> { typedef U& Type; };
#if UAVCAN_CPP_VERSION >= UAVCAN_CPP11
    template <typename U> struct ParamType<U&&> { typedef U&& Type; };
#endif

public:
    LazyConstructor()
        : ptr_(NULL)
    {
        fill(data_.pool, data_.pool + sizeof(T), uint8_t(0));
    }

    LazyConstructor(const LazyConstructor<T>& rhs)   // Implicit
        : ptr_(NULL)
    {
        fill(data_.pool, data_.pool + sizeof(T), uint8_t(0));
        if (rhs)
        {
            construct<const T&>(*rhs);  // Invoke copy constructor
        }
    }

    ~LazyConstructor() { destroy(); }

    LazyConstructor<T>& operator=(const LazyConstructor<T>& rhs)
    {
        destroy();
        if (rhs)
        {
            construct<const T&>(*rhs);  // Invoke copy constructor
        }
        return *this;
    }

    bool isConstructed() const { return ptr_ != NULL; }

    operator T*() const { return ptr_; }

    const T* operator->() const { ensureConstructed(); return ptr_; }
    T*       operator->()       { ensureConstructed(); return ptr_; }

    const T& operator*() const { ensureConstructed(); return *ptr_; }
    T&       operator*()       { ensureConstructed(); return *ptr_; }

    void destroy()
    {
        if (ptr_)
        {
            ptr_->~T();
        }
        ptr_ = NULL;
        fill(data_.pool, data_.pool + sizeof(T), uint8_t(0));
    }

    void construct()
    {
        ensureNotConstructed();
        ptr_ = new (static_cast<void*>(data_.pool)) T();
    }

// MAX_ARGS = 6
// TEMPLATE = '''
//     template <%s>
//     void construct(%s)
//     {
//         ensureNotConstructed();
//         ptr_ = new (static_cast<void*>(data_.pool)) T(%s);
//     }'''
// for nargs in range(1, MAX_ARGS + 1):
//     nums = [(x + 1) for x in range(nargs)]
//     l1 = ['typename P%d' % x for x in nums]
//     l2 = ['typename ParamType<P%d>::Type p%d' % (x, x) for x in nums]
//     l3 = ['p%d' % x for x in nums]
//     print(TEMPLATE % (', '.join(l1), ', '.join(l2), ', '.join(l3)))

    template <typename P1>
    void construct(typename ParamType<P1>::Type p1)
    {
        ensureNotConstructed();
        ptr_ = new (static_cast<void*>(data_.pool)) T(p1);
    }

    template <typename P1, typename P2>
    void construct(typename ParamType<P1>::Type p1, typename ParamType<P2>::Type p2)
    {
        ensureNotConstructed();
        ptr_ = new (static_cast<void*>(data_.pool)) T(p1, p2);
    }

    template <typename P1, typename P2, typename P3>
    void construct(typename ParamType<P1>::Type p1, typename ParamType<P2>::Type p2, typename ParamType<P3>::Type p3)
    {
        ensureNotConstructed();
        ptr_ = new (static_cast<void*>(data_.pool)) T(p1, p2, p3);
    }

    template <typename P1, typename P2, typename P3, typename P4>
    void construct(typename ParamType<P1>::Type p1, typename ParamType<P2>::Type p2, typename ParamType<P3>::Type p3,
                   typename ParamType<P4>::Type p4)
    {
        ensureNotConstructed();
        ptr_ = new (static_cast<void*>(data_.pool)) T(p1, p2, p3, p4);
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5>
    void construct(typename ParamType<P1>::Type p1, typename ParamType<P2>::Type p2, typename ParamType<P3>::Type p3,
                   typename ParamType<P4>::Type p4, typename ParamType<P5>::Type p5)
    {
        ensureNotConstructed();
        ptr_ = new (static_cast<void*>(data_.pool)) T(p1, p2, p3, p4, p5);
    }

    template <typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
    void construct(typename ParamType<P1>::Type p1, typename ParamType<P2>::Type p2, typename ParamType<P3>::Type p3,
                   typename ParamType<P4>::Type p4, typename ParamType<P5>::Type p5, typename ParamType<P6>::Type p6)
    {
        ensureNotConstructed();
        ptr_ = new (static_cast<void*>(data_.pool)) T(p1, p2, p3, p4, p5, p6);
    }
};

}
