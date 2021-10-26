#ifndef COLLECTION_HPP
#define COLLECTION_HPP

#include <cstddef> // size_t
#include <initializer_list> // initializer_list

template <typename T>
class Collection
{
private:
    T* data_;
    std::size_t size_;

public:
    Collection() noexcept;
    explicit Collection(std::size_t size_);
    Collection(const Collection<T>&);
    Collection(Collection<T>&&) noexcept;
    Collection(const std::initializer_list<T>&);

    Collection<T>& operator= (const Collection<T>&);
    Collection<T>& operator= (Collection<T>&&) noexcept;

    std::size_t size() const noexcept;

    T& operator[] (std::size_t i) noexcept;
    const T& operator[] (std::size_t i) const noexcept;
};

template <typename T>
inline Collection<T>::Collection() noexcept : data_(nullptr), size_(0)
{
}

template <typename T>
inline Collection<T>::Collection(std::size_t size) : data_(new T [size]), size_(size)
{
}

template <typename T>
Collection<T>::Collection(const Collection<T>& collection)
    : data_(new T[collection.size_]), size_(collection.size_)
{
    for(std::size_t i = 0; i < size_; ++i)
        data_[i] = collection.data_[i];
}

template <typename T>
Collection<T>::Collection(Collection<T>&& collection) noexcept
{
    size_ = collection.size_;
    data_ = collection.data_;

    collection.data_ = nullptr;
}

template <typename T>
Collection<T>::Collection(const std::initializer_list<T>& list)
    : data_(new T[list.size()]), size_(list.size())
{
    std::size_t i = 0;
    for(const auto& arg: list)
    {
        data_[i] = arg;
        ++i;
    }
}


template <typename T>
Collection<T>& Collection<T>::operator= (const Collection<T>& collection)
{
    if(this == &collection)
        return *this;

    size_ = collection.size_;
    data_ = new T[size_];

    for(std::size_t i = 0; i < size_; ++i)
        data_[i] = collection.data_[i];

    return *this;
}

template <typename T>
Collection<T>& Collection<T>::operator= (Collection<T>&& collection) noexcept
{
    if(this == &collection)
        return *this;

    size_ = collection.size_;
    data_ = collection.data_;

    collection.data_ = nullptr;

    return *this;
}

template <typename T>
inline std::size_t Collection<T>::size() const noexcept
{
    return size_;
}

template <typename T>
inline T& Collection<T>::operator[] (std::size_t i) noexcept
{
    return data_[i];
}
template <typename T>
inline const T& Collection<T>::operator[] (std::size_t i) const noexcept
{
    return data_[i];
}

#endif // COLLECTION_HPP