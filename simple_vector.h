#pragma once

#include <cassert>
#include <initializer_list>
#include <array>
#include <algorithm>
#include <iterator>

#include <iostream>

#include "array_ptr.h"

class ReserveProxyObj {
public:
    explicit ReserveProxyObj (size_t capacity_to_reserve) {
        capacity = capacity_to_reserve;
    }
    size_t capacity = 0;
};

template <typename Type>
class SimpleVector {
public:
    using Iterator = Type*;
    using ConstIterator = const Type*;

    SimpleVector() noexcept = default;

    explicit SimpleVector(size_t size) 
    : simple_vector_(size), size_simple_vector_(size), capacity_simple_vector_(size) {
        if (size) {
            std::fill(begin(), end(), 0);
            // ArrayPtr<Type>new_simple_vector(size);
            // simple_vector_.swap(new_simple_vector);
            // simple_vector_ = new ArrayPtr<Type>(size);
        }
    }

    SimpleVector(ReserveProxyObj reserv) {
        Reserve(reserv.capacity);
    }

    SimpleVector(size_t size, const Type& value)
    : size_simple_vector_(size), capacity_simple_vector_(size) {
        if (size) {
            ArrayPtr<Type>new_simple_vector(size);
            simple_vector_.swap(new_simple_vector);
            // simple_vector_ = new Type[size]{};
            std::fill(begin(), end(), value);
        }
    }

    SimpleVector(std::initializer_list<Type> init) 
     : size_simple_vector_(init.size()), capacity_simple_vector_(init.size()) {
        // вынести эти две строчки в фукцию
        ArrayPtr<Type>new_simple_vector(init.size());
        simple_vector_.swap(new_simple_vector);
        // simple_vector_ = new Type[init.size()]{};
        std::copy(init.begin(), init.end(), begin());
    }

    SimpleVector(const SimpleVector& other) {
        if (this != &other) {
            capacity_simple_vector_ = other.capacity_simple_vector_;
            size_simple_vector_ = other.size_simple_vector_;
            SimpleVector<Type> new_vector(size_simple_vector_);
            // ArrayPtr<Type>new_simple_vector(size_simple_vector_);
            // simple_vector_ = new Type[other.capacity_simple_vector_]{};
            std::copy(other.begin(), other.end(), new_vector.begin());
            simple_vector_.swap(new_vector.simple_vector_);
        }
    }

    SimpleVector(SimpleVector&& other) {
        size_simple_vector_ = other.size_simple_vector_;
        capacity_simple_vector_ = other.capacity_simple_vector_;
        SimpleVector<Type> new_vector(size_simple_vector_);
        std::move(other.begin(), other.end(), new_vector.begin());
        simple_vector_.swap(new_vector.simple_vector_);
        other.size_simple_vector_ = 0;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector buffer(rhs);
            this->swap(buffer);
        }
        return *this;

    }
    // 
    // ~SimpleVector() {
    //     delete[] simple_vector_;
    // }

    size_t GetSize() const noexcept {
        return size_simple_vector_;
    }

    size_t GetCapacity() const noexcept {
        return capacity_simple_vector_;
    }

    bool IsEmpty() const noexcept {
        return !size_simple_vector_;
    }

    Type& operator[](size_t index) noexcept {
        // assert(index < size_simple_vector_);
        return simple_vector_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        // assert(index < size_simple_vector_);
        return simple_vector_[index];
    }

    Type& At(size_t index) {
        if (index >= size_simple_vector_) {
            throw std::out_of_range("index >= size");
        }
        return simple_vector_[index];
    }

    const Type& At(size_t index) const {
        if (index >= size_simple_vector_) {
            throw std::out_of_range("index >= size");
        }
        return simple_vector_[index];
    }

    void Clear() noexcept {
        size_simple_vector_ = 0;
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity > capacity_simple_vector_) {
            SimpleVector new_vector(new_capacity);
            std::move(begin(), end(), new_vector.begin());
            simple_vector_.swap(new_vector.simple_vector_);
            std::exchange(capacity_simple_vector_, new_capacity);
        }
    }

    void Resize(size_t new_size) {

        // size_t counter = size_simple_vector_;
        // if (size_simple_vector_ > new_size) {
        //     size_simple_vector_ = new_size;
        // }
        // if (capacity_simple_vector_ > new_size) {
        //     Type check = {};
        //     for ( ; counter < new_size; counter++) {
        //         simple_vector_[counter] = std::move(check);
        //     }
        //     size_simple_vector_ = new_size;
        // } else {
        //     SimpleVector<Type> new_vector(new_size);
        //     std::move(begin(), end(), new_vector.begin());
        //     simple_vector_.swap(new_vector.simple_vector_);
        //     capacity_simple_vector_ = new_size;
        //     size_simple_vector_ = new_size;
        // }

        // полуработающая версия
        if (new_size < size_simple_vector_) {
            size_simple_vector_ = new_size;
        } else if (new_size > size_simple_vector_ && new_size < capacity_simple_vector_) {
            std::fill(std::move(begin() + size_simple_vector_), std::move(begin() + new_size), 0);
        } else if (new_size > capacity_simple_vector_) {
            SimpleVector<Type> new_vector(new_size);
            std::move(begin(), end(), new_vector.begin());
            simple_vector_.swap(new_vector.simple_vector_);
            capacity_simple_vector_ = new_size;
            size_simple_vector_ = new_size;
        }

        // исходная версия
        // size_t counter = size_simple_vector_;
        // if (size_simple_vector_ > new_size) {
        //     size_simple_vector_ = new_size;
        // }
        // if (capacity_simple_vector_ > new_size) {
        //     Type check = {};
        //     for ( ; counter < new_size; counter++) {
        //         simple_vector_[counter] = std::move(check);
        //     }
        //     size_simple_vector_ = new_size;
        // } else {
        //     Type *new_simple_vector = new Type[new_size]{};
        //     std::move(begin(), end(), new_simple_vector);
        //     delete[] simple_vector_;
        //     simple_vector_ = new_simple_vector;
        //     size_simple_vector_ = new_size;
        //     capacity_simple_vector_ = new_size;
        // }
    }

    void PushBack(const Type& item) {
        if (size_simple_vector_ >= capacity_simple_vector_ || capacity_simple_vector_ == 0) {
            size_t new_capacity_simple_vector_ = capacity_simple_vector_ == 0 ? 1 : capacity_simple_vector_ * 2;
            // ArrayPtr<Type> new_vector(capacity_simple_vector_);
            size_t buf_size = size_simple_vector_;
            this->Resize(new_capacity_simple_vector_);
            size_simple_vector_ = buf_size;
            // new_vector.swap(simple_vector_);
            // simple_vector_.swap(new_vector);
            // Type *new_simple_vector = new Type[new_capacity_simple_vector_]{};
            // std::copy(begin(), end(), new_simple_vector);
            // delete[] simple_vector_;
            // simple_vector_ = new_simple_vector;
            capacity_simple_vector_ = new_capacity_simple_vector_;
        }
        simple_vector_[size_simple_vector_++] = item;
    }

    void PushBack(Type&& item) {
        if (size_simple_vector_ >= capacity_simple_vector_ || capacity_simple_vector_ == 0) {
            size_t new_capacity_simple_vector_ = capacity_simple_vector_ == 0 ? 1 : capacity_simple_vector_ * 2;
            // ArrayPtr<Type> new_vector(capacity_simple_vector_);
            size_t buf_size = size_simple_vector_;
            this->Resize(new_capacity_simple_vector_);
            size_simple_vector_ = buf_size;
            // new_vector.swap(simple_vector_);
            // simple_vector_.swap(new_vector);
            // Type *new_simple_vector = new Type[new_capacity_simple_vector_]{};
            // std::copy(begin(), end(), new_simple_vector);
            // delete[] simple_vector_;
            // simple_vector_ = new_simple_vector;
            capacity_simple_vector_ = new_capacity_simple_vector_;
        }
        // auto it = begin() + size_simple_vector_;
        // *it = std::move(item);
        // size_simple_vector_++;
        simple_vector_[size_simple_vector_++] = std::move(item);
    }

    Iterator Insert(ConstIterator pos, const Type& value) {
        // assert(pos >= begin() && pos <= end());
        auto distance = pos - cbegin();
        auto buffer_for_size = size_simple_vector_;
        if (size_simple_vector_ >= capacity_simple_vector_) {
            capacity_simple_vector_ *= 2;
            Resize(capacity_simple_vector_);
            // SimpleVector<Type> new_vector(capacity_simple_vector_);
            // std::copy(begin(), end(), new_vector.begin());
            // simple_vector_.swap(new_vector.simple_vector_);
        }
        ++size_simple_vector_;
        Iterator pos_for_insert = simple_vector_.Get() + distance;
        std::copy_backward(pos_for_insert, simple_vector_.Get() + buffer_for_size, end());
        *pos_for_insert = value;
        return pos_for_insert;
    }

    Iterator Insert(ConstIterator pos, Type&& value) {
        auto distance = pos - cbegin();
        auto buffer_for_size = size_simple_vector_;
        if (size_simple_vector_ >= capacity_simple_vector_) {
            capacity_simple_vector_ *= 2;
            Resize(capacity_simple_vector_);
            // SimpleVector<Type> new_vector(capacity_simple_vector_);
            // std::move(begin(), end(), new_vector.begin());
            // simple_vector_.swap(new_vector.simple_vector_);
        }
        ++size_simple_vector_;
        Iterator pos_for_insert = simple_vector_.Get() + distance;
        std::move_backward(pos_for_insert, simple_vector_.Get() + buffer_for_size, end());
        *pos_for_insert = std::move(value);
        return pos_for_insert;
    }

    void PopBack() noexcept {
        if (size_simple_vector_ != 0 && capacity_simple_vector_ != 0) {
            --size_simple_vector_;
        }
    }

    Iterator Erase(ConstIterator pos) {
        SimpleVector<Type>::Iterator result = nullptr;
        if (pos != nullptr && capacity_simple_vector_) {
            SimpleVector<Type>::Iterator check = nullptr;
            for (size_t counter = 0; counter <= size_simple_vector_; counter++) {
                if (&simple_vector_[counter] == pos) {
                    result = &simple_vector_[counter];
                    ++counter;
                    check = &simple_vector_[counter];
                    break;
                }
            }
            std::move(check, end(), result);
            if (size_simple_vector_) {
                --size_simple_vector_;
            }
        }
        return result;
    }

    void swap(SimpleVector& other) noexcept {
        simple_vector_.swap(other.simple_vector_);
        std::swap(size_simple_vector_, other.size_simple_vector_);
        std::swap(capacity_simple_vector_, other.capacity_simple_vector_);


        // size_t size_buf_this = this->size_simple_vector_;
        // size_t capacity_buf_this = this->capacity_simple_vector_;
        // Type *buf_adress = this->simple_vector_;

        // this->size_simple_vector_ = other.size_simple_vector_;
        // this->capacity_simple_vector_ = other.capacity_simple_vector_;
        // this->simple_vector_ = other.simple_vector_;

        // other.size_simple_vector_ = size_buf_this;
        // other.capacity_simple_vector_ = capacity_buf_this;
        // other.simple_vector_ = buf_adress;
    }

    Iterator begin() noexcept {
        return Iterator{&simple_vector_[(size_t)0]};
    }

    Iterator end() noexcept {
        return Iterator{&simple_vector_[size_simple_vector_]};
    }

    ConstIterator begin() const noexcept {
        return ConstIterator{&simple_vector_[0]};
    }

    ConstIterator end() const noexcept {
        return ConstIterator{&simple_vector_[size_simple_vector_]};
    }

    ConstIterator cbegin() const noexcept {
        return ConstIterator{&simple_vector_[0]};
    }

    ConstIterator cend() const noexcept {
        return ConstIterator{&simple_vector_[size_simple_vector_]};
    }
private:
    // Type *simple_vector_ = nullptr;
    ArrayPtr<Type> simple_vector_;

    size_t size_simple_vector_ = 0;
    size_t capacity_simple_vector_ = 0;
};


ReserveProxyObj Reserve(size_t capacity_to_reserve) {
    return ReserveProxyObj(capacity_to_reserve);
}

template <typename Type>
inline bool operator==(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    bool check = true;
    if (rhs.GetSize() == lhs.GetSize()) {
            auto check_lhc = lhs.begin();
            auto check_rhs = rhs.begin();
            for (int counter = 0; static_cast<size_t>(counter) < rhs.GetSize() ; counter++) {
                if (*(check_lhc + counter) != *(check_rhs + counter)) {
                    check = false;
                    break;
                };
            }
    } else {
        check = false;
    }
    return check;
}

template <typename Type>
inline bool operator!=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    bool check = true;
    if (lhs == rhs) {
        check = false;
    }
    return check;
}

template <typename Type>
inline bool operator<(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    bool check = false;
    auto check_lhc = lhs.begin();
    auto check_rhs = rhs.begin();
    for (int counter = 0; static_cast<size_t>(counter) < rhs.GetSize() ; counter++) {
        if (*(check_lhc + counter) < *(check_rhs + counter)) {
            check = true;
            break;
        };
    }
    if (lhs.GetSize() < rhs.GetSize()) {
        check = true;
    }
    return check;
}

template <typename Type>
inline bool operator<=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs < rhs || lhs == rhs);
}

template <typename Type>
inline bool operator>(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
   bool check = false;
    auto check_lhc = lhs.begin();
    auto check_rhs = rhs.begin();
    for (int counter = 0; static_cast<size_t>(counter) < rhs.GetSize() ; counter++) {
        if (*(check_lhc + counter) > *(check_rhs + counter)) {
            check = true;
            break;
        };
    }
    if (lhs.GetSize() > rhs.GetSize()) {
        check = true;
    }
    return check;
}
template <typename Type>
inline bool operator>=(const SimpleVector<Type>& lhs, const SimpleVector<Type>& rhs) {
    return (lhs > rhs || lhs == rhs);;
}
