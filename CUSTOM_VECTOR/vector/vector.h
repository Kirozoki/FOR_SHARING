#pragma once
#include <cassert>
#include <cstdlib>
#include <new>
#include <utility>
#include <memory>
#include <algorithm>

template <typename T>
class RawMemory {
public:
    RawMemory() = default;

    explicit RawMemory(size_t capacity)
        : buffer_(Allocate(capacity))
        , capacity_(capacity) {
    }

    RawMemory(const RawMemory&) = delete;
    
    RawMemory& operator=(const RawMemory& rhs) = delete;
    
    RawMemory(RawMemory&& other) noexcept {
        buffer_ = other.buffer_;
        capacity_ = other.capacity_;
        other.buffer_ = nullptr;
        other.capacity_ = 0;
    }

    RawMemory& operator=(RawMemory&& rhs) noexcept {
        if (this != &rhs) {
            Deallocate(buffer_);
            capacity_ = 0;
            Swap(rhs);
        }
        return *this;
    }

    ~RawMemory() {
        Deallocate(buffer_);
    }

    T* operator+(size_t offset) noexcept {
        // Разрешается получать адрес ячейки памяти, следующей за последним элементом массива
        assert(offset <= capacity_);
        return buffer_ + offset;
    }

    const T* operator+(size_t offset) const noexcept {
        return const_cast<RawMemory&>(*this) + offset;
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<RawMemory&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < capacity_);
        return buffer_[index];
    }

    void Swap(RawMemory& other) noexcept {
        std::swap(buffer_, other.buffer_);
        std::swap(capacity_, other.capacity_);
    }

    const T* GetAddress() const noexcept {
        return buffer_;
    }

    T* GetAddress() noexcept {
        return buffer_;
    }

    size_t Capacity() const {
        return capacity_;
    }

private:
    // Выделяет сырую память под n элементов и возвращает указатель на неё
    static T* Allocate(size_t n) {
        return n != 0 ? static_cast<T*>(operator new(n * sizeof(T))) : nullptr;
    }

    // Освобождает сырую память, выделенную ранее по адресу buf при помощи Allocate
    static void Deallocate(T* buf) noexcept {
        operator delete(buf);
    }

    T* buffer_ = nullptr;
    size_t capacity_ = 0;
};

template <typename T>
class Vector {
public:

    using iterator = T*;
    using const_iterator = const T*;

    Vector() = default;

    explicit Vector(size_t size)
        : data_(size)
        , size_(size)
    {
        std::uninitialized_value_construct_n(data_.GetAddress(), size);
    }

    Vector(const Vector& other)
        : data_(other.size_)
        , size_(other.size_)
    {
        std::uninitialized_copy_n(other.data_.GetAddress(), size_, data_.GetAddress());
    }

    Vector& operator=(const Vector& rhs) {
        if (this != &rhs) {
            if (rhs.size_ > data_.Capacity()) {
                /* Применить copy-and-swap */
                Vector rhs_copy(rhs);
                Swap(rhs_copy);
            }
            else {
                /* Скопировать элементы из rhs, создав при необходимости новые
                   или удалив существующие */
                if (rhs.size_ < size_) {
                    std::copy(rhs.data_.GetAddress(), rhs.data_.GetAddress() + rhs.size_, data_.GetAddress());
                    std::destroy_n(data_.GetAddress() + rhs.size_, size_ - rhs.size_);
                }
                else {
                    std::copy(rhs.data_.GetAddress(), rhs.data_.GetAddress() + size_, data_.GetAddress());
                    std::uninitialized_copy_n(rhs.data_.GetAddress() + size_, rhs.size_ - size_, data_.GetAddress() + size_);
                }
                size_ = rhs.size_;
            }
        }
        return *this;
    }

    Vector(Vector&& other) noexcept {
        Swap(other);
    }

    Vector& operator=(Vector&& rhs) noexcept {
        if (this != &rhs) {
            Swap(rhs);
        }
        return *this;
    }

    ~Vector() {
        std::destroy_n(data_.GetAddress(), size_);
    }

    iterator begin() noexcept {
        return data_.GetAddress();
    }
    iterator end() noexcept {
        return data_ + size_;
    }
    const_iterator begin() const noexcept {
        return data_.GetAddress();
    }
    const_iterator end() const noexcept {
        return data_ + size_;
    }
    const_iterator cbegin() const noexcept {
        return data_.GetAddress();
    }
    const_iterator cend() const noexcept {
        return data_ + size_;
    }

    size_t Size() const noexcept {
        return size_;
    }

    size_t Capacity() const noexcept {
        return data_.Capacity();
    }

    const T& operator[](size_t index) const noexcept {
        return const_cast<Vector&>(*this)[index];
    }

    T& operator[](size_t index) noexcept {
        assert(index < size_);
        return data_[index];
    }

    void Reserve(size_t new_capacity) {
        if (new_capacity <= data_.Capacity()) {
            return;
        }
        RawMemory<T> new_data(new_capacity);
        // constexpr оператор if будет вычислен во время компиляции
        if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
            std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
        }
        else {
            std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
        }
        std::destroy_n(data_.GetAddress(), size_);
        data_.Swap(new_data);
    }

    void Swap(Vector& other) noexcept {
        data_.Swap(other.data_);
        std::swap(size_, other.size_);
    }

    void Resize(size_t new_size) {
        Reserve(new_size);
        if (size_ < new_size) {
            std::uninitialized_value_construct_n(data_.GetAddress() + size_, new_size - size_);
        } else {
            std::destroy_n(data_.GetAddress() + new_size, size_ - new_size);
        }
        size_ = new_size;
    }

    template <typename... Args>
    iterator Emplace(const_iterator pos, Args&&... value) {
        size_t index_pos = pos - begin();
        
        if (size_ < Capacity()) {
            
            if (pos != end()) {
                T temp_value(std::forward<Args>(value)...);
                new (data_ + size_) T(std::forward<T>(*(end() - 1)));
                std::move_backward(begin() + index_pos, end() - 1, end());
                *(begin() + index_pos) = std::move(temp_value);
            } else {
                new (end()) T(std::forward<Args>(value)...);
            }

        } else {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            
            new (new_data + index_pos) T(std::forward<Args>(value)...);

            iterator it = nullptr;
            
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                try {
                    it = std::uninitialized_move(begin(), begin() + index_pos, new_data.GetAddress());
                } catch (...) {
                    std::destroy_at(new_data + index_pos);
                    throw;
                }
                try {
                std::uninitialized_move(begin() + index_pos, end(), new_data.GetAddress() + index_pos + 1);
                } catch (...) {
                    std::destroy_n(new_data.GetAddress(), index_pos + 1);
                    throw;
                }
            } else {
                try {
                    it = std::uninitialized_copy(begin(), begin() + index_pos, new_data.GetAddress());
                } catch (...) {
                    std::destroy_at(new_data + index_pos);
                    throw;
                }
                try {
                    std::uninitialized_copy(begin() + index_pos, end(), new_data + index_pos + 1);
                } catch (...) {
                    std::destroy_n(new_data.GetAddress(), index_pos + 1);
                    throw;
                }                
            } 
            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data);
        }

        ++size_;
        return begin() + index_pos;
    }

    template <typename Type>
    iterator Insert(const_iterator pos, Type&& value) {    
        return Emplace(pos, std::forward<Type>(value));
    }

    template <typename Type>
    void PushBack(Type&& value) {
        if (size_ < Capacity()) {
            new(data_ + size_) T(std::forward<Type>(value));
        } else {
            RawMemory<T> new_data(size_ == 0 ? 1 : size_ * 2);
            new(new_data + size_) T(std::forward<Type>(value));
            
            if constexpr (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
                std::uninitialized_move_n(data_.GetAddress(), size_, new_data.GetAddress());
            } else {
                std::uninitialized_copy_n(data_.GetAddress(), size_, new_data.GetAddress());
            }
            std::destroy_n(data_.GetAddress(), size_);
            data_.Swap(new_data);
        }
        ++size_;
    }

    template <typename... Args>
    T& EmplaceBack(Args&&... value) {
        return *Emplace(end(), std::forward<Args>(value)...);
    }

    iterator Erase(const_iterator pos) {
        assert(pos >= begin() && pos < end());
        size_t index_pos = pos - begin();
            
        if (index_pos < size_ - 1) {
            std::move(begin() + index_pos + 1, end(), begin() + index_pos);
        }
        std::destroy_at(begin() + size_ - 1);
        --size_;
        return index_pos == size_ ? end() : begin() + index_pos;
    }

    void PopBack() {
        if (size_ > 0) {
            std::destroy_at(data_.GetAddress() + size_ - 1);
            --size_;
        }
    }

private:
    RawMemory<T> data_;
    size_t size_ = 0;
};