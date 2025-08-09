#pragma once
#include "define.h"

template <typename T>
class Container
{
public:
    Container() = default;
    // Container(Container<T> &c) = default;
    // Container(Container<T> &&c) = default;
    // Container &operator=(Container<T> &c) = default;
    // Container &operator=(Container<T> &&c) = default;

    Container(const T &t);
    // Container(T &&t);

    virtual typename T::iterator begin();
    virtual typename T::iterator end();

    virtual typename T::const_iterator cbegin() const;
    virtual typename T::const_iterator cend() const;

    virtual bool empty() const { return _data.empty(); }

    virtual T &get();
    virtual byte *raw();
    virtual size_t length() const;
    virtual void reserve(size_t size);
    virtual void resize(size_t size);

    

protected:
    T _data;
};

template <typename T>
inline Container<T>::Container(const T &t) : _data(t)
{
}

// template <typename T>
// inline Container<T>::Container(T &&t) : _data(t)
// {
// }

template <typename T>
inline typename T::iterator Container<T>::begin()
{
    return _data.begin();
}

template <typename T>
inline typename T::iterator Container<T>::end()
{
    return _data.end();
}

template <typename T>
inline typename T::const_iterator Container<T>::cbegin() const
{
    return _data.cbegin();
}

template <typename T>
inline typename T::const_iterator Container<T>::cend() const
{
    return _data.cend();
}

template <typename T>
inline byte *Container<T>::raw()
{
    return reinterpret_cast<byte *>(_data.data());
}

template <typename T>
inline T &Container<T>::get()
{
    return _data;
}

template <typename T>
inline size_t Container<T>::length() const
{
    return _data.size();
}

template <typename T>
inline void Container<T>::reserve(size_t size)
{
    _data.reserve(size);
}

template <typename T>
inline void Container<T>::resize(size_t size)
{
    _data.resize(size);
}
