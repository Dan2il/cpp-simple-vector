#pragma once

#include <cassert>
#include <initializer_list>
#include <array>
#include <algorithm>
#include <iterator>

#include <iostream>

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
    : size_simple_vector_(size), capacity_simple_vector_(size) {
        if (size) {
            simple_vector_ = new Type[size]{};
        }
    }

    SimpleVector(ReserveProxyObj reserv) {
        Reserve(reserv.capacity);
    }

    SimpleVector(size_t size, const Type& value)
    : size_simple_vector_(size), capacity_simple_vector_(size) {
        if (size) {
            simple_vector_ = new Type[size]{};
            std::fill(begin(), end(), value);
        }
    }

    SimpleVector(std::initializer_list<Type> init) 
     : size_simple_vector_(init.size()), capacity_simple_vector_(init.size()) {
        simple_vector_ = new Type[init.size()]{};
        std::copy(init.begin(), init.end(), begin());
    }

    SimpleVector(const SimpleVector& other) {
        if (this != &other) {
            capacity_simple_vector_ = other.capacity_simple_vector_;
            size_simple_vector_ = other.size_simple_vector_;
            simple_vector_ = new Type[other.capacity_simple_vector_]{};
            std::copy(other.begin(), other.end(), begin());
        }
    }

    SimpleVector(SimpleVector&& other)
    : simple_vector_(other.simple_vector_), size_simple_vector_(other.size_simple_vector_), \
        capacity_simple_vector_(other.capacity_simple_vector_) {
        other.simple_vector_ = nullptr;
        other.size_simple_vector_ = 0;
    }

    SimpleVector& operator=(const SimpleVector& rhs) {
        if (this != &rhs) {
            SimpleVector buffer(rhs);
            this->swap(buffer);
        }
        return *this;

    }

    ~SimpleVector() {
        delete[] simple_vector_;
    }

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
        return simple_vector_[index];
    }

    const Type& operator[](size_t index) const noexcept {
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
        if (capacity_simple_vector_ < new_capacity) {
            Type *new_simple_vector = new Type[new_capacity]{};
            if (capacity_simple_vector_ != 0) {
                std::copy(begin(), end(), new_simple_vector);
            }
            delete[] simple_vector_;
            simple_vector_ = new_simple_vector;
            capacity_simple_vector_ = new_capacity;
        }
    }

    void Resize(size_t new_size) {
        size_t counter = size_simple_vector_;
        if (size_simple_vector_ > new_size) {
            size_simple_vector_ = new_size;
        }
        if (capacity_simple_vector_ > new_size) {
            Type check = {};
            for ( ; counter < new_size; counter++) {
                simple_vector_[counter] = std::move(check);
            }
            size_simple_vector_ = new_size;
        } else {
            Type *new_simple_vector = new Type[new_size]{};
            std::move(begin(), end(), new_simple_vector);
            delete[] simple_vector_;
            simple_vector_ = new_simple_vector;
            size_simple_vector_ = new_size;
            capacity_simple_vector_ = new_size;
        }
    }

    void PushBack(const Type& item) {
        if (size_simple_vector_ >= capacity_simple_vector_) {
            size_t new_capacity_simple_vector_ = capacity_simple_vector_ == 0 ? 1 : capacity_simple_vector_ * 2;
            Type *new_simple_vector = new Type[new_capacity_simple_vector_]{};
            std::copy(begin(), end(), new_simple_vector);
            delete[] simple_vector_;
            simple_vector_ = new_simple_vector;
            capacity_simple_vector_ = new_capacity_simple_vector_;
        }
        simple_vector_[size_simple_vector_++] = item;

    }

    void PushBack(Type&& item) {
        if (size_simple_vector_ >= capacity_simple_vector_) {
            size_t new_capacity_simple_vector_ = capacity_simple_vector_ == 0 ? 1 : capacity_simple_vector_ * 2;
            Type *new_simple_vector = new Type[new_capacity_simple_vector_]{};
            std::move(begin(), end(), new_simple_vector);
            delete[] simple_vector_;
            simple_vector_ = new_simple_vector;
            capacity_simple_vector_ = new_capacity_simple_vector_;
        }
        simple_vector_[size_simple_vector_++] = std::move(item);
    }

    Iterator Insert(ConstIterator pos, const Type& value) {

        SimpleVector<Type>::Iterator no_const_pos = const_cast<Iterator>(pos);
        int pos_element = std::distance(begin(), no_const_pos);
        
        if (size_simple_vector_ < capacity_simple_vector_) {
            std::move_backward(no_const_pos, end(), &simple_vector_[(size_simple_vector_ + 1)]);
            simple_vector_[pos_element] = std::move(value);
        } else {
            size_t new_capacity = std::max(size_t(1), 2 * capacity_simple_vector_);
            Type *new_simple_vector = new Type[new_capacity]{};
            std::move(simple_vector_, &simple_vector_[pos_element], new_simple_vector);
            std::move_backward(no_const_pos, end(), &new_simple_vector[(size_simple_vector_ + 1)]);
            new_simple_vector[pos_element] = std::move(value);
            delete[] simple_vector_;
            simple_vector_ = new_simple_vector;
            capacity_simple_vector_ = new_capacity;
        }
        
        ++size_simple_vector_;
        return Iterator{&simple_vector_[pos_element]};
    }

    Iterator Insert(ConstIterator pos, Type&& value) {

        SimpleVector<Type>::Iterator no_const_pos = const_cast<Iterator>(pos);
        int pos_element = std::distance(begin(), no_const_pos);
        
        if (size_simple_vector_ < capacity_simple_vector_) {
            std::move_backward(no_const_pos, end(), &simple_vector_[(size_simple_vector_ + 1)]);
            simple_vector_[pos_element] = std::move(value);
        } else {
            size_t new_capacity = std::max(size_t(1), 2 * capacity_simple_vector_);
            Type *new_simple_vector = new Type[new_capacity]{};
            std::move(simple_vector_, &simple_vector_[pos_element], new_simple_vector);
            std::move_backward(no_const_pos, end(), &new_simple_vector[(size_simple_vector_ + 1)]);
            new_simple_vector[pos_element] = std::move(value);
            delete[] simple_vector_;
            simple_vector_ = new_simple_vector;
            capacity_simple_vector_ = new_capacity;
        }
        
        ++size_simple_vector_;
        return Iterator{&simple_vector_[pos_element]};
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
        size_t size_buf_this = this->size_simple_vector_;
        size_t capacity_buf_this = this->capacity_simple_vector_;
        Type *buf_adress = this->simple_vector_;

        this->size_simple_vector_ = other.size_simple_vector_;
        this->capacity_simple_vector_ = other.capacity_simple_vector_;
        this->simple_vector_ = other.simple_vector_;

        other.size_simple_vector_ = size_buf_this;
        other.capacity_simple_vector_ = capacity_buf_this;
        other.simple_vector_ = buf_adress;
    }

    Iterator begin() noexcept {
        return Iterator{&simple_vector_[(size_t)0]};
    }

    Iterator end() noexcept {
        return Iterator{&simple_vector_[size_simple_vector_]};
    }

    ConstIterator begin() const noexcept {
        return Iterator{&simple_vector_[0]};
    }

    ConstIterator end() const noexcept {
        return Iterator{&simple_vector_[size_simple_vector_]};
    }

    ConstIterator cbegin() const noexcept {
        return Iterator{&simple_vector_[0]};
    }

    ConstIterator cend() const noexcept {
        return Iterator{&simple_vector_[size_simple_vector_]};
    }
private:
    Type *simple_vector_ = nullptr;
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
