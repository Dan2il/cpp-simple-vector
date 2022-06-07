// Copyright 2022
// 18:19 07/06/2022

#pragma once

#include <algorithm>
#include <cassert>
#include <cstdlib>

template <typename Type>
class ArrayPtr {
 public:
    ArrayPtr() = default;

    explicit ArrayPtr(size_t size) {
        if (size != 0) {
            raw_ptr_ = new Type[size];
        }
    }

    explicit ArrayPtr(Type* raw_ptr) noexcept {
        raw_ptr_ = raw_ptr;
    }

    ArrayPtr(ArrayPtr&& raw_ptr) noexcept {
        if (this != raw_ptr) {
            ArrayPtr<Type> new_array_ptr(raw_ptr.Get());
            raw_ptr_ = new_array_ptr.Release();
        }
    }

    ArrayPtr& operator=(const ArrayPtr&& raw_ptr) {
        if (this != raw_ptr) {
            ArrayPtr<Type> new_array_ptr(raw_ptr);
            this->swap(new_array_ptr);
        }
    }

    ArrayPtr(const ArrayPtr&) = delete;
    ArrayPtr& operator=(const ArrayPtr&) = delete;


    ~ArrayPtr() {
        delete[] raw_ptr_;
    }

    [[nodiscard]] Type* Release() noexcept {
        Type *buffer_ptr = raw_ptr_;
        raw_ptr_ = nullptr;
        return buffer_ptr;
    }

    Type& operator[](size_t index) noexcept {
        return raw_ptr_[index];
    }

    const Type& operator[](size_t index) const noexcept {
        return raw_ptr_[index];
    }

    explicit operator bool() const {
        return raw_ptr_ != nullptr;
    }

    Type* Get() const noexcept {
        return raw_ptr_;
    }

    void swap(ArrayPtr& other) noexcept {
        std::swap(raw_ptr_, other.raw_ptr_);
    }

 private:
    Type* raw_ptr_ = nullptr;
};
